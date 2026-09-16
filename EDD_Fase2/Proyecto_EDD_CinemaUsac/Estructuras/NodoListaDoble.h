#ifndef NODOLISTADOBLE_H
#define NODOLISTADOBLE_H

#include "../Modelos/Solicitud.h"

// Nodo de la lista circular doblemente enlazada de solicitudes.
class NodoListaDoble {
public:
    Solicitud solicitud;
    NodoListaDoble* siguiente;
    NodoListaDoble* anterior;
    // Guarda una solicitud y deja sus enlaces sin conectar inicialmente.
    NodoListaDoble(Solicitud _solicitud) {
        solicitud = _solicitud;
        siguiente = nullptr;
        anterior = nullptr;
    }
};

#endif