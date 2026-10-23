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
#include "../Estructuras/GrafoSedes.h"
#include "../Estructuras/Blockchain.h"

class GeneradorDot {
private:
    void generarNodos(NodoArbol* nodo, std::ofstream& archivo);
    void generarNodosAVL(NodoAVL* nodo, std::ofstream& archivo);
    void generarNodosB(NodoB* nodo, std::ofstream& archivo, int& contador);
    bool ejecutarGraphviz(const std::string& dot, const std::string& png);

public:
    // Fase 1-2
    void graficarArbol(ArbolPeliculas* arbol, std::string rutaSalida);
    void graficarListaDoble(ListaCircularDoble* lista, std::string rutaSalida);
    void graficarMatriz(MatrizAsientos* matriz, std::string rutaSalida);
    void graficarListaListas(ListaDeListas* lista, std::string rutaSalida);
    void graficarAVL(ArbolAVL* arbol, std::string rutaSalida);
    void graficarArbolB(ArbolB* arbol, std::string rutaSalida);
    void graficarHash(TablaHash* hash, std::string rutaSalida);

    // Fase 3
    void graficarGrafoSedes(GrafoSedes* grafo, std::string rutaSalida);
    void graficarListaAdyacencia(GrafoSedes* grafo, std::string rutaSalida);
    void graficarBlockchain(Blockchain* bc, std::string rutaSalida);
    void graficarMerkle(Blockchain* bc, int indiceBloque, std::string rutaSalida);
};

#endif