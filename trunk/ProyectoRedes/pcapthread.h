#ifndef PCAPTHREAD_H
#define PCAPTHREAD_H

#include <QThread>
#include <struct.h>
#include <QHash>
#include <conexion.h>
#include <fstream>
#include <QDebug>
#include <QMutex>
#include <tablepacket.h>
#include <textpacket.h>

#define LOOP_FOREVER -1

using namespace std;

typedef struct interface {
    char *name;
    int datalink_type;
} Interface;

class PcapThread : public QThread
{
    Q_OBJECT

    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 net;
    struct bpf_program fp;
    QString interfaceName;


public:

    pcap_t * handler;
    bool started;

    explicit PcapThread();
    QString getInterfaceName();
    QList<Interface> getInterfaces();

    bool initInterface();
    std::string getPcapError();
    void resetValues();

    void setTablePacket(TablePacket *&);
    void setTextPacket(TextPacket *&);

    /* Thread Functions */
    void run();
    ~PcapThread();

private:
    bool checkDevice();
    bool openDevice();
    bool compileFilter();
    bool execFilter();

public slots:
    void setInterfaceName(QString interfaceName);
};

#endif // PCAPTHREAD_H
