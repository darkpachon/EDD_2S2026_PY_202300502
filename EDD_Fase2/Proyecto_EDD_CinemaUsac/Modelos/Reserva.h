#ifndef RESERVA_H
#define RESERVA_H
#include <string>
using namespace std;

// Reserva de un asiento. codigoReserva es la clave de la Tabla Hash.
class Reserva {
public:
    string codigoReserva;   // Ej: R001
    string codigoFuncion;   // Ej: F001  -- referencia a la función
    string idCliente;       // Ej: u001  -- referencia al cliente
    int fila;
    int columna;
    string fechaReserva;    // "2026-03-01"

    Reserva() : fila(0), columna(0) {}

    Reserva(string _codigoReserva, string _codigoFuncion, string _idCliente,
            int _fila, int _columna, string _fechaReserva)
        : codigoReserva(_codigoReserva), codigoFuncion(_codigoFuncion),
          idCliente(_idCliente), fila(_fila), columna(_columna),
          fechaReserva(_fechaReserva) {}
};

#endif