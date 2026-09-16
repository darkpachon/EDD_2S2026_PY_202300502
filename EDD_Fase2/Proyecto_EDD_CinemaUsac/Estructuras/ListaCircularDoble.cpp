#include "ListaCircularDoble.h"

ListaCircularDoble::ListaCircularDoble() {
    // La lista comienza vacía.
    primero = nullptr;
    ultimo = nullptr;
}

void ListaCircularDoble::insertar(Solicitud nuevaSolicitud) {
    // Se enlaza el nuevo nodo manteniendo los vínculos circular y doble.
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

bool ListaCircularDoble::eliminarPorNumero(int numero) {
    if (primero == nullptr) return false;

    NodoListaDoble* actual = primero;
    do {
        if (actual->solicitud.numero == numero) {
            // Se desconecta el nodo y se actualizan los extremos si es necesario.
            if (actual == primero && actual == ultimo) {
                primero = nullptr;
                ultimo = nullptr;
            } else {
                actual->anterior->siguiente = actual->siguiente;
                actual->siguiente->anterior = actual->anterior;
                if (actual == primero) primero = actual->siguiente;
                if (actual == ultimo) ultimo = actual->anterior;
            }
            delete actual;
            return true;
        }
        actual = actual->siguiente;
    } while (actual != primero);
    return false;
}

int ListaCircularDoble::contar() {
    // El recorrido finaliza cuando vuelve al primer nodo.
    if (primero == nullptr) return 0;
    int total = 0;
    NodoListaDoble* actual = primero;
    do {
        total++;
        actual = actual->siguiente;
    } while (actual != primero);
    return total;
}

NodoListaDoble* ListaCircularDoble::getPrimero() {
    return primero;
}