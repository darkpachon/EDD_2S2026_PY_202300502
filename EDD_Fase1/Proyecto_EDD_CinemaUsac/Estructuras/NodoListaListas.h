#ifndef NODOLISTALISTAS_H
#define NODOLISTALISTAS_H

#include "../Modelos/Promocion.h"
#include <string>

using namespace std;

// Nodo para la sub-lista (Beneficios)
class NodoBeneficio {
public:
    string beneficio;
    string tipo;
    string valor;
    NodoBeneficio* siguiente;
    NodoBeneficio* anterior;
    NodoBeneficio(string _beneficio, string _tipo = "", string _valor = "")
        : beneficio(_beneficio), tipo(_tipo), valor(_valor), siguiente(nullptr), anterior(nullptr) {}
};

// Nodo para la lista principal (Promociones)
class NodoListaListas {
public:
    Promocion promocion;
    NodoListaListas* siguiente;
    NodoBeneficio* listaBeneficios; 

    NodoListaListas(Promocion _promocion) {
        promocion = _promocion;
        siguiente = nullptr;
        listaBeneficios = nullptr;
    }
};

#endif // NODOLISTALISTAS_H