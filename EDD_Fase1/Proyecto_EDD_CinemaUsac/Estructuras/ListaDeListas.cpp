#include "ListaDeListas.h"

ListaDeListas::ListaDeListas() {
    primero = nullptr;
    ultimo = nullptr;
}

void ListaDeListas::insertarPromocion(Promocion nuevaPromo) {
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
    if (primero == nullptr) return;
    
    NodoListaListas* actual = primero;
    do {
        if (actual->promocion.id == idPromocion) {
            NodoBeneficio* nuevoBen = new NodoBeneficio(beneficio);
            
            // Inserción al inicio de la sub-lista para ser más eficientes
            nuevoBen->siguiente = actual->listaBeneficios;
            actual->listaBeneficios = nuevoBen;
            return;
        }
        actual = actual->siguiente;
    } while (actual != primero);
}

NodoListaListas* ListaDeListas::getPrimero() {
    return primero;
}