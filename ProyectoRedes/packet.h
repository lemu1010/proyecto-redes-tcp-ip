#ifndef PACKET_H
#define PACKET_H

#include <QString>
#include <struct.h>
#include <QList>

using namespace std;

class Packet
{

    int numberPacketCaptured;
    int numberPacketAcusado;
    QString IPFuente;
    QString IPDestino;
    u_short portFuente;
    u_short portDestino;
    u_short id;
    u_int seq;
    u_int numAck;
    u_short cwnd;
    double timeStamp;
    unsigned int size;
    unsigned int sizeData;
    unsigned int nextSeq;
    int fuente;
    int destino;
    bool FIN;
    bool SYN;
    bool RST;
    bool PUSH;
    bool ACK;
    bool URG;
    bool ECE;
    bool CWR;
    bool ackRepetido;
    bool retransmision;
    const u_char *payload;
    int size_payload;
    int sizeIP;
    int sizeTCP;
    QList<string> listaPayload;





public:
    Packet();
    Packet( const struct pcap_pkthdr *header,const struct sniff_ip *ip,const struct sniff_tcp *tcp, int numberPacketCaptured, const u_char *payload
);

    void    setNumberPacketCaptured(int );
    int     getNumberPacketCaptured();

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

    void  setNumAck(u_int numAck);
    u_int getNumAck();

    void setCwnd(u_short);
    u_short getCwnd();

    void    setTimeStamp(double);
    double getTimeStamp();

    void    setSize(unsigned int);
    unsigned int getSize();

    void setSizeData(unsigned int);
    unsigned int getSizeData();

    void setSizeIP(int );
    int getSizeIP();

    void setSizeTCP(int );
    int getSizeTCP();

    void setSizePayload(int );
    int getSizePayload();

    void setNextSeq(unsigned int);
    unsigned int getNextSeq();

    void setFuente(int);
    int getFuente();

    void setDestino(int);
    int  getDestino();

    void setAcusado( int);
    int getAcusado();

    void setAckRepetido(bool);
    bool getAckRepetido();

    void setRetransmision(bool);
    bool getRestransmision();

    void setFlag(u_char flags);
    bool getFIN();
    bool getSYN();
    bool getRST();
    bool getPUSH();
    bool getACK();
    bool getURG();
    bool getECE();
    bool getCWR();

    void setPayload(QList<string> );
    QList<string> getPayload();

    double calculo_time(const struct pcap_pkthdr *);


    QList<string> print_payload(const u_char *payload, int len);
    string print_hex_ascii_line(const u_char *payload, int len, int offset);


};

#endif // PACKET_H
