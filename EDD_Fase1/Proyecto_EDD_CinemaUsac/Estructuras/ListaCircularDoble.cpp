#include "ListaCircularDoble.h"

ListaCircularDoble::ListaCircularDoble() {
    primero = nullptr;
    ultimo = nullptr;
}

void ListaCircularDoble::insertar(Solicitud nuevaSolicitud) {
    NodoListaDoble* nuevoNodo = new NodoListaDoble(nuevaSolicitud);

    if (primero == nullptr) {
        primero = nuevoNodo;
        ultimo = nuevoNodo;
        primero->siguiente = primero;
        primero->anterior = primero;
    } else {
        ultimo->siguiente = nuevoNodo;
        nuevoNodo->anterior = ultimo;
        nuevoNodo->siguiente = primero;
        primero->anterior = nuevoNodo;
        ultimo = nuevoNodo;
    }
}

NodoListaDoble* ListaCircularDoble::getPrimero() {
    return primero;
}