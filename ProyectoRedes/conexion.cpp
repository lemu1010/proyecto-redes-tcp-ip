#include "conexion.h"


Conexion::Conexion()
{
    nodoCliente=-1;
    nodoServidor=-1;
    contadorPaquetes = 0;
    RTTEstimado=0.0;
}
Conexion::Conexion(int nodoCliente,int nodoServidor, int nuemeroConexion)
{
    this->contadorPaquetes = 0;
    this->nodoCliente=nodoCliente;
    this->nodoServidor=nodoServidor;
    this->numeroConexion=nuemeroConexion;
    this->RTTEstimado=0.0;


    cout<<"incializado"<<contadorPaquetes<<endl;

}

void Conexion::setNodoCliente(int nodoCliente)
{
    this->nodoCliente=nodoCliente;
}

void Conexion::setNodoServidor(int nodoServidor )
{
    this->nodoServidor=nodoServidor;
}

int  Conexion::getNodoCliente()
{
    return (nodoCliente);
}

int  Conexion::getNodoServidor()
{
    return(nodoServidor);
}

void Conexion:: setNumeroConexion(int numeroConexion)
{
    this->numeroConexion=numeroConexion;
}
int Conexion:: getNumeroConexion()
{
    return(this->numeroConexion);
}

void Conexion::evaluarNuevoPaquete( const struct pcap_pkthdr *header,const u_char * packet,int fuente, int destino,fstream &trace)
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

    /*Para archivo de traza*/



    string namePacket;
    string eventType = "x";
    string banderas = "-------";
    float timeEncolado = 0.020000;
    double timeStamp=-100;



    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    ltime = localtime(&local_tv_sec);
    strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);

    contadorPaquetes++;

    printf("\n------------------------------Packet number %d:------------------------------\n", contadorPaquetes);
    printf("Tiempo : %s.%.6d len:%d \n", timestr, header->ts.tv_usec, header->len);       /* print timestamp and length of the packet */





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

    timeStamp=calculo_time(header);
    Packet tempPacket;
    unsigned int tam=(header->len - SIZE_ETHERNET - size_ip - size_tcp );
    unsigned int nextSeq=ntohl(tcp->th_seq) + tam ;

    int i;
    double RTT=0.0;
    cout<<"TAM="<<tam<<endl;
    /*--------------------Validando paquetes ----------------------------------------*/

    if(tcp->th_flags & TH_SYN and not(tcp->th_flags & TH_ACK)) {
        cout<<"SYN"<<endl;
        if(fuente==nodoCliente)
        {
            namePacket="tcp";
            eventType = "+";
            trace << eventType << " " << timeStamp << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

            eventType = "-";
            trace << eventType << " " << timeStamp << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;
            tempPacket.packet=packet;
            tempPacket.tiempo=timeStamp;

            listaPaqCliente.append(tempPacket);
            listaConfirmCliente.append(nextSeq+1);


        }

    }

    else  if ((tcp->th_flags & TH_ACK) &&(tcp->th_flags & TH_SYN))
    {
        cout<<"SYN+ACK"<<endl;
        if(fuente==nodoServidor)
        {
            namePacket="tcp";
            eventType = "r";

            for(i=0;i<listaConfirmCliente.size();i++)
            {
                if(listaConfirmCliente[i]== ntohl(tcp->th_ack))
                    break;
            }
            RTT=(timeStamp-listaPaqCliente[i].tiempo);
            RTTEstimado  =	 0.875*RTT +	0.125	* RTT;
            cout<<"ESTIMADO: SYN"<<RTTEstimado<<endl;

            listaPaqCliente.removeAt(i);

            trace << eventType << " " << RTT/2 << " " <<nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << header->len << " ";/*OJO CORRGIR PINGO tamaño*/
            trace << banderas << " " <<  htons(tcp->th_dport) << " " <<htons(tcp->th_sport) << " ";
            trace << tcp->th_win << " " <<listaConfirmCliente[i]-1 << " " << ip->ip_id << endl;/*MOSCA CORREGIR ID*/
            listaConfirmCliente.removeAt(i);

            namePacket="ack";
            eventType = "+";
            trace << eventType << " " << RTT/2 << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

            eventType = "-";
            trace << eventType << " " << RTT/2 << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

            eventType = "r";
            trace << eventType << " " << timeStamp << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

            tempPacket.packet=packet;
            tempPacket.tiempo=timeStamp;

            listaPaqServidor.append(tempPacket);
            listaConfirmServidor.append(nextSeq+1);

        }



    }

    else if ((tcp->th_flags & TH_ACK) && (tam==0))
    {

        cout<<"ACK  PURO"<<endl;
        cout<<"ESTIMADO:"<<RTTEstimado<<endl;
        cout<<"fuente"<<fuente<<endl;
        cout<<"cliente"<<nodoCliente<<endl;
        if(fuente==nodoCliente)
        {

            for(i=0;i<listaConfirmServidor.size();i++)
            {
                if(listaConfirmServidor[i]== ntohl(tcp->th_ack))
                { listaConfirmServidor.removeAt(i);
                    break;}
            }
            listaPaqServidor.removeAt(i);

            namePacket="tcp";
            eventType = "+";
            trace << eventType << " " << timeStamp << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

            eventType = "-";
            trace << eventType << " " << timeStamp << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

            eventType = "r";
            trace << eventType << " " << timeStamp +RTTEstimado/2<< " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << header->len << " ";
            trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
            trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;
        }

    }

    /*
        int nodoCliente;
        int nodoServidor;
        unsigned int ultimoACK;
        unsigned int ultimoSeqEnviado;
        int numeroConexion;
        int contadorPaquetes;

      //  QString prueba;

         QList <Packet>listaPaqCliente;
         QList <Packet>listaPaqEmisor;

         QList <unsigned int> listaConfirmaciones;


    */
    /*   if(nodata)
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
    */
}

double Conexion::calculo_time(const pcap_pkthdr *header)
{
    static double tiempoBase=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;
    double tiempoNuevo=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;


    printf("tiempo con printf %.16g segundos\n",tiempoNuevo-tiempoBase);

    return tiempoNuevo-tiempoBase;
}


