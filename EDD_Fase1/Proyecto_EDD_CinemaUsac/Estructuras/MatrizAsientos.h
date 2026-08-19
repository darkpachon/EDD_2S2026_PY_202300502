#ifndef MATRIZASIENTOS_H
#define MATRIZASIENTOS_H

#include "NodoMatriz.h"
#include <string>

class MatrizAsientos {
private:
    NodoMatriz* raiz;
    int filas;
    int columnas;
    std::string pelicula;
    std::string horario;
    std::string sala;

    NodoMatriz* buscarFila(int fila);
    NodoMatriz* buscarColumna(int columna);
    NodoMatriz* crearFila(int fila);
    NodoMatriz* crearColumna(int columna);

public:
    MatrizAsientos();
    void configurar(int nuevasFilas, int nuevasColumnas);
    void configurarFuncion(std::string peliculaFuncion, std::string horarioFuncion, std::string salaFuncion);
    bool insertarAsiento(int fila, int columna, std::string cliente);
    bool eliminarAsiento(int fila, int columna);
    NodoMatriz* buscarAsiento(int fila, int columna);
    int getFilas() const;
    int getColumnas() const;
    std::string getPelicula() const;
    std::string getHorario() const;
    std::string getSala() const;
    NodoMatriz* getRaiz(); // Útil para Graphviz
};

#endif // MATRIZASIENTOS_H