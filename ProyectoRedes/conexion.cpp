#include "conexion.h"

Conexion::Conexion()
{
    nodo1=-1;
    nodo2=-1;
}
Conexion::Conexion(int nodo1,int nodo2, int nuemeroConexion)
{
    this->nodo1=nodo1;
    this->nodo2=nodo2;
    this->numeroConexion=nuemeroConexion;

}

void Conexion::setnodo1(int nodo1)
{
    this->nodo1=nodo1;
}

void Conexion::setnodo2(int nodo2 )
{
    this->nodo2=nodo2;
}

int  Conexion::getnodo1()
{
    return (nodo1);
}

int  Conexion::getnodo2()
{
    return(nodo2);
}

void Conexion:: setNumeroConexion(int numeroConexion)
{
    this->numeroConexion=numeroConexion;
}
int Conexion:: getNumeroConexion()
{
    return(this->numeroConexion);
}
