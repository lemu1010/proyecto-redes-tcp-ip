#ifndef CONEXION_H
#define CONEXION_H
#include <QList>
#include <mainwindow.h>
#include <struct.h>
#include <iostream>
#include <fstream>
using namespace std;

struct Packet
{
     double tiempo;
     const u_char *packet;
};

class Conexion
{
   /* Nombres relativos pues estos nodos pueden cambiar roles */
   /* Entonces el cliente sera el que inicie la conversacion */
    int nodoCliente;
    int nodoServidor;
    unsigned int ultimoACK;
    unsigned int ultimoSeqEnviado;
    int numeroConexion;
    int contadorPaquetes;
    double RTTEstimado;

  //  QString prueba;

     /*lista que contiene los paquetes aun no confirmados de cada nodo*/
     QList <Packet>listaPaqCliente;
     QList <Packet>listaPaqServidor;

     /*lista con numeros de confirmacion*/
     QList <unsigned int> listaConfirmCliente;
     QList <unsigned int> listaConfirmServidor;


public:
    Conexion();
    Conexion(int,int,int);
    void setNodoCliente(int );
    void setNodoServidor(int );
    void setNumeroConexion(int);
    int  getNodoCliente();
    int  getNodoServidor();
    int  getNumeroConexion();
    void evaluarNuevoPaquete(const struct pcap_pkthdr *,const u_char *,int ,int,fstream &);
    double calculo_time(const struct pcap_pkthdr *);


};

#endif // CONEXION_H
