#ifndef PELICULA_H
#define PELICULA_H

#include <string>
using namespace std;

class Pelicula {
public:
    // Variables de la película
    int codigo;
    string titulo;
    string director;
    int anio;
    int duracion; // en minutos

    // Constructor: Es la función que se llama automáticamente al crear una película nueva
    Pelicula(int _codigo, string _titulo, string _director, int _anio, int _duracion) {
        codigo = _codigo;
        titulo = _titulo;
        director = _director;
        anio = _anio;
        duracion = _duracion;
    }

    // Constructor vacío por defecto (necesario en C++ a veces para inicializar nodos)
    Pelicula() {
        codigo = 0;
        titulo = "";
        director = "";
        anio = 0;
        duracion = 0;
    }
};

#endif // PELICULA_H