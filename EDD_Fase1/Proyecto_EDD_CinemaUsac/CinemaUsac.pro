QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

TARGET = CinemaUsac
TEMPLATE = app

SOURCES += \
    main.cpp \
    InterfazGrafica/mainwindow.cpp \
    Estructuras/ArbolPeliculas.cpp \
    Estructuras/MatrizAsientos.cpp \
    Estructuras/ListaCircularDoble.cpp \
    Estructuras/ListaDeListas.cpp \
    Reportes/GeneradorDot.cpp

HEADERS += \
    InterfazGrafica/mainwindow.h \
    Modelos/Pelicula.h \
    Modelos/Promocion.h \
    Modelos/Solicitud.h \
    Estructuras/NodoArbol.h \
    Estructuras/ArbolPeliculas.h \
    Estructuras/NodoMatriz.h \
    Estructuras/MatrizAsientos.h \
    Estructuras/NodoListaDoble.h \
    Estructuras/ListaCircularDoble.h \
    Estructuras/NodoListaListas.h \
    Estructuras/ListaDeListas.h \
    Reportes/GeneradorDot.h

FORMS += \
    InterfazGrafica/mainwindow.ui