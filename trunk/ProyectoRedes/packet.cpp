#include "packet.h"
#include <iostream>

using namespace std;

Packet::Packet()
{
}

Packet::Packet(const struct pcap_pkthdr *header,const struct sniff_ip *ip,const struct sniff_tcp *tcp, int numberPacketCaptured)
{

    int size_ip;
    int size_tcp;
    FIN=SYN=RST= PUSH= ACK= URG= ECE= CWR=false;


    size_ip = IP_HL(ip)*4;

    if (size_ip < 20) {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }



    size_tcp = TH_OFF(tcp)*4;
    if (size_tcp < 20) {
        printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
        return;
    }

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

double Packet::calculo_time(const pcap_pkthdr * header)
{

    static double tiempoBase=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;
    double tiempoNuevo=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;


    return tiempoNuevo-tiempoBase;
}
