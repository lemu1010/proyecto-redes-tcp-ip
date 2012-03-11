#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <pcapthread.h>
#include <tablepacket.h>

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    void initGUI();
    void initComboBox();
    void createAction();
    void createMenu();
    void createToolBar();
    void enableEstandar(bool enabled);
    void messageBox(std::string message);

private slots:

    void slotAbrir();
    void slotGuardar();
    void slotSalir();
    void slotPlayCaptura();
    void slotStopCaptura();
    void slotFlowTcp();
    void slotAcercaDe();

private:
    Ui::MainWindow *ui;

    QAction *actionAbrir;
    QAction *actionGuardar;
    QAction *actionSalir;
    QAction *actionPlayCapture;
    QAction *actionStopCapture;
    QAction *actionFlujoTcp;
    QAction *actionAcerca;

    QComboBox *boxDevice;

    QMenu *menuArchivo;
    QMenu *menuCaptura;
    QMenu *menuGrafica;
    QMenu *menuAyuda;

    QToolBar *mainToolBar;

    QScrollBar *marco;

    PcapThread *pcapThread;

    bool working;

    TablePacket *tablePacket;

};

#endif // MAINWINDOW_H
