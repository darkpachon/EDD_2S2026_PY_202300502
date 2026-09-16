#ifndef NODOHASH_H
#define NODOHASH_H
#include "../Modelos/Reserva.h"

// Nodo de la lista enlazada usada para resolver colisiones.
class NodoHash {
public:
    Reserva reserva;
    NodoHash* siguiente;

    NodoHash(Reserva r) : reserva(r), siguiente(nullptr) {}
};

#endif