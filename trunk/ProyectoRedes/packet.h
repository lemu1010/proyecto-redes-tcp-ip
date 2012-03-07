#ifndef PACKET_H
#define PACKET_H
#include <QString>
#include<struct.h>

class Packet
{
    QString IPFuente;
    QString IPDestino;
    u_short portFuente;
    u_short portDestino;
    u_short id;
    u_int seq;
    u_short cwnd;
    double timeStamp;
    unsigned int size;
    unsigned int sizeData;
    unsigned int nextSeq;
    int fuente;
    int destino;


public:
    Packet();
    Packet( const struct pcap_pkthdr *header, const u_char *packet,int fuente,int destino);
    void    setIPFuente(char *);
    QString getIPFuente();

    void    setIPDestino(char *);
    QString getIPDestino();

    void    setPortFuente(u_short);
    u_short getPortFuente();

    void    setPortDestino(u_short);
    u_short getPortDestino();

    void    setId(u_short);
    u_short getId();

    void    setSeq(u_int);
    u_int getSeq();

    void setCwnd(u_short);
    u_short getCwnd();

    void    setTimeStamp(double);
    double getTimeStamp();

    void    setSize(unsigned int);
    unsigned int getSize();

    void setSizeData(unsigned int);
    unsigned int getSizeData();

    void setNextSeq(unsigned int);
    unsigned int getNextSeq();

    void setFuente(int);
    int getFuente();

    void setDestino(int);
    int getDestino();

    double calculo_time(const struct pcap_pkthdr *);
};

#endif // PACKET_H
