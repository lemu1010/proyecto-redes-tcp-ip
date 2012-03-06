#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <struct.h>


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void capture_device();
    
private:
    Ui::MainWindow *ui;

    char *net;
    char *mask;
    char *dev;
    int ret;
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 netp;
    bpf_u_int32 maskp;
    struct in_addr addr;
    pcap_if_t* lista;
    pcap_if_t* aux;



};

#endif // MAINWINDOW_H
