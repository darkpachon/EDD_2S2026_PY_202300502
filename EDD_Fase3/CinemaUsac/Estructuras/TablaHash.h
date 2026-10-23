#ifndef TABLAHASH_H
#define TABLAHASH_H
#include "NodoHash.h"
#include <vector>
#include <string>

const int TAM_HASH = 53;

class TablaHash {
private:
    NodoHash* tabla[TAM_HASH];
    int total;

    int funcionHash(const std::string& clave);

public:
    TablaHash();
    ~TablaHash();

    void insertar(Reserva r);
    bool eliminar(const std::string& codigoReserva);
    Reserva* buscar(const std::string& codigoReserva);

    std::vector<Reserva> listar();
    std::vector<Reserva> listarPorFuncion(const std::string& codigoFuncion);
    std::vector<Reserva> listarPorCliente(const std::vector<std::string>& codigos);

    // Fase 3: vacía toda la tabla (para reconstruir desde la blockchain al arrancar)
    void limpiar();

    int getTotal()           const { return total; }
    int getColisiones()      const;
    int getBucketsOcupados() const;
    NodoHash** getTabla()          { return tabla; }
};

#endif