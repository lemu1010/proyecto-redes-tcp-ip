#-------------------------------------------------
#
# Project created by QtCreator 2012-02-03T13:28:32
#
#-------------------------------------------------

QT       += core gui

TARGET = ProyectoRedes
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    conexion.cpp \
    packet.cpp \
    pcapthread.cpp \
    tablepacket.cpp \
    textpacket.cpp


HEADERS  += mainwindow.h \
    conexion.h \
    struct.h \
    packet.h \
    pcapthread.h \
    tablepacket.h \
    pcapthread.h \
    tablepacket.h \
    textpacket.h

FORMS    += mainwindow.ui

#Incluir de libreria pcap.
CONFIG += warn_off

INCLUDEPATH += /usr/include/pcap
#INCLUDEPATH += /home/antonio/lipcap-1.1.1/pcap

LIBS += -L/usr/lib -lpcap
#LIBS += -L/home/antonio/lipcap-1.1.1/pcap -lpcap

DEPENDPATH += /usr/include/pcap
#DEPENDPATH += /home/antonio/lipcap-1.1.1/pcap

RESOURCES += \
    recursos.qrc \
    recursos.qrc

OTHER_FILES += \
    Proyectopy/ns2.py \
    Proyectopy/main.py \
    Proyectopy/execImage.sh \
    Proyectopy/canvas.py \
    Proyectopy/svg.py \
    trazaReal.tr \
    images/zoom-out.png \
    images/zoom-normal.png \
    images/zoom-in.png \
    images/wifi_modem.png \
    images/ver.png \
    images/stop.png \
    images/play.png \
    images/nuevo.png \
    images/logo.png \
    images/guardar.png \
    images/flowTCP.png \
    images/ethernet_card.png \
    images/cerrar.png \
    images/cancel.png \
    images/ajustar.png \
    images/acercade.png \
    images/aceptar.png \
    images/abrir.png \
    Proyectopy/out.svg \
    Proyectopy/trazaReal.tr

