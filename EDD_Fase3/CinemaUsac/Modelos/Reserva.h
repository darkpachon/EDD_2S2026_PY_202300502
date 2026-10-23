#ifndef RESERVA_H
#define RESERVA_H
#include <string>
using namespace std;

class Reserva {
public:
    string codigoReserva;
    string codigoFuncion;
    string idCliente;
    int fila;
    int columna;
    string fechaReserva;

    Reserva() : fila(0), columna(0) {}

    Reserva(string _codigoReserva, string _codigoFuncion, string _idCliente,
            int _fila, int _columna, string _fechaReserva)
        : codigoReserva(_codigoReserva), codigoFuncion(_codigoFuncion),
          idCliente(_idCliente), fila(_fila), columna(_columna),
          fechaReserva(_fechaReserva) {}
};

#endif