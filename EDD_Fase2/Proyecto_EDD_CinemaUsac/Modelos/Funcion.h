#ifndef FUNCION_H
#define FUNCION_H
#include <string>
using namespace std;

// Representa una función (proyección) indexada por codigo_funcion en el AVL.
class Funcion {
public:
    string codigoFuncion;    // Ej: F001  -- clave del AVL
    string codigoPelicula;   // Ej: P001  -- referencia a la película
    string tituloPelicula;   // Conveniencia para mostrar en UI
    string horario;          // Ej: "17:00"
    string sala;             // Ej: "Sala 2"
    int filas;
    int columnas;
    string archivoAsientos;  // Ej: "F001_funcion.json"

    Funcion() : filas(0), columnas(0) {}

    Funcion(string _codigoFuncion, string _codigoPelicula, string _tituloPelicula,
            string _horario, string _sala, int _filas, int _columnas, string _archivoAsientos)
        : codigoFuncion(_codigoFuncion), codigoPelicula(_codigoPelicula),
          tituloPelicula(_tituloPelicula), horario(_horario), sala(_sala),
          filas(_filas), columnas(_columnas), archivoAsientos(_archivoAsientos) {}
};

#endif