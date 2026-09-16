#ifndef NODOAVL_H
#define NODOAVL_H
#include "../Modelos/Funcion.h"

// Nodo del Árbol AVL ordenado por codigo_funcion.
class NodoAVL {
public:
    Funcion funcion;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;

    NodoAVL(Funcion f)
        : funcion(f), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};

#endif