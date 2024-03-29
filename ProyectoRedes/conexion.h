#ifndef CONEXION_H
#define CONEXION_H
#include <QList>
#include <struct.h>
#include <iostream>
#include <fstream>
#include <packet.h>
#include <tablepacket.h>
#include <treepacket.h>

using namespace std;


class Conexion
{
    /* Nombres relativos pues estos nodos pueden cambiar roles */
    /* Entonces el cliente sera el que inicie la conversacion */
    int nodoCliente;
    int nodoServidor;
    unsigned int ultimoACKEnviadoCliente;
    unsigned int ultimoSeqEnviadoCliente;
    unsigned int ultimoACKRecibidoServidor;
    unsigned int ultimoSeqEnviadoServidor;

    int numeroConexion;
    int contadorPaquetes;
    int contadorPaquetesPintados;
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
    void setcontadorPaquetesPintados(int);
    void setcontadorPaquetes(int);

    int  getNodoCliente();
    int  getNodoServidor();
    int  getNumeroConexion();
    int  getcontadorPaquetesPintados( );
    int  getcontadorPaquetes();

    void evaluarNuevoPaquete(Packet,int ,int,ofstream &);
    void imprimirListas();
    double actualizarRttEstimado(double, double);



};

#endif // CONEXION_H
