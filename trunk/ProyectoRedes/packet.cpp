#include "packet.h"
#include <iostream>

double tiempoB;

Packet::Packet()
{
}

Packet::Packet(const struct pcap_pkthdr *header,const struct sniff_ip *ip,const struct sniff_tcp *tcp, int numberPacketCaptured, const u_char *payload)
{

    int size_ip;
    int size_tcp;


    FIN=SYN=RST= PUSH= ACK= URG= ECE= CWR=false;
    setAcusado(-100);
    setAckRepetido(false);
    setRetransmision(false);


    size_ip = IP_HL(ip)*4;

    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }
    setSizeIP(size_ip);

    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }
    setSizeTCP(size_tcp);

    setNumberPacketCaptured(numberPacketCaptured);
    setIPFuente(inet_ntoa(ip->ip_src));
    setPortFuente(ntohs(tcp->th_sport));

    setIPDestino(inet_ntoa(ip->ip_dst));
    setPortDestino(ntohs(tcp->th_dport));

    setId(ip->ip_id);

    setSeq(ntohl(tcp->th_seq));
    setNumAck(ntohl(tcp->th_ack));
    setCwnd(tcp->th_win);
    setTimeStamp(calculo_time(header));
    setSize(header->len );


    unsigned int tam=(header->len - SIZE_ETHERNET - size_ip - size_tcp );
    unsigned int nextSeq=ntohl(tcp->th_seq) + tam ;

    setSizeData(tam);
    setNextSeq(nextSeq);

    setFlag(tcp->th_flags);

    if(SYN or (SYN && ACK))
        setNextSeq(nextSeq+1);

    else if(ACK && tam==0)
    {
        setNextSeq(-100);
    }
    else if(tam>0 && ACK)
    {
        setNextSeq(nextSeq);
    }

    if( FIN && ACK )
        setNextSeq(nextSeq + 1);

    cout<<"paquete creado lo  reconocera ack"<<this->nextSeq<<endl;
    printf("ack llego %u",ntohl(tcp->th_ack));
    printf("ack guardo %u",getNumAck());


    /*PAYLOAD*/


    /* define/compute tcp payload (segment) offset */
    this->payload = payload;

    /* compute tcp payload (segment) size */
    this->size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
    setSizePayload(size_payload);

    /*
               * Print payload data; it might be binary, so don't just
               * treat it as a string.
               */
//    if (size_payload > 0) {
//        printf("   Payload (%d bytes):\n", size_payload);
//        print_payload();
//    }
    if( this->size_payload > 0 ) {
        printf("   Payload (%d bytes):\n", size_payload);
        setPayload(print_payload(payload, this->size_payload));
    }
}

void Packet::setNumberPacketCaptured(int numberPackeCaptured)
{
    this->numberPacketCaptured = numberPackeCaptured;
}

int Packet::getNumberPacketCaptured()
{
    return numberPacketCaptured;
}

void Packet::setIPFuente(char * IPFuente)
{

    this->IPFuente= IPFuente;
}

QString Packet::getIPFuente()
{
    return(IPFuente);
}

void Packet::setIPDestino(char * IPDestino)
{
    this->IPDestino=IPDestino;
}

QString Packet::getIPDestino()
{
    return(IPDestino);
}

void Packet::setPortFuente(u_short portFuente)
{
    this->portFuente=portFuente;
}

u_short Packet::getPortFuente()
{
    return(portFuente);
}

void Packet::setPortDestino(u_short portDestino)
{
    this->portDestino=portDestino;
}

u_short Packet::getPortDestino()
{
    return(portDestino);
}

void Packet::setId(u_short id)
{
    this->id=id;
}

u_short Packet::getId()
{
    return(id);
}

void Packet::setSeq(u_int seq)
{
    this->seq=seq;
}

u_int Packet::getSeq()
{
    return(seq);
}

void Packet::setNumAck(u_int numAck)
{
    this->numAck=numAck;
}

u_int Packet::getNumAck()
{
    return(numAck);
}

void Packet::setCwnd(u_short cwnd)
{
    this->cwnd;
}

u_short Packet::getCwnd()
{
    return(cwnd);
}

void Packet::setTimeStamp(double timeStamp)
{
    this->timeStamp=timeStamp;
}

double Packet::getTimeStamp()
{
    return(timeStamp);
}

void Packet::setSize(unsigned int size)
{
    this->size=size;
}

unsigned int Packet::getSize()
{
    return(size);
}

void Packet::setSizeData(unsigned int sizeData)
{
    this->sizeData=sizeData;
}

unsigned int Packet::getSizeData()
{
    return(sizeData);
}

void Packet::setSizeIP(int sizeIP)
{
    this->sizeIP = sizeIP;
}

int Packet::getSizeIP()
{
    return(sizeIP);
}

void Packet::setSizeTCP(int sizeTCP)
{
    this->sizeTCP = sizeTCP;
}

int Packet::getSizeTCP()
{
    return(sizeTCP);
}

void Packet::setSizePayload(int size_payload)
{
    this->size_payload = size_payload;
}

int Packet::getSizePayload()
{
    return(size_payload);
}

void Packet::setNextSeq(unsigned int nextSeq)
{
    this->nextSeq=nextSeq;
}


unsigned int Packet::getNextSeq()
{
    return(nextSeq);
}

void Packet::setFuente(int fuente)
{
    this->fuente=fuente;
}

int Packet::getFuente()
{
    return(fuente);
}

void Packet::setDestino(int destino)
{
    this->destino=destino;
}

int Packet::getDestino()
{
    return(destino);
}

void Packet::setAcusado(int numberPacketAcusado)
{
    this->numberPacketAcusado=numberPacketAcusado;
}

int Packet::getAcusado()
{
    return this->numberPacketAcusado;
}

void Packet::setAckRepetido(bool ackRepetido)
{
    this->ackRepetido=ackRepetido;
}


bool Packet::getAckRepetido()
{
    return ackRepetido;
}

void Packet::setRetransmision(bool  retransmision)
{
    this->retransmision=retransmision;
}

bool Packet::getRestransmision()
{
    return this->retransmision;
}

void Packet::setFlag(u_char flags)
{

    /*FLAGS*/

    if( flags & TH_ACK )
        ACK=true;



    if( flags & TH_SYN )
        SYN=true;


    if(flags & TH_PUSH )
        PUSH=true;



    if( flags & TH_FIN )
        FIN =true;


    if( flags & TH_RST )
        RST = true;

    if( flags & TH_CWR )
        CWR =true;

    if( flags & TH_ECE )
        ECE =true;

    if( flags & TH_URG )
        URG=true;
}

bool Packet::getFIN()
{
    return(FIN);
}

bool Packet::getSYN()
{
    return(SYN);
}

bool Packet::getRST()
{
    return(RST);
}

bool Packet::getPUSH()
{
    return(PUSH);
}

bool Packet::getACK()
{
    return(ACK);
}

bool Packet::getURG()
{
    return(URG);
}

bool Packet::getECE()
{
    return(ECE);
}

bool Packet::getCWR()
{
    return(CWR);
}

void Packet::setPayload(QList<string> listaPayload)
{
    this->listaPayload = listaPayload;
}

QList<string> Packet::getPayload()
{
    return(listaPayload);
}

double Packet::calculo_time(const pcap_pkthdr * header)
{

    if( numberPacketCaptured == 1 )
        tiempoB = header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;

    double tiempoNuevo=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;

    return tiempoNuevo-tiempoB;
}

QList<string> Packet::print_payload(const u_char *payload, int len)
{
    int len_rem = len;
    int line_width = 16;			/* number of bytes per line */
    int line_len;
    int offset = 0;					/* zero-based offset counter */
    const u_char *ch = payload;
    QList<string> listaLineas;

    if (len_rem <= 0)
        return listaLineas;

    /* data fits on one line */
    if (len_rem <= line_width) {
        listaLineas.append( print_hex_ascii_line(ch, len_rem, offset));
        return listaLineas;
    }

    /* data spans multiple lines */
    for ( ;; ) {

        /* compute current line length */
        line_len = line_width % len_rem;
        /* print line */
        listaLineas.append(print_hex_ascii_line(ch, line_len, offset));
        /* compute total remaining */
        len_rem = len_rem - line_len;
        /* shift pointer to remaining bytes to print */
        ch = ch + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_rem <= line_width) {
            /* print last line and get out */
            listaLineas.append(print_hex_ascii_line(ch, len_rem, offset));
            break;
        }
    }

    return listaLineas;
}

string Packet::print_hex_ascii_line(const u_char *payload, int len, int offset)
{

    int i;
    int gap;
    const u_char *ch;
    string linea;
    char str[20];

    /* offset */
    sprintf(str, "%05d\t", offset);
    linea.append(str);

    /* hex */
    ch = payload;
    for(i = 0; i < len; i++) {
        sprintf(str,"%02x  ", *ch);
        linea.append(str);
        ch++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            linea.append("\t");
    }

    /* print space to handle line less than 8 bytes */
    if (len < 8)
        linea.append("\t");

    /* fill hex gap with spaces if not full line */
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            linea.append("   ");
        }
    }

    linea.append("\t");

    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch)) {
            sprintf(str,"%c", *ch);
            linea.append(str);
        }else
            linea.append(".");
        ch++;
    }

    linea.append("\n");

    return linea;
}







