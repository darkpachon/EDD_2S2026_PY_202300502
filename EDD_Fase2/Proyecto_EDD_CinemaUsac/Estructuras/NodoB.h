#ifndef NODOB_H
#define NODOB_H
#include "../Modelos/Cliente.h"

// Parámetros del Árbol B de orden 4
const int ORDEN_B       = 4;
const int MAX_CLAVES_B  = ORDEN_B - 1;   // 3 claves máximo
const int MIN_CLAVES_B  = 1;              // 1 clave mínima (ceil(4/2)-1)

// Nodo del Árbol B. Puede almacenar hasta 3 clientes y 4 hijos.
class NodoB {
public:
    int n;                          // claves actuales (0..3)
    Cliente claves[MAX_CLAVES_B];   // clientes ordenados por id
    NodoB* hijos[ORDEN_B];          // 4 hijos
    bool hoja;

    NodoB(bool _hoja) : n(0), hoja(_hoja) {
        for (int i = 0; i < ORDEN_B; i++) hijos[i] = nullptr;
    }
};

#endif