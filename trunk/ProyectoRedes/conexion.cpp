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
    this->ultimoSeqEnviadoCliente=this->ultimoSeqEnviadoServidor=0;


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
    float timeEncolado = 0.004;


    contadorPaquetes++;

    printf("\n------------------------------Packet number CLASS %d:------------------------------\n", contadorPaquetes);


    int i;
    double RTT=0.0;
    cout<<"TAM="<<packet.getSizeData()<<endl;
    cout<<"ultimo server"<<ultimoSeqEnviadoServidor<<endl;
    cout<<"ultimo cliente"<<ultimoSeqEnviadoCliente<<endl;
    cout<<"num SEQ"<<packet.getSeq()<<endl;
    /*--------------------Validando paquetes ----------------------------------------*/
    string typeFlag;

    //NOTA FALTA MANEJAR REFLEJO DE LOS DOS PRIMEROS CASOS
    if( (packet.getSYN() and not(packet.getACK() ) )  or (packet.getSizeData() > 0&& packet.getACK() ) ) {
        cout<<"SYN"<<endl;
        if(fuente==nodoCliente)
        {
            if(packet.getSeq()<ultimoSeqEnviadoCliente) //RETRANSMISION
            {
                cout<<"RESTRANSMISION primer if"<<endl;
                for(i=0;i<listaPaqCliente.size();i++)
                {
                    if(listaPaqCliente[i].getSeq()==packet.getSeq())
                    {
                        listaPaqCliente.removeAt(i);
                        break;
                    }

                }
                if(i==listaPaqCliente.size())
                {
                    cout<<"no se consigue retransmision"<<endl;
                    exit(0);
                }

            }
            namePacket="tcp";
            eventType = "+";
            typeFlag = packet.getSYN() ? "SYN" : "DATA";

            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " "<< typeFlag << endl;

            eventType = "-";
            double timeAux=packet.getSizeData() > 0? packet.getTimeStamp()+timeEncolado:packet.getTimeStamp();
            trace << eventType << " " << timeAux << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << typeFlag << endl;

            ultimoSeqEnviadoCliente=packet.getSeq();
            listaPaqCliente.append(packet);

        }

    }

    else if (  (packet.getACK() ) && (packet.getSYN() )  )
    {
        cout<<"SYN+ACK"<<endl;
        if(fuente==nodoServidor)
        {

            if(packet.getSeq()<ultimoSeqEnviadoServidor) //RETRANSMISION
            {
                cout<<"RESTRANSMISION primer if"<<endl;
                for(i=0;i<listaPaqServidor.size();i++)
                {
                    if(listaPaqServidor[i].getSeq()==packet.getSeq())
                    {
                        listaPaqServidor.removeAt(i);
                        break;
                    }

                }
                if(i==listaPaqServidor.size())
                {
                    cout<<"no se consigue retransmision 2"<<endl;
                    exit(0);
                }

            }
            cout<<"antes tam de lista"<<listaPaqCliente.size()<<endl;
            if(listaPaqCliente.size()>0)
            {
                for(i=0;i<listaPaqCliente.size();i++)
                {
                    cout<<"EN FOR 1 siguiente seq "<<listaPaqCliente[i].getNextSeq()<<" para i "<<i<<endl;
                    printf("ack llego %u ",packet.getNumAck());
                    if(listaPaqCliente[i].getNextSeq()== packet.getNumAck())
                    {
                        cout<<"RECONOCIO"<<endl;
                        break;
                    }
                }
                cout<<"aqui salio i"<<i<<endl;

                // cout<<"num ack"<< ntohl(packet.getNumAck());
                cout<<"siguiente seq PAQUETE"<<listaPaqCliente[i].getNextSeq()<<endl;
                RTT=(packet.getTimeStamp()-listaPaqCliente[i].getTimeStamp());
                //cout<<"NO"<<endl;

                RTTEstimado  =	 0.875*RTT +	0.125	* RTT;
                cout<<"RTT"<<RTT<<endl;
                cout<<"ESTIMADO: SYN"<<RTTEstimado<<endl;


                namePacket="tcp";
                eventType = "r";

                typeFlag = listaPaqCliente[i].getSizeData() > 0 ? "DATA" : "SYN";

                trace << eventType << " " << RTT/2 << " " <<nodoCliente << " ";
                trace << nodoServidor << " " << namePacket << " " << listaPaqCliente[i].getSize() << " ";
                trace << banderas << " " <<  listaPaqCliente[i].getPortFuente()<< " " <<listaPaqCliente[i].getPortDestino() << " ";
                trace << listaPaqCliente[i].getCwnd() << " " <<listaPaqCliente[i].getSeq() << " " << listaPaqCliente[i].getId() << " " << typeFlag << endl;

                listaPaqCliente.removeAt(i);
            }
            namePacket="ack";
            eventType = "+";  /*OJO YA AQUI NO SERA RTT/2 sino el tiempo del paquete -este RTT/2*/
            trace << eventType << " " << RTT/2 << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "SYN+ACK" << endl;

            eventType = "-";
            trace << eventType << " " << RTT/2 << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " <<  packet.getId() << " " << "SYN+ACK" << endl;

            eventType = "r";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "SYN+ACK" << endl;

            ultimoSeqEnviadoServidor=packet.getSeq();
            listaPaqServidor.append(packet);


        }



    }

    else if ((packet.getACK()) && (packet.getSizeData()==0) &&not(packet.getFIN()))
    {

        cout<<"ACK  PURO"<<endl;
        cout<<"ESTIMADO:"<<RTTEstimado<<endl;
        //cout<<"fuente"<<fuente<<endl;
        //cout<<"cliente"<<nodoCliente<<endl;
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
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;

            eventType = "-";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;

            eventType = "r";
            trace << eventType << " " << packet.getTimeStamp() +RTTEstimado/2 << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;
            ultimoSeqEnviadoCliente=packet.getSeq();
        }

        else
        {

            cout<<"antes tam de lista"<<listaPaqCliente.size()<<endl;
            if(listaPaqCliente.size()>0)
            {
                for(i=0;i<listaPaqCliente.size();i++)
                {
                    cout<<"EN FOR 2 del else siguiente seq"<<listaPaqCliente[i].getNextSeq()<<endl;
                    printf("ack llego %u",packet.getNumAck());
                    if(listaPaqCliente[i].getNextSeq()== packet.getNumAck())
                    {
                        cout<<"ENTRO RECONOCIO"<<endl;
                        break;
                    }
                }
                cout<<"aqui salio i"<<i<<endl;

                //cout<<"num ack"<< ntohl(packet.getNumAck());
                cout<<"siguiente seq"<<listaPaqCliente[i].getNextSeq()<<endl;

                RTT=(packet.getTimeStamp()-listaPaqCliente[i].getTimeStamp());
                //cout<<"NO"<<endl;

                RTTEstimado  =	 0.875*RTTEstimado +	0.125	* RTT;
                cout<<"RTT"<<RTT<<endl;
                cout<<"ESTIMADO: SYN"<<RTTEstimado<<endl;

                cout<<"antes de escribir en else"<<endl;
                namePacket="tcp";
                eventType = "r";
                trace << eventType << " " << listaPaqCliente[i].getTimeStamp()+RTT/2 << " " <<nodoCliente << " ";
                trace << nodoServidor << " " << namePacket << " " << listaPaqCliente[i].getSize() << " ";
                trace << banderas << " " <<  listaPaqCliente[i].getPortFuente()<< " " <<listaPaqCliente[i].getPortDestino() << " ";
                trace << listaPaqCliente[i].getCwnd() << " " <<listaPaqCliente[i].getSeq() << " " << listaPaqCliente[i].getId() << " " << "ACK PURO" << endl;

            }

            namePacket="ack";
            eventType = "+";
            trace << eventType << " " << listaPaqCliente[i].getTimeStamp()+RTT/2 << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;

            eventType = "-";
            trace << eventType << " " << listaPaqCliente[i].getTimeStamp()+RTT/2        << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " <<  packet.getId() << " " << "ACK PURO" << endl;

            eventType = "r";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;

            ultimoSeqEnviadoServidor=packet.getSeq();
            listaPaqCliente.removeAt(i);
            listaPaqServidor.append(packet);


        }

    }

    else if( packet.getACK() and packet.getFIN() ) {

        cout<<"FYN + ACK"<<endl;
        if( fuente == nodoCliente ) {

            namePacket="tcp";
            eventType = "+";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "FIN+ACK" << endl;

            eventType = "-";
            double timeAux = packet.getSizeData() > 0? packet.getTimeStamp()+timeEncolado:packet.getTimeStamp();
            trace << eventType << " " << timeAux << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "FIN+ACK" << endl;

            ultimoSeqEnviadoCliente=packet.getSeq();
            listaPaqCliente.append(packet);

        }
        else {

            namePacket = "ack";
            eventType = "+";
            trace << eventType << " " << packet.getTimeStamp() - RTTEstimado/2 << " "<<nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "FIN+ACK" << endl;

            eventType = "-";
            trace << eventType << " " << packet.getTimeStamp() - RTTEstimado/2 <<" "<< nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "FIN+ACK" << endl;


            eventType = "r";
            trace << eventType << " " << packet.getTimeStamp() <<" "<< nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "FIN+ACK" << endl;

            ultimoACKRecibidoServidor=packet.getSeq();

            listaPaqServidor.append(packet);
        }


    }


}

void Conexion::imprimirListas()
{
    cout<<"lista de paquetes no confirmados cliente"<<endl;
    for(int i=0;i<listaPaqCliente.size();i++)
    {
        cout<<" siguiente seq"<<listaPaqCliente[i].getNextSeq()<<endl;
        printf("ack  %u",listaPaqCliente[i].getNumAck());
        printf("id  %u",listaPaqCliente[i].getId());

    }

    cout<<"lista de paquetes no confirmados servidor"<<endl;
    for(int i=0;i<listaPaqServidor.size();i++)
    {
        cout<<" siguiente seq"<<listaPaqServidor[i].getNextSeq()<<endl;
        printf("ack  %u",listaPaqServidor[i].getNumAck());
        printf("id  %u",listaPaqServidor[i].getId());

    }
}



