#include "conexion.h"
/*

    int nodoCliente;
    int nodoServidor;
    unsigned int ultimoACK;
    unsigned int ultimoSeqEnviado;
    int numeroConexion;



public:
    Conexion();
    Conexion(int,int,int);
    void setnodoCliente(int );
    void setnodoServidor(int );
    void setNumeroConexion(int);
    int  getnodoCliente();
    int  getnodoServidor();
    int  getNumeroConexion();

*/
Conexion::Conexion()
{
    nodoCliente=-1;
    nodoServidor=-1;
}
Conexion::Conexion(int nodoCliente,int nodoServidor, int nuemeroConexion)
{
    this->nodoCliente=nodoCliente;
    this->nodoServidor=nodoServidor;
    this->numeroConexion=nuemeroConexion;

}

void Conexion::setNodoCliente(int nodoCliente)
{
    this->nodoCliente=nodoCliente;
}

void Conexion::setNodoServidor(int nodoServidor )
{
    this->nodoServidor=nodoServidor;
}

int  Conexion::getNodoCliente()
{
    return (nodoCliente);
}

int  Conexion::getNodoServidor()
{
    return(nodoServidor);
}

void Conexion:: setNumeroConexion(int numeroConexion)
{
    this->numeroConexion=numeroConexion;
}
int Conexion:: getNumeroConexion()
{
    return(this->numeroConexion);
}

bool Conexion::evaluarNuevoPaquete(const u_char *)
{

}


