#ifndef CONEXION_H
#define CONEXION_H
#include <QList>
#include <mainwindow.h>

class Conexion
{

    int nodo1;
    int nodo2;
    unsigned int nextSeq;
    unsigned int currentSeq;
    int numeroConexion;



public:
    Conexion();
    Conexion(int,int,int);
    void setnodo1(int );
    void setnodo2(int );
    void  setNumeroConexion(int);
    int  getnodo1();
    int  getnodo2();
    int  getNumeroConexion();


};

#endif // CONEXION_H
