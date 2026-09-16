#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <string>
#include <vector>
#include "../Estructuras/ArbolPeliculas.h"
#include "../Estructuras/ArbolAVL.h"
#include "../Estructuras/ArbolB.h"
#include "../Estructuras/TablaHash.h"
#include "../Estructuras/MatrizAsientos.h"

// Capa de persistencia JSON: lectura y escritura de todas las estructuras.
class Persistencia {
public:
    // ---------- Carga masiva ----------
    // Carga películas + funciones. Crea los F00X_funcion.json vacíos.
    static bool cargarPeliculasYFunciones(
        const std::string& rutaArchivo,
        ArbolPeliculas* arbol,
        ArbolAVL* avl,
        std::vector<std::string>& errores);

    // Carga clientes + reservas. Inserta en Árbol B, Tabla Hash y actualiza los
    // archivos de asientos de cada función afectada.
    static bool cargarClientesYReservas(
        const std::string& rutaArchivo,
        ArbolB* arbolB,
        TablaHash* hash,
        ArbolAVL* avlFunciones,
        std::vector<std::string>& errores);

    // ---------- Persistencia de una función concreta ----------
    static bool cargarAsientosFuncion(const std::string& ruta, MatrizAsientos* matriz);
    static bool guardarAsientosFuncion(const std::string& ruta, MatrizAsientos* matriz);
    static bool crearArchivoAsientosVacio(const std::string& ruta, int filas, int columnas);

    // ---------- Guardado masivo (opcional) ----------
    static bool guardarCartelera(const std::string& ruta, ArbolPeliculas* arbol, ArbolAVL* avl);
    static bool guardarClientes(const std::string& ruta, ArbolB* arbolB);
    static bool guardarReservas(const std::string& ruta, TablaHash* hash);

    // ---------- Utilidades ----------
    static std::string siguienteCodigoFuncion(ArbolAVL* avl);
    static std::string siguienteCodigoReserva(TablaHash* hash);
    static std::string rutaAsientosFuncion(const std::string& codigoFuncion);
    static std::string formatearCodigo(const std::string& prefijo, int numero, int ancho = 3);
};

#endif