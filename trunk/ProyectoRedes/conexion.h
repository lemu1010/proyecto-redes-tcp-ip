#ifndef CONEXION_H
#define CONEXION_H
#include <QList>
#include <mainwindow.h>

class Conexion
{
   /* Nombres relativos pues estos nodos pueden cambiar roles */
   /* Entonces el cliente sera el que inicie la conversacion */
    int nodoCliente;
    int nodoServidor;
    unsigned int ultimoACK;
    unsigned int ultimoSeqEnviado;
    int numeroConexion;

     /*Estructuras que guardaran paquetes que fueron emitidos por un nodo hasta recibir confirmacion*/
     /*tuplas (ID_packet,packet)*/
     QHash<u_short,const u_char *> hashPacketCliente;
     QHash<u_short,const u_char *> hashPacketServidor;

public:
    Conexion();
    Conexion(int,int,int);
    void setNodoCliente(int );
    void setNodoServidor(int );
    void setNumeroConexion(int);
    int  getNodoCliente();
    int  getNodoServidor();
    int  getNumeroConexion();


};

#endif // CONEXION_H
