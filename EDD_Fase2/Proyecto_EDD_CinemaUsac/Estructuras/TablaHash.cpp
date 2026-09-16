#include "TablaHash.h"

TablaHash::TablaHash() : total(0) {
    for (int i = 0; i < TAM_HASH; i++) tabla[i] = nullptr;
}

TablaHash::~TablaHash() {
    for (int i = 0; i < TAM_HASH; i++) {
        NodoHash* actual = tabla[i];
        while (actual != nullptr) {
            NodoHash* sig = actual->siguiente;
            delete actual;
            actual = sig;
        }
    }
}

// Hash djb2 sobre el codigo_reserva
int TablaHash::funcionHash(const std::string& clave) {
    unsigned long hash = 5381;
    for (char c : clave)
        hash = ((hash << 5) + hash) + static_cast<unsigned char>(c);
    return static_cast<int>(hash % TAM_HASH);
}

void TablaHash::insertar(Reserva r) {
    int idx = funcionHash(r.codigoReserva);

    // Evitar duplicados
    NodoHash* actual = tabla[idx];
    while (actual != nullptr) {
        if (actual->reserva.codigoReserva == r.codigoReserva) return;
        actual = actual->siguiente;
    }

    NodoHash* nuevo = new NodoHash(r);
    nuevo->siguiente = tabla[idx];
    tabla[idx] = nuevo;
    total++;
}

bool TablaHash::eliminar(const std::string& codigoReserva) {
    int idx = funcionHash(codigoReserva);
    NodoHash* actual = tabla[idx];
    NodoHash* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->reserva.codigoReserva == codigoReserva) {
            if (anterior == nullptr) tabla[idx] = actual->siguiente;
            else                     anterior->siguiente = actual->siguiente;
            delete actual;
            total--;
            return true;
        }
        anterior = actual;
        actual = actual->siguiente;
    }
    return false;
}

Reserva* TablaHash::buscar(const std::string& codigoReserva) {
    int idx = funcionHash(codigoReserva);
    NodoHash* actual = tabla[idx];
    while (actual != nullptr) {
        if (actual->reserva.codigoReserva == codigoReserva) return &(actual->reserva);
        actual = actual->siguiente;
    }
    return nullptr;
}

std::vector<Reserva> TablaHash::listar() {
    std::vector<Reserva> r;
    for (int i = 0; i < TAM_HASH; i++) {
        NodoHash* actual = tabla[i];
        while (actual != nullptr) {
            r.push_back(actual->reserva);
            actual = actual->siguiente;
        }
    }
    return r;
}

std::vector<Reserva> TablaHash::listarPorFuncion(const std::string& codigoFuncion) {
    std::vector<Reserva> r;
    for (int i = 0; i < TAM_HASH; i++) {
        NodoHash* actual = tabla[i];
        while (actual != nullptr) {
            if (actual->reserva.codigoFuncion == codigoFuncion)
                r.push_back(actual->reserva);
            actual = actual->siguiente;
        }
    }
    return r;
}

std::vector<Reserva> TablaHash::listarPorCliente(const std::vector<std::string>& codigos) {
    std::vector<Reserva> r;
    for (const auto& c : codigos) {
        Reserva* res = buscar(c);
        if (res != nullptr) r.push_back(*res);
    }
    return r;
}

int TablaHash::getColisiones() const {
    int colisiones = 0;
    for (int i = 0; i < TAM_HASH; i++) {
        int count = 0;
        NodoHash* actual = tabla[i];
        while (actual != nullptr) { count++; actual = actual->siguiente; }
        if (count > 1) colisiones += count - 1;
    }
    return colisiones;
}

int TablaHash::getBucketsOcupados() const {
    int ocupados = 0;
    for (int i = 0; i < TAM_HASH; i++)
        if (tabla[i] != nullptr) ocupados++;
    return ocupados;
}