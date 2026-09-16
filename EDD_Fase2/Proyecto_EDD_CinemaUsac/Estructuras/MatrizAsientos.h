#ifndef MATRIZASIENTOS_H
#define MATRIZASIENTOS_H

#include "NodoMatriz.h"
#include <string>
#include <vector>
#include <tuple>

// Matriz dispersa que almacena únicamente los asientos reservados.
// En Fase 2 representa SIEMPRE la función activa en memoria.
class MatrizAsientos {
private:
    NodoMatriz* raiz;
    int filas;
    int columnas;
    std::string pelicula;      // se conserva para el reporte
    std::string horario;       // se conserva para el reporte
    std::string sala;          // se conserva para el reporte
    std::string codigoFuncion; // NUEVO: función actualmente cargada (ej. F001)

    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearFila(int fila);
    NodoMatriz* crearColumna(int columna);

public:
    MatrizAsientos();
    ~MatrizAsientos();

    // ----- Firmas originales conservadas -----
    void configurar(int nuevasFilas, int nuevasColumnas);
    void configurarFuncion(std::string peliculaFuncion, std::string horarioFuncion, std::string salaFuncion);
    void eliminarFuncion();
    bool insertarAsiento(int fila, int columna, std::string codigoReserva);
    bool eliminarAsiento(int fila, int columna);
    NodoMatriz* buscarAsiento(int fila, int columna);
    int getFilas() const;
    int getColumnas() const;
    std::string getPelicula() const;
    std::string getHorario() const;
    std::string getSala() const;
    NodoMatriz* getRaiz();

    // ----- Nuevos métodos para Fase 2 -----
    void setCodigoFuncion(const std::string& codigo) { codigoFuncion = codigo; }
    std::string getCodigoFuncion() const { return codigoFuncion; }

    // Vacía la matriz sin destruir el objeto (para cambiar de función activa).
    void vaciar();

    // Recorre la matriz y devuelve las reservas (fila, columna, codigoReserva).
    std::vector<std::tuple<int, int, std::string>> obtenerReservas() const;

    // Búsqueda por codigo_reserva (para cancelación desde el cliente).
    bool buscarPorCodigoReserva(const std::string& codigoReserva, int& filaOut, int& colOut) const;
};

#endif