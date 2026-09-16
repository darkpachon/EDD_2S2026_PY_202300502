#include "ListaDeListas.h"

ListaDeListas::ListaDeListas() {
    // La lista circular inicia sin promociones.
    primero = nullptr;
    ultimo = nullptr;
}

void ListaDeListas::insertarPromocion(Promocion nuevaPromo) {
    // El nuevo nodo se conecta al final y apunta nuevamente al primero.
    NodoListaListas* nuevo = new NodoListaListas(nuevaPromo);
    
    if (primero == nullptr) {
        primero = nuevo;
        ultimo = nuevo;
        primero->siguiente = primero; 
    } else {
        ultimo->siguiente = nuevo;
        nuevo->siguiente = primero;
        ultimo = nuevo;
    }
}

void ListaDeListas::insertarBeneficio(int idPromocion, string beneficio) {
    insertarBeneficio(idPromocion, "", beneficio, "");
}

void ListaDeListas::insertarBeneficio(int idPromocion, string tipo, string beneficio, string valor) {
    // Se recorre el ciclo hasta localizar la promoción indicada.
    if (primero == nullptr) return;
    
    NodoListaListas* actual = primero;
    do {
        if (actual->promocion.id == idPromocion) {
            NodoBeneficio* nuevoBen = new NodoBeneficio(beneficio, tipo, valor);
            
            // Los beneficios se enlazan en una sublista doblemente enlazada.
            if (actual->listaBeneficios == nullptr) {
                actual->listaBeneficios = nuevoBen;
            } else {
                NodoBeneficio* ultimoBeneficio = actual->listaBeneficios;
                while (ultimoBeneficio->siguiente != nullptr) ultimoBeneficio = ultimoBeneficio->siguiente;
                ultimoBeneficio->siguiente = nuevoBen;
                nuevoBen->anterior = ultimoBeneficio;
            }
            return;
        }
        actual = actual->siguiente;
    } while (actual != primero);
}

bool ListaDeListas::eliminarPromocion(int idPromocion) {
    // Además de la promoción, se liberan todos sus beneficios asociados.
    if (primero == nullptr) return false;

    NodoListaListas* actual = primero;
    do {
        if (actual->promocion.id == idPromocion) {
            if (actual == primero && actual == ultimo) {
                primero = nullptr;
                ultimo = nullptr;
            } else {
                NodoListaListas* anterior = ultimo;
                while (anterior->siguiente != actual) anterior = anterior->siguiente;
                anterior->siguiente = actual->siguiente;
                if (actual == primero) primero = actual->siguiente;
                if (actual == ultimo) ultimo = anterior;
            }
            NodoBeneficio* beneficio = actual->listaBeneficios;
            while (beneficio != nullptr) {
                NodoBeneficio* siguiente = beneficio->siguiente;
                delete beneficio;
                beneficio = siguiente;
            }
            delete actual;
            return true;
        }
        actual = actual->siguiente;
    } while (actual != primero);
    return false;
}

NodoListaListas* ListaDeListas::getPrimero() {
    return primero;
}