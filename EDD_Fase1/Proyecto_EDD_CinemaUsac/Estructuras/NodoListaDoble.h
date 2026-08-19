#ifndef NODOLISTADOBLE_H
#define NODOLISTADOBLE_H

#include "../Modelos/Solicitud.h"

class NodoListaDoble {
public:
    Solicitud solicitud;
    NodoListaDoble* siguiente;
    NodoListaDoble* anterior;

    NodoListaDoble(Solicitud _solicitud) {
        solicitud = _solicitud;
        siguiente = nullptr;
        anterior = nullptr;
    }
};

#endif // NODOLISTADOBLE_H