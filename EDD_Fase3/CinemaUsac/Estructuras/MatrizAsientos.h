#ifndef MATRIZASIENTOS_H
#define MATRIZASIENTOS_H

#include "NodoMatriz.h"
#include <string>
#include <vector>
#include <tuple>

class MatrizAsientos {
private:
    NodoMatriz* raiz;
    int filas;
    int columnas;
    std::string pelicula;
    std::string horario;
    std::string sala;
    std::string codigoFuncion;

    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearFila(int fila);
    NodoMatriz* crearColumna(int columna);

public:
    MatrizAsientos();
    ~MatrizAsientos();

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

    void setCodigoFuncion(const std::string& codigo) { codigoFuncion = codigo; }
    std::string getCodigoFuncion() const { return codigoFuncion; }

    void vaciar();

    std::vector<std::tuple<int, int, std::string>> obtenerReservas() const;
    bool buscarPorCodigoReserva(const std::string& codigoReserva, int& filaOut, int& colOut) const;
};

#endif