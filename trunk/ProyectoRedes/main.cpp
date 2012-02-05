#include <QtGui/QApplication>
#include <QHash>
#include <QTimer>
#include <QDebug>
#include "mainwindow.h"
#include <iostream>
#include <time.h>
#include <fstream>

using namespace std;

void
        got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

void
        print_payload(const u_char *payload, int len);

void
        print_hex_ascii_line(const u_char *payload, int len, int offset);

void
        print_app_banner(void);

void
        print_app_usage(void);

/*
 * app name/banner
 */
void
        print_app_banner(void)
{

    printf("%s - %s\n", APP_NAME, APP_DESC);
    printf("%s\n", APP_COPYRIGHT);
    printf("%s\n", APP_DISCLAIMER);
    printf("\n");

    return;
}

/*
 * print help text
 */
void
        print_app_usage(void)
{

    printf("Usage: %s [interface]\n", APP_NAME);
    printf("\n");
    printf("Options:\n");
    printf("    interface    Listen on <interface> for packets.\n");
    printf("\n");

    return;
}

/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void
        print_hex_ascii_line(const u_char *payload, int len, int offset)
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
    const char *payload;                    /* Packet payload */

    int size_ip;
    int size_tcp;
    int size_payload;


    struct tm *ltime;
    char timestr[16];
    time_t local_tv_sec;


    /* convert the timestamp to readable format */
    local_tv_sec = header->ts.tv_sec;
    ltime = localtime(&local_tv_sec);

    strftime( timestr, sizeof timestr, "%H:%M:%S", ltime);

    /* print timestamp and length of the packet */
    printf("Tiempo: %s.%.6d len:%d ", timestr, header->ts.tv_usec, header->len);


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

    /*
         *  OK, this packet is TCP.
         */

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
    cout<<"-----------------CHAMO1--------------"<<endl;
    u_char flags;

    if ((flags = tcp->th_flags) &(TH_FIN|TH_SYN|TH_RST|TH_PUSH|TH_ACK|TH_URG|TH_ECE|TH_CWR)) {
         cout<<"ENTRA "<<endl;
//        if (flags & TH_FIN) printf(" FIN");
//        if (flags & TH_SYN) printf(" SYN");
//        if (flags & TH_RST) printf(" RST");
//        if (flags & TH_PUSH) printf(" PUSH");
//        if (flags & TH_ACK) printf(" ACK");
//        if (flags & TH_URG) printf(" URG");
//        if (flags & TH_ECE) printf(" ECE");
//        if (flags & TH_CWR) printf(" CWR");
         printf("CAPA 4 TCP-> puerto fuente:%d puerto destino:%d sequencia: 0x%x ack: 0x%x flags:", htons(tcp->th_sport), htons(tcp->th_dport),
         ntohl(tcp->th_seq), ntohl(tcp->th_ack));
     cout<<endl;

        if ((flags & TH_ACK) &&(flags & TH_SYN))
        {
            cout<<"SYN+ACK"<<endl;
            cout<<"ID: "<<ip->ip_id<<endl;
            cout<<"VENTANA="<<tcp->th_win<<endl;
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
    cout<<"-----------------CHAMO2--------------"<<endl;
    //------------------------------fin LOQUEANDO JJ------------------------------------

    /* define/compute tcp payload (segment) offset */
    //payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);

    /* compute tcp payload (segment) size */
  //  size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);

    /*
         * Print payload data; it might be binary, so don't just
         * treat it as a string.
         */
//    if (size_payload > 0) {
//        printf("   Payload (%d bytes):\n", size_payload);
//        print_payload(payload, size_payload);
//    }

    /*********** DATA TRACE ************/
    fstream trace;
    trace.open("trazaReal.tr",ios::out);

    QString eventType = "x";



    return;
}





int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.capture_device();
    //w.show();


    char *dev ="wlan0"; //NULL;			/* capture device name */

    char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
    pcap_t *handle;				/* packet capture handle */

    char filter_exp[] = " tcp and port 80";		/* filter expression [3] */
    struct bpf_program fp;			/* compiled filter program (expression) */
    bpf_u_int32 mask;			/* subnet mask */
    bpf_u_int32 net;			/* ip */
    int num_packets = -1;			/* number of packets to capture */

    print_app_banner();

    /* check for capture device name on command-line */
//    if (argc == 2) {
//        dev = argv[1];
//    }
//    else if (argc > 2) {
//        fprintf(stderr, "error: unrecognized command-line options\n\n");
//        print_app_usage();
//        exit(EXIT_FAILURE);
//    }
//    else {
//        /* find a capture device if not specified on command-line */
//        dev = pcap_lookupdev(errbuf);
//        if (dev == NULL) {
//            fprintf(stderr, "Couldn't find default device: %s\n",
//                    errbuf);
//            exit(EXIT_FAILURE);
//        }
//    }

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

    /* make sure we're capturing on an Ethernet device [2]
            if (pcap_datalink(handle) != DLT_IEEE802_11) {
                    fprintf(stderr, "%s is not an Ethernet\n", dev);
                    exit(EXIT_FAILURE);
            }*/

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
