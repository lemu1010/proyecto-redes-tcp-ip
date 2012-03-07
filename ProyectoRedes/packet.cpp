#include "packet.h"

Packet::Packet()
{
}

Packet::Packet(const pcap_pkthdr *header, const u_char *packet, int fuente, int destino)
{


    /* declare pointers to packet headers */
    const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
    const struct sniff_ip *ip;              /* The IP header */
    const struct sniff_tcp *tcp;            /* The TCP header */

    int size_ip;
    int size_tcp;


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


    /* define/compute tcp header offset */
    tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }

    setIPFuente(inet_ntoa(ip->ip_src));
    setPortFuente(ntohs(tcp->th_sport));

    setIPDestino(inet_ntoa(ip->ip_dst));
    setPortDestino(ntohs(tcp->th_dport));

    setId(ip->ip_id);

    setSeq(ntohl(tcp->th_seq));
    setCwnd(tcp->th_win);
    setTimeStamp(calculo_time(header));
    setSize(header->len );


    unsigned int tam=(header->len - SIZE_ETHERNET - size_ip - size_tcp );
    unsigned int nextSeq=ntohl(tcp->th_seq) + tam ;

    setSizeData(tam);
    setNextSeq(nextSeq);


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

double Packet::calculo_time(const pcap_pkthdr * header)
{

    static double tiempoBase=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;
    double tiempoNuevo=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;


    return tiempoNuevo-tiempoBase;
}
