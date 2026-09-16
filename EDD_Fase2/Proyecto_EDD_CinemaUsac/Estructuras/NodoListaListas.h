#ifndef NODOLISTALISTAS_H
#define NODOLISTALISTAS_H

#include "../Modelos/Promocion.h"
#include <string>

using namespace std;

 
class NodoBeneficio {
public:
    string beneficio;
    string tipo;
    string valor;
    NodoBeneficio* siguiente;
    NodoBeneficio* anterior;
    // Crea un beneficio y deja sus enlaces sin conectar.
    NodoBeneficio(string _beneficio, string _tipo = "", string _valor = "")
        : beneficio(_beneficio), tipo(_tipo), valor(_valor), siguiente(nullptr), anterior(nullptr) {}
};

 
class NodoListaListas {
public:
    Promocion promocion;
    NodoListaListas* siguiente;
    NodoBeneficio* listaBeneficios; 

    // Guarda una promoción y comienza sin beneficios.
    NodoListaListas(Promocion _promocion) {
        promocion = _promocion;
        siguiente = nullptr;
        listaBeneficios = nullptr;
    }
};

#endif