#include "ArbolAVL.h"
#include <algorithm>
using namespace std;

ArbolAVL::ArbolAVL() : raiz(nullptr) {}

ArbolAVL::~ArbolAVL() { destruir(raiz); }

void ArbolAVL::destruir(NodoAVL* nodo) {
    if (nodo == nullptr) return;
    destruir(nodo->izquierdo);
    destruir(nodo->derecho);
    delete nodo;
}

int ArbolAVL::altura(NodoAVL* n) {
    return (n == nullptr) ? 0 : n->altura;
}

int ArbolAVL::balance(NodoAVL* n) {
    return (n == nullptr) ? 0 : altura(n->izquierdo) - altura(n->derecho);
}

void ArbolAVL::actualizarAltura(NodoAVL* n) {
    if (n != nullptr) n->altura = 1 + max(altura(n->izquierdo), altura(n->derecho));
}

NodoAVL* ArbolAVL::rotarDerecha(NodoAVL* y) {
    NodoAVL* x = y->izquierdo;
    NodoAVL* T2 = x->derecho;
    x->derecho = y;
    y->izquierdo = T2;
    actualizarAltura(y);
    actualizarAltura(x);
    return x;
}

NodoAVL* ArbolAVL::rotarIzquierda(NodoAVL* x) {
    NodoAVL* y = x->derecho;
    NodoAVL* T2 = y->izquierdo;
    y->izquierdo = x;
    x->derecho = T2;
    actualizarAltura(x);
    actualizarAltura(y);
    return y;
}

NodoAVL* ArbolAVL::insertarRec(NodoAVL* nodo, Funcion f) {
    if (nodo == nullptr) return new NodoAVL(f);

    if (f.codigoFuncion < nodo->funcion.codigoFuncion)
        nodo->izquierdo = insertarRec(nodo->izquierdo, f);
    else if (f.codigoFuncion > nodo->funcion.codigoFuncion)
        nodo->derecho = insertarRec(nodo->derecho, f);
    else
        return nodo; // Duplicado: no se inserta

    actualizarAltura(nodo);
    int b = balance(nodo);

    // Izquierda-Izquierda
    if (b > 1 && f.codigoFuncion < nodo->izquierdo->funcion.codigoFuncion)
        return rotarDerecha(nodo);
    // Derecha-Derecha
    if (b < -1 && f.codigoFuncion > nodo->derecho->funcion.codigoFuncion)
        return rotarIzquierda(nodo);
    // Izquierda-Derecha
    if (b > 1 && f.codigoFuncion > nodo->izquierdo->funcion.codigoFuncion) {
        nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
        return rotarDerecha(nodo);
    }
    // Derecha-Izquierda
    if (b < -1 && f.codigoFuncion < nodo->derecho->funcion.codigoFuncion) {
        nodo->derecho = rotarDerecha(nodo->derecho);
        return rotarIzquierda(nodo);
    }
    return nodo;
}

void ArbolAVL::insertar(Funcion f) {
    raiz = insertarRec(raiz, f);
}

NodoAVL* ArbolAVL::minimo(NodoAVL* nodo) {
    while (nodo->izquierdo != nullptr) nodo = nodo->izquierdo;
    return nodo;
}

NodoAVL* ArbolAVL::eliminarRec(NodoAVL* nodo, const string& codigo) {
    if (nodo == nullptr) return nullptr;

    if (codigo < nodo->funcion.codigoFuncion)
        nodo->izquierdo = eliminarRec(nodo->izquierdo, codigo);
    else if (codigo > nodo->funcion.codigoFuncion)
        nodo->derecho = eliminarRec(nodo->derecho, codigo);
    else {
        // Encontrado: resolver según hijos
        if (nodo->izquierdo == nullptr || nodo->derecho == nullptr) {
            NodoAVL* temp = nodo->izquierdo ? nodo->izquierdo : nodo->derecho;
            if (temp == nullptr) {
                temp = nodo;
                nodo = nullptr;
            } else {
                *nodo = *temp;
            }
            delete temp;
        } else {
            NodoAVL* sucesor = minimo(nodo->derecho);
            nodo->funcion = sucesor->funcion;
            nodo->derecho = eliminarRec(nodo->derecho, sucesor->funcion.codigoFuncion);
        }
    }

    if (nodo == nullptr) return nullptr;

    actualizarAltura(nodo);
    int b = balance(nodo);

    if (b > 1 && balance(nodo->izquierdo) >= 0) return rotarDerecha(nodo);
    if (b > 1 && balance(nodo->izquierdo) < 0) {
        nodo->izquierdo = rotarIzquierda(nodo->izquierdo);
        return rotarDerecha(nodo);
    }
    if (b < -1 && balance(nodo->derecho) <= 0) return rotarIzquierda(nodo);
    if (b < -1 && balance(nodo->derecho) > 0) {
        nodo->derecho = rotarDerecha(nodo->derecho);
        return rotarIzquierda(nodo);
    }
    return nodo;
}

bool ArbolAVL::eliminar(const string& codigo) {
    if (buscar(codigo) == nullptr) return false;
    raiz = eliminarRec(raiz, codigo);
    return true;
}

NodoAVL* ArbolAVL::buscarRec(NodoAVL* nodo, const string& codigo) {
    if (nodo == nullptr || nodo->funcion.codigoFuncion == codigo) return nodo;
    if (codigo < nodo->funcion.codigoFuncion) return buscarRec(nodo->izquierdo, codigo);
    return buscarRec(nodo->derecho, codigo);
}

Funcion* ArbolAVL::buscar(const string& codigo) {
    NodoAVL* n = buscarRec(raiz, codigo);
    return n ? &(n->funcion) : nullptr;
}

void ArbolAVL::preordenRec(NodoAVL* n, vector<Funcion>& out) {
    if (n == nullptr) return;
    out.push_back(n->funcion);
    preordenRec(n->izquierdo, out);
    preordenRec(n->derecho, out);
}
void ArbolAVL::inordenRec(NodoAVL* n, vector<Funcion>& out) {
    if (n == nullptr) return;
    inordenRec(n->izquierdo, out);
    out.push_back(n->funcion);
    inordenRec(n->derecho, out);
}
void ArbolAVL::postordenRec(NodoAVL* n, vector<Funcion>& out) {
    if (n == nullptr) return;
    postordenRec(n->izquierdo, out);
    postordenRec(n->derecho, out);
    out.push_back(n->funcion);
}

vector<Funcion> ArbolAVL::preorden()  { vector<Funcion> r; preordenRec(raiz, r);  return r; }
vector<Funcion> ArbolAVL::inorden()   { vector<Funcion> r; inordenRec(raiz, r);   return r; }
vector<Funcion> ArbolAVL::postorden() { vector<Funcion> r; postordenRec(raiz, r); return r; }