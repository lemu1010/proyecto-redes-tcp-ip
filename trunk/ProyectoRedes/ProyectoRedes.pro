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
    conexion.cpp


HEADERS  += mainwindow.h \
    conexion.h \
    struct.h

FORMS    += mainwindow.ui

#Incluir de libreria pcap.
CONFIG += warn_off

INCLUDEPATH += /usr/include/pcap
#INCLUDEPATH += /home/antonio/lipcap-1.1.1/pcap

LIBS += -L/usr/lib -lpcap
#LIBS += -L/home/antonio/lipcap-1.1.1/pcap -lpcap

DEPENDPATH += /usr/include/pcap
#DEPENDPATH += /home/antonio/lipcap-1.1.1/pcap
