#include <QtGui/QApplication>
#include <QHash>
#include <QTimer>
#include <QDebug>
#include "mainwindow.h"
#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <fstream>

using namespace std;

fstream trace("../ProyectoRedes/trazaReal.tr",ios::out);

float calculo_time(QString hourBase, suseconds_t usecondsBase, QString hourNew, suseconds_t usecondsNew);

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
    static int countNodes  =0;
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

    /* print timestamp and length of the packet */
    cout<<"\n-------------------------------"<<endl;
    printf("Tiempo : %s.%.6d len:%d \n", timestr, header->ts.tv_usec, header->len);

    static QHash<QString,int> hashNodos;
    QString *numSrcIP,*numTgtIP;

    printf("\nPacket number %d:\n", countPacket);
    countPacket++;

    /* define ethernet header */
    ethernet = (struct sniff_ethernet*)(packet);

    /* define/compute ip header offset */
    ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;

    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    /* print source and destination IP addresses */
    printf("       From: %s\n", inet_ntoa(ip->ip_src));
    printf("         To: %s\n", inet_ntoa(ip->ip_dst));

    numSrcIP=new QString(inet_ntoa(ip->ip_src));

    if(!hashNodos.contains(*numSrcIP))
        hashNodos.insert(*numSrcIP,++countNodes);


    numTgtIP=new QString(inet_ntoa(ip->ip_dst));

    if(!hashNodos.contains(*numTgtIP))
        hashNodos.insert(*numTgtIP,++countNodes);

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

    printf("   Src port: %d\n", ntohs(tcp->th_sport));
    printf("   Dst port: %d\n", ntohs(tcp->th_dport));


    //------------------------------LOQUEANDO JJ------------------------------------

    u_char flags;

    if ((flags = tcp->th_flags) &(TH_FIN|TH_SYN|TH_RST|TH_PUSH|TH_ACK|TH_URG|TH_ECE|TH_CWR)) {
        printf("CAPA 4 TCP-> sequencia: 0x%x ack: 0x%x flags:",ntohl(tcp->th_seq), ntohl(tcp->th_ack));
         printf("\n OK sequencia: %d ack: %d flags:",ntohl(tcp->th_seq), ntohl(tcp->th_ack));
         printf("\n OK sequencia: %u ack: %u flags:",ntohl(tcp->th_seq), ntohl(tcp->th_ack));
         printf("\n Tam packet: %d", header->len);
         printf("\n Tam Header Ethernet: %d", SIZE_ETHERNET);
         printf("\n Tam Header ip: %d", size_ip);
         printf("\n Tam Header tcp: %d", size_tcp);
         printf("\n OK next secuencia: %u",ntohl(tcp->th_seq) + (header->len - SIZE_ETHERNET - size_ip - size_tcp + 1));

         cout<<endl;

        if ((flags & TH_ACK) &&(flags & TH_SYN))
        {
            cout<<"SYN+ACK"<<endl;
            cout<<"ID: "<<ip->ip_id<<endl;
            cout<<"VENTANA="<<tcp->th_win<<endl;
            printf("VENTANA TAM1 = %u \n",tcp->th_win);
            printf("VENTANA TAM2 = %d \n",tcp->th_win);
            cout<<"nodo fuente: "<<hashNodos.value(*numSrcIP)<<endl;
            cout<<"numero seq="<<tcp->th_seq<<endl;
            cout<<"bit ack="<<TH_FLAGS<<endl;
            cout<<"numero ack="<<tcp->th_ack<<endl;

            //------------------------------------
            cout<<"nodo destino: "<<hashNodos.value(*numTgtIP)<<endl;
        }
        if ((flags & TH_SYN)and not((flags & TH_ACK)))
        {
            cout<<"SYN"<<endl;
            cout<<"ID: "<<ip->ip_id<<endl;
            cout<<"VENTANA="<<tcp->th_win<<endl;
            cout<<"nodo fuente: "<<hashNodos.value(*numSrcIP)<<endl;
            cout<<"numero seq="<<tcp->th_seq<<endl;
            cout<<"bit ack="<<TH_FLAGS<<endl;
            cout<<"numero ack="<<tcp->th_ack<<endl;

            //------------------------------------
            cout<<"nodo destino: "<<hashNodos.value(*numTgtIP)<<endl;
        }
        if ((flags & TH_ACK)and not((flags & TH_SYN)) )
        {
            cout<<"ACK"<<endl;
            cout<<"ID: "<<ip->ip_id<<endl;
            cout<<"VENTANA="<<tcp->th_win<<endl;
            cout<<"nodo fuente: "<<hashNodos.value(*numSrcIP)<<endl;
            cout<<"numero seq="<<tcp->th_seq<<endl;
            cout<<"bit ack="<<TH_FLAGS<<endl;
            cout<<"numero ack="<<tcp->th_ack<<endl;

            //------------------------------------
            cout<<"nodo destino: "<<hashNodos.value(*numTgtIP)<<endl;
        }

        cout<<endl;

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


    //----- NAME PACKET -----//

    string namePacket;

    if( tcp->th_flags & TH_ACK  )
        namePacket = "ack";
    else
        namePacket = "tcp";


    /*********** DATA TRACE ************/

    string eventType = "x";
    string banderas = "-------";

    float timeEncolado = 0.020000;

    /*static time_t timeSecBase = ltime->tm_sec;*/
    static suseconds_t timeUSecBase = header->ts.tv_usec;
    static QString timeHourBase(timestr);

    suseconds_t timeUSecNew = header->ts.tv_usec;
    QString  timeHourNew(timestr);
    float diff = calculo_time(timeHourBase,timeUSecBase,timeHourNew,timeUSecNew);
    cout<<"diferencia "<<diff<<endl;

    cout<<"-------------------------------"<<endl;

    if( (syn == 1) && (ack == 0) ) {
        eventType = "+";
        trace << eventType << " " << diff << " " << hashNodos.value(*numSrcIP) << " ";
        trace << hashNodos.value(*numTgtIP) << " " << namePacket << " " << header->len << " ";
        trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
        trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

        eventType = "-";
        trace << eventType << " " << diff << " " << hashNodos.value(*numSrcIP) << " ";
        trace << hashNodos.value(*numTgtIP) << " " << namePacket << " " << header->len << " ";
        trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
        trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;
    }
    else if( (syn == 1) && (ack == 1) ) {
        eventType = "+";
        trace << eventType << " " << diff << " " << hashNodos.value(*numSrcIP) << " ";
        trace << hashNodos.value(*numTgtIP) << " " << namePacket << " " << header->len << " ";
        trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
        trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;

        eventType = "-";
        trace << eventType << " " << diff << " " << hashNodos.value(*numSrcIP) << " ";
        trace << hashNodos.value(*numTgtIP) << " " << namePacket << " " << header->len << " ";
        trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
        trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;
    }
    else{
        eventType = "x";
        trace << eventType << " " << diff << " " << hashNodos.value(*numSrcIP) << " ";
        trace << hashNodos.value(*numTgtIP) << " " << namePacket << " " << header->len << " ";
        trace << banderas << " " << htons(tcp->th_sport) << " " << htons(tcp->th_dport) << " ";
        trace << tcp->th_win << " " << ntohl(tcp->th_seq) << " " << ip->ip_id << endl;
    }

    return;
}

float calculo_time(QString hourBase, suseconds_t usecondsBase, QString hourNew, suseconds_t usecondsNew)
{

    QString tbase,tnuevo;
    int secondsBasef,secondsNewf;


    secondsBasef=hourBase.section(":",0,0).toInt()*3600+hourBase.section(":",1,1).toInt()*60+hourBase.section(":",2,2).toInt();
    secondsNewf=hourNew.section(":",0,0).toInt()*3600+hourNew.section(":",1,1).toInt()*60+hourNew.section(":",2,2).toInt();
//    cout<<"HORA BASE"<<hourBase.section(":",1,1).toStdString()<<endl;
//    cout<<"MIN BASE"<<hourBase.section(":",2,2).toStdString()<<endl;
//    cout<<"SEC BASE"<<hourBase.section(":",3,3).toStdString()<<endl;

    tbase = QString::number(secondsBasef).append(".").append(QString::number(usecondsBase));
    tnuevo=  QString::number(secondsNewf).append(".").append(QString::number(usecondsNew));
    cout<<"base "<<tbase.toStdString()<<endl;
    cout<<"nuevo "<<tnuevo.toStdString()<<endl;
   /* cout<<"base en float"<<tnuevo.toDouble()<<endl;
    cout<<"nuevo en float "<<tbase.toDouble()<<endl;*/
    cout<<"diferencia en float antes "<<tnuevo.toDouble()-tbase.toDouble()<<endl;

    return tnuevo.toDouble()-tbase.toDouble() < 0 ? tnuevo.toDouble()-tbase.toDouble()+1 :tnuevo.toDouble()-tbase.toDouble() ;

}



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    //w.capture_device();
    w.show();

    char *dev ="wlan0"; //NULL;			/* capture device name */
    //char *dev ="eth1"; //NULL;			/* capture device name */
    //char *dev ="eth1";

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
