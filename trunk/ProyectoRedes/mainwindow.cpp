#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    initGUI();
    createAction();
    createMenu();
    createToolBar();

}

void  MainWindow::initGUI()
{
    setWindowTitle("WireFish");
    setWindowIcon(QIcon(":/images/logo.png"));

    ancho = QApplication::desktop()->width();
    alto = QApplication::desktop()->height();

    QRect geometry = this->geometry();
    geometry.moveCenter(QApplication::desktop()->availableGeometry().center());
    setGeometry(geometry);
    showMaximized();

    working = false;
    initComboBox();

    //Codificación de caracteres UTF-8
    QTextCodec *linuxCodec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForTr(linuxCodec);
    QTextCodec::setCodecForCStrings(linuxCodec);
    QTextCodec::setCodecForLocale(linuxCodec);
}

void MainWindow::createAction()
{
    actionAbrir = new QAction(QIcon(":/images/abrir.png"),tr("&Abrir"),this);
    actionAbrir->setShortcut(tr("Ctrl+o"));
    actionAbrir->setStatusTip("Abrir un archivo de captura de trazas");
    actionAbrir->setToolTip("Abrir un archivo de captura de trazas");
    connect(actionAbrir, SIGNAL(triggered()), this, SLOT(slotAbrir()));

    actionGuardar = new QAction(QIcon(":/images/guardar.png"),tr("&Guardar"),this);
    actionGuardar->setShortcut(tr("Ctrl+s"));
    actionGuardar->setStatusTip("Guardar las trazas capturadas");
    actionGuardar->setToolTip("Guardar las trazas capturadas");
    actionGuardar->setEnabled(false);
    connect(actionGuardar, SIGNAL(triggered()), this, SLOT(slotGuardar()));

    actionSalir = new QAction(QIcon(":/images/cerrar.png"),tr("&Salir"),this);
    actionSalir->setShortcut(tr("Ctrl+q"));
    actionSalir->setStatusTip("Salir de la aplicación");
    connect(actionSalir, SIGNAL(triggered()), this, SLOT(slotSalir()));

    actionPlayCapture = new QAction(QIcon(":/images/play.png"),tr("&Empezar Captura"),this);
    actionPlayCapture->setShortcut(tr("Ctrl+e"));
    actionPlayCapture->setStatusTip("Empezar captura de paquetes");
    actionPlayCapture->setToolTip("Empezar captura de paquetes");
    connect(actionPlayCapture, SIGNAL(triggered()), this, SLOT(slotPlayCaptura()));

    actionStopCapture = new QAction(QIcon(":/images/stop.png"),tr("&Parar Captura"), this);
    actionStopCapture->setShortcut(tr("Ctrl+p"));
    actionStopCapture->setStatusTip("Parar captura de paquetes");
    actionStopCapture->setToolTip("Parar captura de paquetes");
    actionStopCapture->setEnabled(false);
    connect(actionStopCapture, SIGNAL(triggered()), this, SLOT(slotStopCaptura()));

    actionFlujoTcp = new QAction(QIcon(":/images/flowTCP.png"),tr("Grafica Flujo TCP"), this);
    actionFlujoTcp->setStatusTip("Muestra grafica flujo tcp");
    actionFlujoTcp->setToolTip("Muestra grafica flujo tcp");
    actionFlujoTcp->setEnabled(false);
    connect(actionFlujoTcp, SIGNAL(triggered()), this, SLOT(slotFlowTcp()));

    actionTraza = new QAction(tr("Archivo Traza"), this);
    actionTraza->setStatusTip("Muestra el archivo de trazas");
    actionTraza->setToolTip("Muestra el archivo de trazas");
    actionTraza->setEnabled(false);
    connect(actionTraza, SIGNAL(triggered()), this, SLOT(slotFileTraza()));

    actionAcerca = new QAction(QIcon(":/images/acercade.png"),tr("Acerca"),this);
    connect(actionAcerca, SIGNAL(triggered()), this, SLOT(slotAcercaDe()));
}

void MainWindow::initComboBox()
{
    pcapThread = new PcapThread;

    boxDevice = new QComboBox(this);
    connect(boxDevice, SIGNAL(currentIndexChanged(QString)), pcapThread, SLOT(setInterfaceName(QString)));

    QList<Interface> listaInterface = pcapThread->getInterfaces();

    pcapThread->terminate();

    QIcon wifiIcon(":images/wifi_modem.png");
    QIcon ethIcon(":images/ethernet_card.png");

    foreach(Interface interface, listaInterface ) {

        if(interface.datalink_type == DLT_IEEE802_11) /* Wifi */
            boxDevice->addItem(wifiIcon,interface.name);

        if(interface.datalink_type == DLT_EN10MB) /* Ethernet 10MB */
            boxDevice->addItem(ethIcon,interface.name);
        else
            boxDevice->addItem(ethIcon,interface.name);
    }

}

void MainWindow::createMenu()
{
    menuArchivo = menuBar()->addMenu("&Archivo");
    menuArchivo->addAction(actionAbrir);
    menuArchivo->addSeparator();
    menuArchivo->addAction(actionGuardar);
    menuArchivo->addSeparator();
    menuArchivo->addAction(actionSalir);

    menuCaptura = menuBar()->addMenu("&Captura");
    menuCaptura->addAction(actionPlayCapture);
    menuCaptura->addAction(actionStopCapture);

    menuGrafica = menuBar()->addMenu("&Grafica");
    menuGrafica->addAction(actionFlujoTcp);

    menuTraza = menuBar()->addMenu("&Traza");
    menuTraza->addAction(actionTraza);

    menuAyuda = menuBar()->addMenu("A&yuda");
    menuAyuda->addAction(actionAcerca);
}

void MainWindow::createToolBar()
{
    mainToolBar = new QToolBar();
    mainToolBar->addAction(actionAbrir);
    mainToolBar->addAction(actionGuardar);
    mainToolBar->addSeparator();
    mainToolBar->addWidget(boxDevice);
    mainToolBar->addAction(actionPlayCapture);
    mainToolBar->addAction(actionStopCapture);
    mainToolBar->setMovable(false);
    addToolBar(mainToolBar);
}

void MainWindow::slotAbrir()
{

}

void MainWindow::slotGuardar()
{
    working = false;
}

void MainWindow::closeEvent(QCloseEvent * evento)
{
    if( QMessageBox::question(this, tr(" "),tr("<center>¿Esta seguro que desea salir?"),
                              QMessageBox::Yes | QMessageBox::No ) == QMessageBox::Yes )
        ::exit(0);

    evento->ignore();
}

void MainWindow::slotSalir()
{
    this->close();
}

void MainWindow::slotPlayCaptura()
{

    QString name = boxDevice->currentText();
    QString nameFile("../ProyectoRedes/Proyectopy/trazaReal.tr");

    pcapThread = new PcapThread;
    pcapThread->setInterfaceName(name);

    if( !working ) {

        if( pcapThread->initInterface() ) {

            splitPane = new QSplitter(Qt::Vertical);

            tablePacket = new TablePacket;
            connect(tablePacket, SIGNAL(cellClicked(int,int)), tablePacket, SLOT(selectRow(int)));
            splitPane->addWidget(tablePacket);

            treePacket = new TreePacket();
            connect(tablePacket, SIGNAL(cellClicked(int,int)), treePacket, SLOT(setRowPackect(int,int)));
            splitPane->addWidget(treePacket);

//            textPayload=new TextPayload();
//            splitPane->addWidget(textPayload);
//            connect(tablePacket,SIGNAL(cellClicked(Packet)),textPayload,SLOT(setTextPayload(Packet)));

            setCentralWidget(splitPane);

            pcapThread->setTablePacket(tablePacket);
            pcapThread->setTreePacket(treePacket);

            if(!pcapThread->setFile(nameFile))
                 messageBox("ERROR: no ha sido posible establecer archivo de traza");

            pcapThread->started = true;
            pcapThread->start();

            actionStopCapture->setEnabled(true);
            actionGuardar->setEnabled(true);
            actionPlayCapture->setEnabled(false);

            working = true;
        }
        else {
            std::string cad = "ERROR: " + pcapThread->getPcapError();
            messageBox(cad);
        }
    }
    else {
        clearEvent();
    }

}

void MainWindow::slotStopCaptura()
{
    if( not pcapThread->started )
        return;

    if( pcapThread->handler == NULL )
        return;

    pcap_breakloop(pcapThread->handler);
    pcap_close(pcapThread->handler);

    if( pcapThread->isRunning() ) {
        pcapThread->resetValues();
        pcapThread->closeFile();
        pcapThread->terminate();
    }

    pcapThread->started = false;

    actionStopCapture->setEnabled(false);
    actionPlayCapture->setEnabled(true);
    actionFlujoTcp->setEnabled(true);
    actionTraza->setEnabled(true);

}

void MainWindow::slotFlowTcp()
{

    QProcess bash;
    bash.setWorkingDirectory("Proyectopy/");
    qDebug()<<bash.workingDirectory()<<" este es el directorio"<<endl;
    bash.start("bash");


    if(!bash.waitForStarted()){
        messageBox("ERROR: bash no responde grafica no podra ser creada");

    }
    bash.write("sh execImage.sh");


    bash.closeWriteChannel();
    if(!bash.waitForFinished()){
        messageBox("ERROR: grafica consumiendo recursos en sistemas ");

    }
    QByteArray response = bash.readAll();
    qDebug() << response.data();
    bash.close();

}

void MainWindow::slotFileTraza()
{
    QProcess bash;
    bash.setWorkingDirectory("Proyectopy/");
    bash.start("bash");

    if(!bash.waitForStarted()){
        messageBox("ERROR: bash no responde grafica no podra ser creada");

    }
    bash.write("gedit trazaReal.tr &");


    bash.closeWriteChannel();
    if(!bash.waitForFinished()){
        messageBox("ERROR: grafica consumiendo recursos en sistemas ");

    }

    bash.close();
}

void MainWindow::clearEvent()
{

    if( QMessageBox::question(this,tr(" "),tr("<center>¿Esta seguro que desea descartar la traza actual?"),//
                              QMessageBox::No | QMessageBox::Yes) == QMessageBox::Yes )
        clearWidgets();


}

void MainWindow::slotAcercaDe()
{
    QString titulo = "<H2> WireFish </H2>";
    QString version = "<H5> Version 1.0 </H5> <br>";
    QString descripcion1 = "Es un pequeño sistema que muestra el flujo TCP <br><br>";
    QString desarrollador1 = "Desarrollado por: <br><br>";
    QString desarrollador2 = "Jonathan Monsalve <br> Julio Muchacho <br> Fernando Osuna <br> Antonio López <br><br>";
    QString tituloEmail = "Dirección electronica: <br>";
    QString email = "j.jmonsalveg@gmail.com <br> muchacho.julio@gmail.com <br> osunaf@ula.ve <br> antoniol@ula.ve <br><br>";
    QString credito = "Postscript Python: <br><br>";
    QString desarrollador3 = "Miguel Flores <br><b><br>";
    QString lugar = "Universidad de los Andes, 2012.";

    QMessageBox::about(this, tr("Acerca de Proyecto Redes"), "<center>" + titulo +
                       version + descripcion1 + desarrollador1 + desarrollador2 +
                       tituloEmail + email + credito + desarrollador3 + "<font color = #FF8000>" + lugar +
                       "</font>" + "</center>");
}

void MainWindow::messageBox(std::string message)
{
    QMessageBox::warning(this,"warning",message.c_str());
}


void MainWindow::clearWidgets()
{

    working = false;

    actionFlujoTcp->setEnabled(false);
    actionTraza->setEnabled(false);

    treePacket->clear();
    delete treePacket;

    tablePacket->clearContents();
    delete tablePacket;

    pcapThread->started = false;

    pcapThread->resetValues();
    pcapThread->terminate();
}

MainWindow::~MainWindow()
{
    delete ui;
}

