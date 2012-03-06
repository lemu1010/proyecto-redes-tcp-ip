#include "conexion.h"
#include <iostream>
using namespace std;

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

void Conexion::evaluarNuevoPaquete( const struct pcap_pkthdr *header,const u_char * packet)
{



}

double Conexion::calculo_time(const pcap_pkthdr *header)
{
    static double tiempoBase=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;
    double tiempoNuevo=header->ts.tv_sec +((double) header->ts.tv_usec) / 1000000;


    printf("tiempo con printf %.16g segundos\n",tiempoNuevo-tiempoBase);

   return tiempoNuevo-tiempoBase;
}


