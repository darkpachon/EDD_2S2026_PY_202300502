#ifndef LISTADELISTAS_H
#define LISTADELISTAS_H

#include "NodoListaListas.h"
#include <string>

class ListaDeListas {
private:
    NodoListaListas* primero;
    NodoListaListas* ultimo;

public:
    ListaDeListas();
    void insertarPromocion(Promocion nuevaPromo);
    void insertarBeneficio(int idPromocion, std::string beneficio);
    void insertarBeneficio(int idPromocion, std::string tipo, std::string beneficio, std::string valor);
    bool eliminarPromocion(int idPromocion);
    NodoListaListas* getPrimero();
};

#endif