#ifndef GENERADORDOT_H
#define GENERADORDOT_H

#include <string>
#include <fstream>
#include "../Estructuras/ArbolPeliculas.h"
#include "../Estructuras/ListaCircularDoble.h"
#include "../Estructuras/MatrizAsientos.h"
#include "../Estructuras/ListaDeListas.h"

class GeneradorDot {
private:
    void generarNodos(NodoArbol* nodo, std::ofstream& archivo);

public:
    void graficarArbol(ArbolPeliculas* arbol, std::string rutaSalida);
    void graficarListaDoble(ListaCircularDoble* lista, std::string rutaSalida);
    void graficarMatriz(MatrizAsientos* matriz, std::string rutaSalida);
    void graficarListaListas(ListaDeListas* lista, std::string rutaSalida);
};

#endif // GENERADORDOT_H