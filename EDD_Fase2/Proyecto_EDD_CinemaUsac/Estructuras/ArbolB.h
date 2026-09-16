#ifndef ARBOLB_H
#define ARBOLB_H
#include "NodoB.h"
#include <vector>
#include <string>

// Árbol B de orden 4 que administra los clientes del sistema. Clave: id.
class ArbolB {
private:
    NodoB* raiz;

    void dividirHijo(NodoB* padre, int i);
    void insertarNoLleno(NodoB* nodo, Cliente c);
    Cliente* buscarRec(NodoB* nodo, const std::string& id);
    void recorrerRec(NodoB* nodo, std::vector<Cliente>& out);
    void destruirRec(NodoB* nodo);

    // Eliminación
    int encontrarClave(NodoB* nodo, const std::string& id);
    void eliminarDeNodo(NodoB* nodo, const std::string& id);
    void eliminarDeNoHoja(NodoB* nodo, int idx);
    Cliente obtenerPredecesor(NodoB* nodo, int idx);
    Cliente obtenerSucesor(NodoB* nodo, int idx);
    void llenar(NodoB* nodo, int idx);
    void tomarDeAnterior(NodoB* nodo, int idx);
    void tomarDeSiguiente(NodoB* nodo, int idx);
    void fusionar(NodoB* nodo, int idx);

public:
    ArbolB();
    ~ArbolB();

    void insertar(Cliente c);
    bool eliminar(const std::string& id);
    Cliente* buscar(const std::string& id);
    std::vector<Cliente> listar();

    NodoB* getRaiz() { return raiz; }
    bool vacio() { return raiz == nullptr || raiz->n == 0; }
};

#endif