#ifndef NODOHASH_H
#define NODOHASH_H
#include "../Modelos/Reserva.h"

class NodoHash {
public:
    Reserva reserva;
    NodoHash* siguiente;

    NodoHash(Reserva r) : reserva(r), siguiente(nullptr) {}
};

#endif