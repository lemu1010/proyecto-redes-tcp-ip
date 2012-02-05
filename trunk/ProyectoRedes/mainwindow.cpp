#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
}

void MainWindow::capture_device()
{


    if( (ret = pcap_findalldevs(&lista,errbuf)) == -1  )
    {
        printf("ERROR: %s\n",errbuf);
        exit(-1);
    }

    aux = lista;

    while( aux != NULL )
    {
        printf("Nombre del Dispositivo: %s\n",aux->name);
        printf("Bandera: %d\n",aux->flags);
        printf("Descripcion: %s\n",aux->description);

        dev = aux->name;

        if( (ret = pcap_lookupnet(dev,&netp,&maskp,errbuf)) == -1 )
        {
            printf("ERROR: %s\n",errbuf);
        }

        printf("ret devolvio: %d\n",ret);
        addr.s_addr = netp;

        if( (net = inet_ntoa(addr)) == NULL )
        {
            perror("inet_ntoa");
            exit(-1);
        }

        printf("Direccion de Red: %s\n",net);

        addr.s_addr = maskp;
        mask = inet_ntoa(addr);

        if( (net = inet_ntoa(addr)) == NULL)
        {
            perror("inet_ntoa");
            exit(-1);
        }

        printf("Mascara de Red: %s\n\n\n",mask);

        aux = aux->next;
    }

    exit(-1);

}

MainWindow::~MainWindow()
{
    delete ui;
}

