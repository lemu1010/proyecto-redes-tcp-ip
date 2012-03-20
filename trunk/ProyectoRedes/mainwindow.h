#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <pcapthread.h>
#include <tablepacket.h>
#include <textpacket.h>
#include <textpayload.h>

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
    void closeEvent(QCloseEvent *);
    void clearEvent();
    void clearWidgets();

private slots:

    void slotAbrir();
    void slotGuardar();
    void slotSalir();
    void slotPlayCaptura();
    void slotStopCaptura();
    void slotFlowTcp();
    void slotFileTraza();
    void slotAcercaDe();

private:
    Ui::MainWindow *ui;

    QAction *actionAbrir;
    QAction *actionGuardar;
    QAction *actionSalir;
    QAction *actionPlayCapture;
    QAction *actionStopCapture;
    QAction *actionFlujoTcp;
    QAction *actionTraza;
    QAction *actionAcerca;

    QComboBox *boxDevice;

    QMenu *menuArchivo;
    QMenu *menuCaptura;
    QMenu *menuGrafica;
    QMenu *menuTraza;
    QMenu *menuAyuda;

    QToolBar *mainToolBar;

    PcapThread *pcapThread;

    bool working;
    int alto;

    QSplitter *splitPane;
    TextPacket * textPacket;
    TablePacket *tablePacket;
    TextPayload   *textPayload;

};

#endif // MAINWINDOW_H
