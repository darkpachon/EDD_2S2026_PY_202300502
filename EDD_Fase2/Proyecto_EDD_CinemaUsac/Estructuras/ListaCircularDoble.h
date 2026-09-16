#ifndef LISTACIRCULARDOBLE_H
#define LISTACIRCULARDOBLE_H

#include "NodoListaDoble.h"

// Lista circular doblemente enlazada para las solicitudes especiales.
class ListaCircularDoble {
private:
    NodoListaDoble* primero; // Inicio de la lista.
    NodoListaDoble* ultimo; // Último nodo conectado nuevamente al inicio.

public:
    ListaCircularDoble();
    void insertar(Solicitud nuevaSolicitud); // Agrega al final.
    bool eliminarPorNumero(int numero); // Elimina por identificador.
    int contar(); // Cuenta las solicitudes.
    NodoListaDoble* getPrimero(); // Inicio para recorrer la lista.
};

#endif