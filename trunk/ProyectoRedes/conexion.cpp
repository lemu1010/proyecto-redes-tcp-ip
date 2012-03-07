#include "conexion.h"


Conexion::Conexion()
{
    nodoCliente=-1;
    nodoServidor=-1;
    contadorPaquetes = 0;
    RTTEstimado=0.0;
}
Conexion::Conexion(int nodoCliente,int nodoServidor, int nuemeroConexion)
{
    this->contadorPaquetes = 0;
    this->nodoCliente=nodoCliente;
    this->nodoServidor=nodoServidor;
    this->numeroConexion=nuemeroConexion;
    this->RTTEstimado=0.0;


    cout<<"incializado"<<contadorPaquetes<<endl;

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

void Conexion::evaluarNuevoPaquete( Packet packet,int fuente,int destino, fstream &trace)
{

    /*Para archivo de traza*/
    string namePacket;
    string eventType = "x";
    string banderas = "-------";
    float timeEncolado = 0.020000;


    contadorPaquetes++;

    printf("\n------------------------------Packet number CLASS %d:------------------------------\n", contadorPaquetes);


    int i;
    double RTT=0.0;
    cout<<"TAM="<<packet.getSizeData()<<endl;
    /*--------------------Validando paquetes ----------------------------------------*/

    if(packet.getSYN() and not(packet.getACK())) {
        cout<<"SYN"<<endl;
        if(fuente==nodoCliente)
        {
            namePacket="tcp";
            eventType = "+";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << endl;

            eventType = "-";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << endl;


            listaPaqCliente.append(packet);

        }

    }

    else  if ((packet.getACK()) &&(packet.getSYN()))
    {
        cout<<"SYN+ACK"<<endl;
        if(fuente==nodoServidor)
        {
            namePacket="tcp";
            eventType = "r";

            for(i=0;i<listaPaqCliente.size();i++)
            {
                if(listaPaqCliente[i].getNextSeq()== ntohl(packet.getNumAck()))
                    break;
            }
            RTT=(packet.getTimeStamp()-listaPaqCliente[i].getTimeStamp());
            RTTEstimado  =	 0.875*RTT +	0.125	* RTT;
            cout<<"ESTIMADO: SYN"<<RTTEstimado<<endl;



            trace << eventType << " " << RTT/2 << " " <<nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << listaPaqCliente[i].getSize() << " ";
            trace << banderas << " " <<  listaPaqCliente[i].getPortFuente()<< " " <<listaPaqCliente[i].getPortDestino() << " ";
            trace << listaPaqCliente[i].getCwnd() << " " <<listaPaqCliente[i].getSeq() << " " << listaPaqCliente[i].getId() << endl;

            listaPaqCliente.removeAt(i);

            namePacket="ack";
            eventType = "+";
            trace << eventType << " " << RTT/2 << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId()<< endl;

            eventType = "-";
            trace << eventType << " " << RTT/2 << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " <<  packet.getId()<< endl;

            eventType = "r";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId()<< endl;


            listaPaqServidor.append(packet);


        }



    }

    else if ((packet.getACK()) && (packet.getSizeData()==0))
    {

        cout<<"ACK  PURO"<<endl;
        cout<<"ESTIMADO:"<<RTTEstimado<<endl;
        cout<<"fuente"<<fuente<<endl;
        cout<<"cliente"<<nodoCliente<<endl;
        if(fuente==nodoCliente)
        {

            for(i=0;i<listaPaqServidor.size();i++)
            {
                if(listaPaqServidor[i].getNextSeq()== packet.getNumAck()) {
                    listaPaqServidor.removeAt(i);
                    break;
                }
            }


            namePacket="tcp";
            eventType = "+";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << endl;

            eventType = "-";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << endl;

            eventType = "r";
            trace << eventType << " " << packet.getTimeStamp() +RTTEstimado/2 << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << endl;
        }

    }

    /*
        int nodoCliente;
        int nodoServidor;
        unsigned int ultimoACK;
        unsigned int ultimoSeqEnviado;
        int numeroConexion;
        int contadorPaquetes;

      //  QString prueba;

         QList <Packet>listaPaqCliente;
         QList <Packet>listaPaqEmisor;

         QList <unsigned int> listaConfirmaciones;


    */
    /*   if(nodata)
    {
        cout<<"NO DATA ";
    }

   if ((flags & TH_ACK) &&(flags & TH_SYN))
   {
       cout<<"SYN+ACK"<<endl;


   }
   else if ((flags & TH_SYN)and not((flags & TH_ACK)))
   {
       cout<<"SYN"<<endl;

   }


   else if ((flags & TH_PUSH)and ((flags & TH_ACK)) )
   {
       cout<<"PUSH+ACK"<<endl;

   }
   else if ((flags & TH_FIN)and not((flags & TH_ACK)) )
   {
       cout<<"FIN"<<endl;

   }
  else  if ((flags & TH_FIN)and ((flags & TH_ACK)) )
   {
       cout<<"FIN+ACK"<<endl;

   }
   else if ((flags & TH_ACK))
   {
       cout<<"ACK "<<endl;

   }
   else
       cout<<"EN IF NO HAY UNA BANDERA CONSIDERADA"<<endl;
    */
}



