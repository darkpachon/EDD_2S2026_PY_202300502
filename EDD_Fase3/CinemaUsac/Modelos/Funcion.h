#ifndef FUNCION_H
#define FUNCION_H
#include <string>
using namespace std;

class Funcion {
public:
    string codigoFuncion;
    string codigoPelicula;
    string tituloPelicula;
    string horario;
    string sala;
    int filas;
    int columnas;
    string archivoAsientos;
    string sede;              // "s001" — Fase 3

    Funcion() : filas(0), columnas(0) {}

    Funcion(string _codigoFuncion, string _codigoPelicula, string _tituloPelicula,
            string _horario, string _sala, int _filas, int _columnas,
            string _archivoAsientos, string _sede = "")
        : codigoFuncion(_codigoFuncion), codigoPelicula(_codigoPelicula),
          tituloPelicula(_tituloPelicula), horario(_horario), sala(_sala),
          filas(_filas), columnas(_columnas), archivoAsientos(_archivoAsientos),
          sede(_sede) {}
};

#endif