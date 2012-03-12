#ifndef CONEXION_H
#define CONEXION_H
#include <QList>
#include <struct.h>
#include <iostream>
#include <fstream>
#include <packet.h>
#include <tablepacket.h>

using namespace std;


class Conexion
{
    /* Nombres relativos pues estos nodos pueden cambiar roles */
    /* Entonces el cliente sera el que inicie la conversacion */
    int nodoCliente;
    int nodoServidor;
    unsigned int ultimoACKRecibidoCliente;
    unsigned int ultimoSeqEnviadoCliente;
    unsigned int ultimoACKRecibidoServidor;
    unsigned int ultimoSeqEnviadoServidor;

    int numeroConexion;
    int contadorPaquetes;
    double RTTEstimado;

    //  QString prueba;

    /*lista que contiene los paquetes aun no confirmados de cada nodo*/
    QList <Packet>listaPaqCliente;
    QList <Packet>listaPaqServidor;

    TablePacket *tablePacket;

public:
    Conexion();
    Conexion(int,int,int, TablePacket *&);
    void setNodoCliente(int );
    void setNodoServidor(int );
    void setNumeroConexion(int);
    int  getNodoCliente();
    int  getNodoServidor();
    int  getNumeroConexion();
    void evaluarNuevoPaquete(Packet,int ,int,fstream &);
    void imprimirListas();
    double actualizarRttEstimado(double, double);



};

#endif // CONEXION_H
