#include "conexion.h"


Conexion::Conexion()
{
    nodoCliente=-1;
    nodoServidor=-1;
    contadorPaquetes = 0;
    contadorPaquetesPintados=0;
    RTTEstimado=0.0;
}
Conexion::Conexion(int nodoCliente,int nodoServidor, int nuemeroConexion, TablePacket *& tablePacket)
{
    this->contadorPaquetes = 0;
    this->contadorPaquetesPintados=0;
    this->nodoCliente=nodoCliente;
    this->nodoServidor=nodoServidor;
    this->numeroConexion=nuemeroConexion;
    this->RTTEstimado=0.0;
    this->ultimoSeqEnviadoCliente=this->ultimoSeqEnviadoServidor=0;


    this->tablePacket = &(*tablePacket);

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

void Conexion::setcontadorPaquetesPintados(int contadorPaquetesPintados)
{
    this->contadorPaquetesPintados;
}

void Conexion::setcontadorPaquetes(int contadorPaquetes)
{
    this->contadorPaquetes=contadorPaquetes;
}

int Conexion:: getNumeroConexion()
{
    return(this->numeroConexion);
}

int Conexion::getcontadorPaquetesPintados()
{
    return (this->contadorPaquetesPintados);
}

int Conexion::getcontadorPaquetes()
{
    return( this->contadorPaquetes);
}

void Conexion::evaluarNuevoPaquete( Packet packet,int fuente,int destino, fstream &trace)
{

    /*Para archivo de traza*/
    string namePacket;
    string eventType = "x";
    string banderas = "-------";
    float timeEncolado = 0.004;


    contadorPaquetes++;



    printf("\n------------------------------CLASS Packet number  %d:------------------------------\n", contadorPaquetes);

    int i;
    double RTT=0.0;

    if(fuente ==nodoCliente)
        cout<<"Cliente------->Servidor"<<endl;
    else
        cout<<"Servidor------->Cliente"<<endl;

    cout<<"TAM="<<packet.getSizeData()<<endl;
    cout<<"num SEQ"<<packet.getSeq()<<endl;
    cout<<"ultimo server"<<ultimoSeqEnviadoServidor<<endl;
    cout<<"ultimo cliente"<<ultimoSeqEnviadoCliente<<endl;


    /*------------------------------Validando paquetes ----------------------------------------*/
    string typeFlag;

    if( (packet.getSYN() and not(packet.getACK() ) )  or (packet.getSizeData() > 0&& packet.getACK() ) ) {
        typeFlag = packet.getSYN() ? "SYN" : "DATA";
        cout<<typeFlag<<endl;
        if(fuente==nodoCliente)
        {
            if(packet.getSeq()<ultimoSeqEnviadoCliente) //RETRANSMISION
            {
                cout<<"RESTRANSMISION primer if"<<endl;
                for(i=0;i<listaPaqCliente.size();i++)
                {
                    if(listaPaqCliente[i].getSeq()==packet.getSeq())
                    {
                        cout<<"PAQUETE PERDIDO ID retransmision"<<listaPaqCliente[i].getId()<<endl;
                        packet.setRetransmision(true);
                        listaPaqCliente.removeAt(i);
                        break;
                    }

                }
                if(i==listaPaqCliente.size())
                {
                    cout<<"MOSCA no se consigue retransmision"<<endl;
                    // exit(0);
                }

            }
            namePacket="tcp";
            eventType = "+";


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

            if(packet.getACK())
            {
                if(ultimoACKEnviadoCliente==packet.getNumAck())
                    packet.setAckRepetido(true);

                ultimoACKEnviadoCliente=packet.getNumAck();
            }

            ultimoSeqEnviadoCliente=packet.getSeq();
            listaPaqCliente.append(packet);

        }

        else{           //En el caso SYN es enviado por el Servidor caso atipico pero sucede
            cout<<"CASO NO SOPORTADO 1"<<endl;
            double tiempo=-1;
            /*ERROR*/
            if(packet.getSeq()<ultimoSeqEnviadoServidor) //RETRANSMISION
            {
                cout<<"RESTRANSMISION primer if"<<endl;
                for(i=0;i<listaPaqServidor.size();i++)
                {
                    if(listaPaqServidor[i].getSeq()==packet.getSeq())
                    {
                        cout<<"informacion duplicada ID retransmision"<<listaPaqServidor[i].getId()<<endl;
                        packet.setRetransmision(true);
                        listaPaqServidor.removeAt(i);
                        break;
                    }

                }
                /*ver que pasa aqui*/               if(i==listaPaqServidor.size())
                {
                    cout<<"MOSCA no se consigue retransmision"<<endl;
                    //  exit(0);
                }

            }

            //lo que se recibio reconoce algo??

            if(packet.getACK())
            {
                for(i=0;i<listaPaqCliente.size();i++)
                {
                    cout<<"EN FOR 1 siguiente seq "<<listaPaqCliente[i].getNextSeq()<<" para i "<<i<<endl;
                    printf("ack llego %u ",packet.getNumAck());
                    if(listaPaqCliente[i].getNextSeq()== packet.getNumAck())
                    {
                        packet.setAcusado(listaPaqCliente[i].getNumberPacketCaptured());
                        cout<<"RECONOCIO"<<packet.getAcusado()<<endl;
                        break;
                    }
                }
                if(i<listaPaqCliente.size())
                {
                    namePacket="tcp";
                    RTT=actualizarRttEstimado(packet.getTimeStamp(),listaPaqCliente[i].getTimeStamp());


                    tiempo=packet.getTimeStamp()-RTT/2;

                    eventType = "r";
                    contadorPaquetesPintados++;

                    trace << eventType << " " << tiempo<< " " << nodoCliente << " ";
                    trace << nodoServidor << " " << namePacket << " " << listaPaqCliente[i].getSize() << " ";
                    trace << banderas << " " <<listaPaqCliente[i].getPortFuente() << " " << listaPaqCliente[i].getPortDestino()<< " ";
                    trace << listaPaqCliente[i].getCwnd() << " " <<listaPaqCliente[i].getSeq() << " " << listaPaqCliente[i].getId() << " "<< typeFlag << endl;



                }


                if(ultimoACKRecibidoServidor==packet.getNumAck())
                    packet.setAckRepetido(true);

                ultimoACKRecibidoServidor=packet.getNumAck();



            }

            if(tiempo==-1)
            {
                tiempo=packet.getTimeStamp() -RTTEstimado/2;
            }

            namePacket="ack";
            eventType = "+";


            trace << eventType << " " << tiempo<< " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " "<< typeFlag << endl;

            eventType = "-";
            trace << eventType << " " << tiempo << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " "<< typeFlag << endl;

            eventType = "r";
            contadorPaquetesPintados++;
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " "<< typeFlag << endl;

            ultimoSeqEnviadoServidor=packet.getSeq();
            listaPaqServidor.append(packet);
            /*ERROR*/

        }

    }

    else if (  (packet.getACK() ) && (packet.getSYN() )  )
    {
        cout<<"SYN+ACK"<<endl;
        if(fuente==nodoServidor)
        {
            double tiempo=-1;
            if(packet.getSeq()<ultimoSeqEnviadoServidor) //RETRANSMISION
            {
                cout<<"RESTRANSMISION primer if"<<endl;
                for(i=0;i<listaPaqServidor.size();i++)
                {
                    if(listaPaqServidor[i].getSeq()==packet.getSeq())
                    {
                        packet.setRetransmision(true);
                        listaPaqServidor.removeAt(i);
                        break;
                    }

                }
                if(i==listaPaqServidor.size())
                {
                    cout<<"MOSCA no se consigue retransmision 2"<<endl;
                    // exit(0);
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

                        packet.setAcusado(listaPaqCliente[i].getNumberPacketCaptured());
                        cout<<"RECONOCIO"<<packet.getAcusado()<<endl;
                        break;
                    }
                }
                cout<<"aqui salio i"<<i<<endl;
                if(i<listaPaqCliente.size())
                {

                    cout<<"siguiente seq PAQUETE"<<listaPaqCliente[i].getNextSeq()<<endl;
                    RTT=actualizarRttEstimado(packet.getTimeStamp(),listaPaqCliente[i].getTimeStamp());


                    tiempo =RTT/2;

                    cout<<"RTT"<<RTT<<endl;
                    cout<<"ESTIMADO: SYN"<<RTTEstimado<<endl;


                    namePacket="tcp";
                    eventType = "r";
                    contadorPaquetesPintados++;
                    typeFlag = listaPaqCliente[i].getSizeData() > 0 ? "DATA" : "SYN";

                    trace << eventType << " " <<listaPaqCliente[i].getTimeStamp()+ tiempo<< " " <<nodoCliente << " ";
                    trace << nodoServidor << " " << namePacket << " " << listaPaqCliente[i].getSize() << " ";
                    trace << banderas << " " <<  listaPaqCliente[i].getPortFuente()<< " " <<listaPaqCliente[i].getPortDestino() << " ";
                    trace << listaPaqCliente[i].getCwnd() << " " <<listaPaqCliente[i].getSeq() << " " << listaPaqCliente[i].getId() << " " << typeFlag << endl;

                    listaPaqCliente.removeAt(i);
                }
            }

            if(tiempo==-1)
            {
                tiempo=RTTEstimado/2;
            }





            namePacket="ack";
            eventType = "+";
            trace << eventType << " " << packet.getTimeStamp()-tiempo << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "SYN+ACK" << endl;

            eventType = "-";
            trace << eventType << " " <<packet.getTimeStamp()- tiempo<< " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " <<  packet.getId() << " " << "SYN+ACK" << endl;

            eventType = "r";
            contadorPaquetesPintados++;
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "SYN+ACK" << endl;

            if(ultimoACKRecibidoServidor==packet.getNumAck())
                packet.setAckRepetido(true);

            ultimoACKRecibidoServidor=packet.getNumAck();

            ultimoSeqEnviadoServidor=packet.getSeq();
            listaPaqServidor.append(packet);


        }

        else   //EL cliente envia el SYN+ACK caso atipico pero sucede
        {
            cout<<"CASO NO SOPORTADO 2"<<endl;

            /*else cliente syn +ack*/

            for(i=0;i<listaPaqServidor.size();i++)
            {
                if(listaPaqServidor[i].getNextSeq()== packet.getNumAck()) {
                    packet.setAcusado(listaPaqServidor[i].getNumberPacketCaptured());
                    cout<<"RECONOCIO"<<packet.getAcusado()<<endl;
                    listaPaqServidor.removeAt(i);
                    break;
                }
            }


            namePacket="tcp";
            eventType = "+";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "SYN+ACK" << endl;

            eventType = "-";
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "SYN+ACK" << endl;

            eventType = "r";
            contadorPaquetesPintados++;
            trace << eventType << " " << packet.getTimeStamp() +RTTEstimado/2 << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "SYN+ACK" << endl;
            ultimoSeqEnviadoCliente=packet.getSeq();

            if(ultimoACKEnviadoCliente==packet.getNumAck())
                packet.setAckRepetido(true);

            ultimoACKEnviadoCliente=packet.getNumAck();

            /*fin else cliente syn+ack*/

        }

    }

    else if ((packet.getACK()) && (packet.getSizeData()==0) &&not(packet.getFIN()))
    {

        cout<<"ACK  PURO"<<endl;
        cout<<"ESTIMADO:"<<RTTEstimado<<endl;

        if(fuente==nodoCliente)
        {

            for(i=0;i<listaPaqServidor.size();i++)
            {
                if(listaPaqServidor[i].getNextSeq()== packet.getNumAck()) {

                    actualizarRttEstimado(packet.getTimeStamp(),listaPaqServidor[i].getTimeStamp());
                    packet.setAcusado(listaPaqServidor[i].getNumberPacketCaptured());
                    cout<<"RECONOCIO"<<packet.getAcusado()<<endl;
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
            contadorPaquetesPintados++;
            trace << eventType << " " << packet.getTimeStamp() +RTTEstimado/2 << " " << nodoCliente << " ";
            trace << nodoServidor << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " <<packet.getPortFuente() << " " << packet.getPortDestino()<< " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;
            ultimoSeqEnviadoCliente=packet.getSeq();

            if(ultimoACKEnviadoCliente==packet.getNumAck())
                packet.setAckRepetido(true);

            ultimoACKEnviadoCliente=packet.getNumAck();
        }

        else
        {
            double tiempo=-1;
            cout<<"antes tam de lista"<<listaPaqCliente.size()<<endl;
            if(listaPaqCliente.size()>0)
            {
                for(i=0;i<listaPaqCliente.size();i++)
                {
                    cout<<"EN FOR 2 del else siguiente seq"<<listaPaqCliente[i].getNextSeq()<<endl;
                    printf("ack llego %u",packet.getNumAck());
                    if(listaPaqCliente[i].getNextSeq()== packet.getNumAck())
                    {
                        packet.setAcusado(listaPaqCliente[i].getNumberPacketCaptured());
                        cout<<"RECONOCIO"<<packet.getAcusado()<<endl;
                        break;
                    }
                }
                cout<<"aqui salio i"<<i<<endl;
                if(i<listaPaqCliente.size())
                {
                    //cout<<"num ack"<< ntohl(packet.getNumAck());
                    cout<<"siguiente seq"<<listaPaqCliente[i].getNextSeq()<<endl;

                    RTT=actualizarRttEstimado(packet.getTimeStamp(),listaPaqCliente[i].getTimeStamp());
                    //cout<<"NO"<<endl;
                    tiempo=RTT/2;

                    cout<<"RTT"<<RTT<<endl;
                    cout<<"ESTIMADO: SYN"<<RTTEstimado<<endl;

                    cout<<"antes de escribir en else"<<endl;
                    namePacket="tcp";
                    eventType = "r";
                    contadorPaquetesPintados++;
                    trace << eventType << " " << listaPaqCliente[i].getTimeStamp()+tiempo<< " " <<nodoCliente << " ";
                    trace << nodoServidor << " " << namePacket << " " << listaPaqCliente[i].getSize() << " ";
                    trace << banderas << " " <<  listaPaqCliente[i].getPortFuente()<< " " <<listaPaqCliente[i].getPortDestino() << " ";
                    trace << listaPaqCliente[i].getCwnd() << " " <<listaPaqCliente[i].getSeq() << " " << listaPaqCliente[i].getId() << " " << "ACK PURO" << endl;
                }
            }
            if(tiempo==-1)
            {
                tiempo=RTTEstimado/2;
            }

            namePacket="ack";
            eventType = "+";
            trace << eventType << " " << packet.getTimeStamp()-tiempo<< " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;

            eventType = "-";
            trace << eventType << " " <<  packet.getTimeStamp()-tiempo     << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " <<  packet.getId() << " " << "ACK PURO" << endl;

            eventType = "r";
            contadorPaquetesPintados++;
            trace << eventType << " " << packet.getTimeStamp() << " " << nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " <<packet.getSeq() << " " << packet.getId() << " " << "ACK PURO" << endl;

            ultimoSeqEnviadoServidor=packet.getSeq();

            if(ultimoACKRecibidoServidor==packet.getNumAck())
                packet.setAckRepetido(true);

            ultimoACKRecibidoServidor=packet.getNumAck();

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

            if(ultimoACKEnviadoCliente==packet.getNumAck())
                packet.setAckRepetido(true);

            ultimoACKEnviadoCliente=packet.getNumAck();

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
            contadorPaquetesPintados++;
            trace << eventType << " " << packet.getTimeStamp() <<" "<< nodoServidor << " ";
            trace << nodoCliente << " " << namePacket << " " << packet.getSize() << " ";
            trace << banderas << " " << packet.getPortFuente() << " " << packet.getPortDestino() << " ";
            trace << packet.getCwnd() << " " << packet.getSeq() << " " << packet.getId() << " " << "FIN+ACK" << endl;

            ultimoSeqEnviadoServidor=packet.getSeq();

            if(ultimoACKRecibidoServidor==packet.getNumAck())
                packet.setAckRepetido(true);

            ultimoACKRecibidoServidor=packet.getNumAck();
            listaPaqServidor.append(packet);
        }


    }

    tablePacket->addPacket(packet);
}

void Conexion::imprimirListas()
{
    cout<<"lista de paquetes no confirmados cliente"<<endl;
    for(int i=0;i<listaPaqCliente.size();i++)
    {
        cout<<"paquete i="<<i<<endl;
        cout<<" siguiente seq"<<listaPaqCliente[i].getNextSeq()<<endl;
        printf("ack  %u \n",listaPaqCliente[i].getNumAck());
        printf("id  %u \n",listaPaqCliente[i].getId());

    }

    cout<<"lista de paquetes no confirmados servidor"<<endl;
    for(int i=0;i<listaPaqServidor.size();i++)
    {
        cout<<"paquete i="<<i<<endl;
        cout<<" siguiente seq"<<listaPaqServidor[i].getNextSeq()<<endl;
        printf("ack  %u \n",listaPaqServidor[i].getNumAck());
        printf("id  %u\n",listaPaqServidor[i].getId());

    }
}

double Conexion::actualizarRttEstimado(double tLlegada, double tSalida)
{
    double RTT=tLlegada-tSalida;


    if(RTTEstimado==0.0)                              //no hay muestras esta es la primera?
        RTTEstimado  =	 RTT;

    else
        RTTEstimado  =	 0.875*RTTEstimado +	0.125	* RTT;

    return RTT;
}



