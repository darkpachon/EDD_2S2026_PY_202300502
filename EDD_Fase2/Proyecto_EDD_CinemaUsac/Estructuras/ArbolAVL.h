#ifndef ARBOLAVL_H
#define ARBOLAVL_H
#include "NodoAVL.h"
#include <vector>
#include <string>

// Árbol AVL que administra las funciones del cine. Clave: codigo_funcion.
class ArbolAVL {
private:
    NodoAVL* raiz;

    int altura(NodoAVL* n);
    int balance(NodoAVL* n);
    void actualizarAltura(NodoAVL* n);
    NodoAVL* rotarDerecha(NodoAVL* y);
    NodoAVL* rotarIzquierda(NodoAVL* x);

    NodoAVL* insertarRec(NodoAVL* nodo, Funcion f);
    NodoAVL* eliminarRec(NodoAVL* nodo, const std::string& codigo);
    NodoAVL* minimo(NodoAVL* nodo);
    NodoAVL* buscarRec(NodoAVL* nodo, const std::string& codigo);

    void preordenRec(NodoAVL* nodo, std::vector<Funcion>& out);
    void inordenRec(NodoAVL* nodo, std::vector<Funcion>& out);
    void postordenRec(NodoAVL* nodo, std::vector<Funcion>& out);
    void destruir(NodoAVL* nodo);

public:
    ArbolAVL();
    ~ArbolAVL();

    void insertar(Funcion f);
    bool eliminar(const std::string& codigo);
    Funcion* buscar(const std::string& codigo);

    std::vector<Funcion> preorden();
    std::vector<Funcion> inorden();
    std::vector<Funcion> postorden();

    NodoAVL* getRaiz() { return raiz; }
    bool vacio() { return raiz == nullptr; }
};

#endif