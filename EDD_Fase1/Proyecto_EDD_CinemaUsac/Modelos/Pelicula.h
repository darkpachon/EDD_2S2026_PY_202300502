#ifndef PELICULA_H
#define PELICULA_H
#include <string>

using namespace std;

class Pelicula {
public:
    int codigo;              
    string codigoOriginal;   
    string titulo;
    string genero;
    int duracion;
    string clasificacion;
    string idioma;
    string fecha_estreno;
    string fecha_fin;

     
    Pelicula(int _codigo, string _codigoOriginal, string _titulo, string _genero, int _duracion, string _clasificacion, string _idioma, string _fecha_estreno, string _fecha_fin) {
        codigo = _codigo;
        codigoOriginal = _codigoOriginal;
        titulo = _titulo;
        genero = _genero;
        duracion = _duracion;
        clasificacion = _clasificacion;
        idioma = _idioma;
        fecha_estreno = _fecha_estreno;
        fecha_fin = _fecha_fin;
    }

     
    Pelicula() : codigo(0), duracion(0) {}
};

#endif