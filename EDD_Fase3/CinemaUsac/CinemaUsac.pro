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
    Estructuras/GrafoSedes.cpp \
    Estructuras/MerkleArbol.cpp \
    Estructuras/Blockchain.cpp \
    Utilidades/SHA256.cpp \
    Utilidades/LZW.cpp \
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
    Modelos/Sede.h \
    Modelos/Json.h \
    Utilidades/Validaciones.h \
    Utilidades/SHA256.h \
    Utilidades/LZW.h \
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
    Estructuras/GrafoSedes.h \
    Estructuras/MerkleArbol.h \
    Estructuras/Blockchain.h \
    Persistencia/Persistencia.h \
    Reportes/GeneradorDot.h