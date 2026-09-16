QT       += core gui widgets
CONFIG   += c++17
TARGET    = CinemaUsac
TEMPLATE  = app

SOURCES += \
    main.cpp \
    InterfazGrafica/mainwindow.cpp \
    InterfazGrafica/PerfilDialog.cpp \
    Estructuras/ArbolPeliculas.cpp \
    Estructuras/ArbolAVL.cpp \
    Estructuras/ArbolB.cpp \
    Estructuras/TablaHash.cpp \
    Estructuras/MatrizAsientos.cpp \
    Estructuras/ListaCircularDoble.cpp \
    Estructuras/ListaDeListas.cpp \
    Modelos/Json.cpp \
    Persistencia/Persistencia.cpp \
    Reportes/GeneradorDot.cpp

HEADERS += \
    InterfazGrafica/mainwindow.h \
    InterfazGrafica/PerfilDialog.h \
    Modelos/Pelicula.h \
    Modelos/Funcion.h \
    Modelos/Cliente.h \
    Modelos/Reserva.h \
    Modelos/Promocion.h \
    Modelos/Solicitud.h \
    Modelos/Json.h \
    Utilidades/Validaciones.h \
    Estructuras/NodoArbol.h \
    Estructuras/ArbolPeliculas.h \
    Estructuras/NodoAVL.h \
    Estructuras/ArbolAVL.h \
    Estructuras/NodoB.h \
    Estructuras/ArbolB.h \
    Estructuras/NodoHash.h \
    Estructuras/TablaHash.h \
    Estructuras/NodoMatriz.h \
    Estructuras/MatrizAsientos.h \
    Estructuras/NodoListaDoble.h \
    Estructuras/ListaCircularDoble.h \
    Estructuras/NodoListaListas.h \
    Estructuras/ListaDeListas.h \
    Persistencia/Persistencia.h \
    Reportes/GeneradorDot.h