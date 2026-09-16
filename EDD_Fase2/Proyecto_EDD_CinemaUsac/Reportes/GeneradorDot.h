#ifndef GENERADORDOT_H
#define GENERADORDOT_H

#include <string>
#include <fstream>
#include "../Estructuras/ArbolPeliculas.h"
#include "../Estructuras/ListaCircularDoble.h"
#include "../Estructuras/MatrizAsientos.h"
#include "../Estructuras/ListaDeListas.h"
#include "../Estructuras/ArbolAVL.h"
#include "../Estructuras/ArbolB.h"
#include "../Estructuras/TablaHash.h"

// Genera archivos DOT e imágenes PNG para visualizar las estructuras.
class GeneradorDot {
private:
    void generarNodos(NodoArbol* nodo, std::ofstream& archivo);
    void generarNodosAVL(NodoAVL* nodo, std::ofstream& archivo);
    void generarNodosB(NodoB* nodo, std::ofstream& archivo, int& contador);
    bool ejecutarGraphviz(const std::string& dot, const std::string& png);

public:
    // Firmas originales (Fase 1) — conservadas
    void graficarArbol(ArbolPeliculas* arbol, std::string rutaSalida);
    void graficarListaDoble(ListaCircularDoble* lista, std::string rutaSalida);
    void graficarMatriz(MatrizAsientos* matriz, std::string rutaSalida);
    void graficarListaListas(ListaDeListas* lista, std::string rutaSalida);

    // Nuevas firmas (Fase 2)
    void graficarAVL(ArbolAVL* arbol, std::string rutaSalida);
    void graficarArbolB(ArbolB* arbol, std::string rutaSalida);
    void graficarHash(TablaHash* hash, std::string rutaSalida);
};

#endif