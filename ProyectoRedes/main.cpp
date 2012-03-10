#include <QtGui/QApplication>
#include <QHash>
#include <QTimer>
#include <QDebug>
#include "mainwindow.h"
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <fstream>
#include <conexion.h>

using namespace std;

fstream trace("../ProyectoRedes/trazaReal.tr",ios::out);

float calculo_time(const struct pcap_pkthdr *header);

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void print_payload(const u_char *payload, int len);

void print_hex_ascii_line(const u_char *payload, int len, int offset);


/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void print_hex_ascii_line(const u_char *payload, int len, int offset)
{

    int i;
    int gap;
    const u_char *ch;

    /* offset */
    printf("%05d   ", offset);

    /* hex */
    ch = payload;
    for(i = 0; i < len; i++) {
        printf("%02x ", *ch);
        ch++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            printf(" ");
    }
    /* print space to handle line less than 8 bytes */
    if (len < 8)
        printf(" ");

    /* fill hex gap with spaces if not full line */
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            printf("   ");
        }
    }
    printf("   ");

    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch))
            printf("%c", *ch);
        else
            printf(".");
        ch++;
    }

    printf("\n");

    return;
}

/*
 * print packet payload data (avoid printing binary data)
 */
void print_payload(const u_char *payload, int len)
{

    int len_rem = len;
    int line_width = 16;			/* number of bytes per line */
    int line_len;
    int offset = 0;					/* zero-based offset counter */
    const u_char *ch = payload;

    if (len <= 0)
        return;

    /* data fits on one line */
    if (len <= line_width) {
        print_hex_ascii_line(ch, len, offset);
        return;
    }

    /* data spans multiple lines */
    for ( ;; ) {
        /* compute current line length */
        line_len = line_width % len_rem;
        /* print line */
        print_hex_ascii_line(ch, line_len, offset);
        /* compute total remaining */
        len_rem = len_rem - line_len;
        /* shift pointer to remaining bytes to print */
        ch = ch + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_rem <= line_width) {
            /* print last line and get out */
            print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }

    return;
}

/*
 * dissect/print packet
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

    static int countPacket = 1;    /* packet counter */
    static int countNodes  =-1;
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

    static QHash<QString,int> hashNodos;
    static QHash<QString,Conexion> hashConexiones;
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
        Conexion nuevaConexion(nodo1.toInt(),nodo2.toInt(),hashConexiones.size()+1);// una conexion compuesta de dos nodos
        hashConexiones.insert(nodo1+nodo2,nuevaConexion);

       }

    Conexion conexionFalsa(-1,-1,-1);

    Conexion    conexionActual    = hashConexiones.value(nodo1+nodo2,conexionFalsa);
    key=nodo1+nodo2;

     if(conexionActual.getNumeroConexion()==-1)
     {
          conexionActual= hashConexiones.value(nodo2+nodo1,conexionFalsa);
          key= nodo2+nodo1;
     }
     if(conexionActual.getNumeroConexion()==-1)
         exit(0);


    //------------------------------INFO PAQUETE-----------------------------------

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

calculo_time(header);
    //-----------------------------CODIGO DE CLASE---------------------------------------
   Packet paqueteEvaluado(header,ip,tcp);
   conexionActual.evaluarNuevoPaquete(paqueteEvaluado,nodo1.toInt(),nodo2.toInt(),trace);
   hashConexiones.remove(key);
   hashConexiones.insert(key,conexionActual);


   if(countPacket>215)
   {
       conexionActual.imprimirListas();
   }

    return;
}


float calculo_time(const struct pcap_pkthdr *header)
{

    static double tiempoBase=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;
    double tiempoNuevo=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;


    printf("tiempo con printf %.16g segundos\n",tiempoNuevo-tiempoBase);

   return tiempoNuevo-tiempoBase;
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.capture_device();
    w.show();

    //char *dev = "wlan0"; //NULL;			/* capture device name */
    //char *dev ="eth0";
    char *dev ="eth1"; //NULL;			/* capture device name */
   // char *dev ="eth2"; //NULL;			/* capture device name */
   //char *dev ="eth1"; //NULL;			/* capture device name */


    char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
    pcap_t *handle;				/* packet capture handle */

    char filter_exp[] = " tcp and port 80";		/* filter expression [3] */
    struct bpf_program fp;			/* compiled filter program (expression) */
    bpf_u_int32 mask;			/* subnet mask */
    bpf_u_int32 net;			/* ip */
    int num_packets = -1;			/* number of packets to capture */

    /* get network number and mask associated with capture device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        fprintf(stderr, "Couldn't get netmask for device %s: %s\n",
                dev, errbuf);
        net = 0;
        mask = 0;
    }

    /* print capture info */
    printf("Device: %s\n", dev);
    printf("Number of packets: %d\n", num_packets);
    printf("Filter expression: %s\n", filter_exp);

    /* open capture device */
    handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf);
    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        exit(EXIT_FAILURE);
    }

    /* compile the filter expression */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
        fprintf(stderr, "Couldn't parse filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* apply the compiled filter */
    if (pcap_setfilter(handle, &fp) == -1) {
        fprintf(stderr, "Couldn't install filter %s: %s\n",
                filter_exp, pcap_geterr(handle));
        exit(EXIT_FAILURE);
    }

    /* now we can set our callback function */
    pcap_loop(handle, num_packets, got_packet, NULL);

    /* cleanup */
    pcap_freecode(&fp);
    pcap_close(handle);

    printf("\nCapture complete.\n");
    
    return a.exec();
}
