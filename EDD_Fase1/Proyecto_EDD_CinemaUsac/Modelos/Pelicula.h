#ifndef PELICULA_H
#define PELICULA_H
#include <string>

using namespace std;

class Pelicula {
public:
    int codigo;             // Ej: 34 (El número interno que usa tu Árbol Binario para ordenar)
    string codigoOriginal;  // Ej: "P034" (El texto real para mostrar en pantalla)
    string titulo;
    string genero;
    int duracion;
    string clasificacion;
    string idioma;
    string fecha_estreno;
    string fecha_fin;

    // Constructor completo
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

    // Constructor vacío por defecto
    Pelicula() : codigo(0), duracion(0) {}
};

#endif // PELICULA_H