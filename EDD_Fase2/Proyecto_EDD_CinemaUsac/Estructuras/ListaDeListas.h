#ifndef LISTADELISTAS_H
#define LISTADELISTAS_H

#include "NodoListaListas.h"
#include <string>

// Lista circular de promociones; cada promoción posee una sublista de beneficios.
class ListaDeListas {
private:
    NodoListaListas* primero;
    NodoListaListas* ultimo;

public:
    ListaDeListas();
    void insertarPromocion(Promocion nuevaPromo); // Agrega una promoción al ciclo.
    void insertarBeneficio(int idPromocion, std::string beneficio); // Agrega beneficio básico.
    void insertarBeneficio(int idPromocion, std::string tipo, std::string beneficio, std::string valor); // Agrega beneficio completo.
    bool eliminarPromocion(int idPromocion); // Elimina promoción y sus beneficios.
    NodoListaListas* getPrimero(); // Devuelve el inicio para recorrer el ciclo.
};

#endif