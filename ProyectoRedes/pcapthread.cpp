#include "pcapthread.h"

TablePacket *tablePacketThread;

int countPacket = 0;
int countNodes  =-1;
QHash<QString,int> hashNodos;
QHash<QString,Conexion> hashConexiones;
double tiempoBase;

fstream trace("../ProyectoRedes/Proyectopy/trazaReal.tr",ios::out);

// Calcular tiempo
float calculo_time(const struct pcap_pkthdr *header)
{

    if( countPacket == 1 ) {
        tiempoBase = header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;
        cout << "Tiempo prueba: " << tiempoBase << " ENTRA!!" << endl;
    }
    double tiempoNuevo=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;

    printf("tiempo con printf %.16g segundos\n",tiempoNuevo-tiempoBase);

    return tiempoNuevo-tiempoBase;
}

// Captura de paquetes
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

    /* declare pointers to packet headers */
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_tcp *tcp;            /* The TCP header */

    int size_ip;
    int size_tcp;

    struct tm *ltime;
    char timestr[16];
    time_t local_tv_sec;


    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    ltime = localtime(&local_tv_sec);
    strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);



    printf("\n------------------------------Packet number %d:------------------------------\n", countPacket);
    printf("Tiempo : %s.%.6d len:%d \n", timestr, header->ts.tv_usec, header->len);       /* print timestamp and length of the packet */




    countPacket++;
    //------------------------------------------Extraemos Todas las cabeceras para ser procesadas-----------------------
    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;

    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }




    /* determine protocol */
    switch(ip->ip_p) {
    case IPPROTO_TCP:
        printf("   Protocol: TCP\n");
        break;
    case IPPROTO_UDP:
        printf("   Protocol: UDP\n");
        return;
    case IPPROTO_ICMP:
        printf("   Protocol: ICMP\n");
        return;
    case IPPROTO_IP:
        printf("   Protocol: IP\n");
        return;
    default:
        printf("   Protocol: unknown\n");
        return;
    }

    /* define/compute tcp header offset */
    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }

    //--------------------------------------------Imprimiendo 4tupla----------------------------------------------------
    /* print source and destination IP addresses */
    printf("   From: %s\n", inet_ntoa(ip->ip_src));
    printf("   To: %s\n", inet_ntoa(ip->ip_dst));

    /* print source and destination PORT */
    printf("   Src port: %d\n", ntohs(tcp->th_sport));
    printf("   Dst port: %d\n", ntohs(tcp->th_dport));


    //--------------------------------------------USANDO LAS HASH---------------------------------------


    QString *numSrcIP,*numTgtIP;
    QString nodo1,nodo2; //nodos estan compuestos de su IP + numero de Puerto
    QString key;

    numSrcIP=new QString(inet_ntoa(ip->ip_src));
    numSrcIP->append(ntohs(tcp->th_sport));


    if(!hashNodos.contains(*numSrcIP))
        hashNodos.insert(*numSrcIP,++countNodes);



    numTgtIP=new QString(inet_ntoa(ip->ip_dst));
    numTgtIP->append(ntohs(tcp->th_dport));


    if(!hashNodos.contains(*numTgtIP))
        hashNodos.insert(*numTgtIP,++countNodes);

    nodo1=QString::number(hashNodos.value(*numSrcIP));/*nodo1 sera el fuente*/
    nodo2=QString::number(hashNodos.value(*numTgtIP));/*nodo2 sera el destino*/

    if(!hashConexiones.contains(nodo1+nodo2)&&!hashConexiones.contains(nodo2+nodo1))
    {

        cout<<"insertara esta conexion con clave "<<(nodo1+nodo2).toStdString()<<endl;
        Conexion nuevaConexion(nodo1.toInt(),nodo2.toInt(),hashConexiones.size()+1,tablePacketThread);// una conexion compuesta de dos nodos
        hashConexiones.insert(nodo1+nodo2,nuevaConexion);

    }
    cout<<"conexion falsa"<<endl;
    Conexion conexionFalsa(-1,-1,-1, tablePacketThread);

    Conexion    conexionActual    = hashConexiones.value(nodo1+nodo2,conexionFalsa);
    key=nodo1+nodo2;

    if(conexionActual.getNumeroConexion()==-1)
    {
        conexionActual= hashConexiones.value(nodo2+nodo1,conexionFalsa);
        key= nodo2+nodo1;
    }
    if(conexionActual.getNumeroConexion()==-1)
        exit(0);


    //------------------------------LOQUEANDO JJ------------------------------------

    u_char flags;

    if ((flags = tcp->th_flags) &(TH_FIN|TH_SYN|TH_RST|TH_PUSH|TH_ACK|TH_URG|TH_ECE|TH_CWR)) {

        printf("CAPA 4 TCP-> sequencia: 0x%x ack: 0x%x flags:",ntohl(tcp->th_seq), ntohl(tcp->th_ack));
        printf("\n OK sequencia: %u ack: %u ",ntohl(tcp->th_seq), ntohl(tcp->th_ack));
        printf("\n OK next secuencia: %u",ntohl(tcp->th_seq) + (header->len - SIZE_ETHERNET - size_ip - size_tcp ));
        printf("\n Tam packet: %d", header->len);
        printf("\n Tam Header Ethernet: %d", SIZE_ETHERNET);
        printf("\n Tam Header ip: %d", size_ip);
        printf("\n Tam Header tcp: %d", size_tcp);
        printf("\n Tam DATA tcp: %u", (header->len - SIZE_ETHERNET - size_ip - size_tcp ));
        printf("\nVENTANA TAM = %u \n",tcp->th_win);
        printf("Esta conexion es la numero= %d\n",conexionActual.getNumeroConexion());
        cout<<"nodo fuente: "<<hashNodos.value(*numSrcIP)<<endl;
        cout<<"nodo destino: "<<hashNodos.value(*numTgtIP)<<endl;
        cout<<"ID: "<<ip->ip_id<<endl;
        printf("ID unsigned:%hu \n",ip->ip_id);

        cout<<endl;

        bool nodata=(header->len - SIZE_ETHERNET - size_ip - size_tcp )==0;

        if(nodata)
        {
            cout<<"NO DATA ";
        }

        if ((flags & TH_ACK) &&(flags & TH_SYN))
        {
            cout<<"SYN+ACK"<<endl;


        }
        else if ((flags & TH_SYN)and not((flags & TH_ACK)))
        {
            cout<<"SYN"<<endl;

        }


        else if ((flags & TH_PUSH)and ((flags & TH_ACK)) )
        {
            cout<<"PUSH+ACK"<<endl;

        }
        else if ((flags & TH_FIN)and not((flags & TH_ACK)) )
        {
            cout<<"FIN"<<endl;

        }
        else  if ((flags & TH_FIN)and ((flags & TH_ACK)) )
        {
            cout<<"FIN+ACK"<<endl;

        }
        else if ((flags & TH_ACK))
        {
            cout<<"ACK "<<endl;

        }
        else
            cout<<"EN IF NO HAY UNA BANDERA CONSIDERADA"<<endl;



    }

    //------------------------------fin LOQUEANDO JJ------------------------------------

    //------------ FLAGS TCP --------------//
    cout << "FLAGS TCP" << endl;
    int ack = 0;
    int syn = 0;
    int push = 0;
    int fin = 0;
    int rst = 0;
    int cwr = 0;
    int ece = 0;
    int urg = 0;

    if( tcp->th_flags & TH_ACK ) {
        ack = 1;
        cout << "ACK = " << ack << endl;
    }
    else
        cout << "ACK = " << ack << endl;

    if( tcp->th_flags & TH_SYN ) {
        syn = 1;
        cout << "SYN = " << syn << endl;
    }
    else
        cout << "SYN = " << syn << endl;

    if( tcp->th_flags & TH_PUSH ) {
        push = 1;
        cout << "PUSH = " << push << endl;
    }
    else
        cout << "PUSH = " << push << endl;

    if( tcp->th_flags & TH_FIN ) {
        fin = 1;
        cout << "FIN = " << fin << endl;
    }
    else
        cout << "FIN = " << fin << endl;

    if( tcp->th_flags & TH_RST ) {
        rst = 1;
        cout << "RST = " << rst << endl;
    }
    else
        cout << "RST = " << rst << endl;

    if( tcp->th_flags & TH_CWR ) {
        cwr = 1;
        cout << "CWR = " << cwr << endl;
    }
    else
        cout << "CWR = " << cwr << endl;

    if( tcp->th_flags & TH_ECE ) {
        ece = 1;
        cout << "ECE = " << ece << endl;
    }
    else
        cout << "ECE = " << ece << endl;

    if( tcp->th_flags & TH_URG ) {
        urg = 1;
        cout << "URG = " << urg << endl;
    }
    else
        cout << "URG = " << urg << endl;

    cout << endl;
    //------------------------------------//




    float diff = calculo_time(header);
    cout<<"tiempo de funcion "<<diff<<endl;

    //-----------------------------CODIGO DE CLASE---------------------------------------
    Packet paqueteEvaluado(header,ip,tcp,countPacket);
    conexionActual.evaluarNuevoPaquete(paqueteEvaluado,nodo1.toInt(),nodo2.toInt(),trace);
    hashConexiones.remove(key);
    hashConexiones.insert(key,conexionActual);

    if(countPacket>110)
    {
        conexionActual.imprimirListas();
        cout<<"PAQUETES PINTADOS DE CONEXION"<< conexionActual.getcontadorPaquetesPintados()<<endl;
        cout<<"num Paquetes conexion"<<conexionActual.getcontadorPaquetes()<<endl;
        cout<<"porcentaje de paquetes pintados"<<((float)conexionActual.getcontadorPaquetesPintados()/(float)conexionActual.getcontadorPaquetes()) *100<<endl;

    }

    return;
}

PcapThread::PcapThread()
{
    this->started = false;
}

void PcapThread::setInterfaceName(QString interfaceName)
{
    this->interfaceName = interfaceName;
}


QString PcapThread::getInterfaceName()
{
    return interfaceName;
}

QList<Interface> PcapThread::getInterfaces()
{
    QList<Interface> list;

    pcap_if_t * interfaces;
    pcap_findalldevs(&interfaces,errbuf);

    while( interfaces->next ){

        pcap_t * tmp = pcap_open_live(interfaces->name,SNAP_LEN, 1, 1000, errbuf);
        int datalink_type = pcap_datalink(tmp);
        pcap_close(tmp);

        Interface device;
        device.name = interfaces->name;
        device.datalink_type = datalink_type;

        list.append(device);

        interfaces = interfaces->next;
    }

    return list;
}

bool PcapThread::checkDevice()
{
    bpf_u_int32 mask;
    int ret;

    if ( (ret = pcap_lookupnet(interfaceName.toAscii(), &net, &mask, errbuf)) == -1) {
        //        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
        //                interfaceName.toAscii(), errbuf);
        cout << "Couldn't get netmask for device " << errbuf << endl;
        net = 0;
        mask = 0;

        return false;
    }

    return true;

}

bool PcapThread::openDevice()
{

    /* open capture device */
    handler = pcap_open_live(interfaceName.toAscii(), SNAP_LEN, 1, 1000, errbuf);
    if (handler == NULL) {
        //fprintf(stderr, "Couldn't open device %s: %s\n", interfaceName.toAscii(), errbuf);
        cout <<  "Couldn't open device %s:\n" <<  errbuf << endl;

        return false;
    }

    return true;
}

bool PcapThread::compileFilter()
{

    char filter_exp[] = " tcp and port 80 ";

    /* compile the filter expression */
    if (pcap_compile(handler, &fp, filter_exp, 0, net) == -1) {
//        fprintf(stderr, "Couldn't parse filter %s: %s\n",
//                filter_exp, pcap_geterr(handler));

        cout << "Couldn't parse filter %s: " << errbuf << endl;

        return false;
    }

    return true;

}

bool PcapThread::execFilter()
{
    /* apply the compiled filter */
    if (pcap_setfilter(handler, &fp) == -1) {
//        fprintf(stderr, "Couldn't install filter %s: %s\n",
//                filter_exp, pcap_geterr(handler));

        cout << "Couldn't install filter %s: " << errbuf << endl;
        return false;
    }

    return true;
}


bool PcapThread::initInterface()
{
    return ( checkDevice() && openDevice() && compileFilter() && execFilter() );
}

void PcapThread::run()
{
    /* now we can set our callback function */
    pcap_loop(handler, LOOP_FOREVER, got_packet, NULL);
}

std::string PcapThread::getPcapError()
{
    return std::string(errbuf);
}

void PcapThread::resetValues()
{
    countPacket = 0;
    countNodes  =-1;
    hashNodos.clear();
    hashConexiones.clear();
}

void PcapThread::setTablePacket(TablePacket *& tablePacket)
{
    tablePacketThread = tablePacket;
}

PcapThread::~PcapThread()
{

}

