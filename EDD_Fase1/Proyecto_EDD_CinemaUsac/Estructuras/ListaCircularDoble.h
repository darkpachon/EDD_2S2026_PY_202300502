#ifndef LISTACIRCULARDOBLE_H
#define LISTACIRCULARDOBLE_H

#include "NodoListaDoble.h"

class ListaCircularDoble {
private:
    NodoListaDoble* primero;
    NodoListaDoble* ultimo;

public:
    ListaCircularDoble();
    void insertar(Solicitud nuevaSolicitud);
    bool eliminarPorNumero(int numero);
    int contar();
    NodoListaDoble* getPrimero();
};

#endif // LISTACIRCULARDOBLE_H