#ifndef NODOB_H
#define NODOB_H
#include "../Modelos/Cliente.h"

const int ORDEN_B       = 4;
const int MAX_CLAVES_B  = ORDEN_B - 1;
const int MIN_CLAVES_B  = 1;

class NodoB {
public:
    int n;
    Cliente claves[MAX_CLAVES_B];
    NodoB* hijos[ORDEN_B];
    bool hoja;

    NodoB(bool _hoja) : n(0), hoja(_hoja) {
        for (int i = 0; i < ORDEN_B; i++) hijos[i] = nullptr;
    }
};

#endif