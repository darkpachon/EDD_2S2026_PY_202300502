#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <string>
#include <vector>
#include "../Estructuras/ArbolPeliculas.h"
#include "../Estructuras/ArbolAVL.h"
#include "../Estructuras/ArbolB.h"
#include "../Estructuras/TablaHash.h"
#include "../Estructuras/MatrizAsientos.h"
#include "../Estructuras/GrafoSedes.h"

class Persistencia {
public:
    // ---------- Carga masiva (JSON) ----------
    static bool cargarPeliculasYFunciones(
        const std::string& rutaArchivo,
        ArbolPeliculas* arbol,
        ArbolAVL* avl,
        GrafoSedes* grafo,
        std::vector<std::string>& errores);

    static bool cargarClientesYReservas(
        const std::string& rutaArchivo,
        ArbolB* arbolB,
        TablaHash* hash,
        ArbolAVL* avlFunciones,
        std::vector<std::string>& errores);

    static bool cargarPeliculasYFuncionesDesdeTexto(
        const std::string& contenido,
        ArbolPeliculas* arbol,
        ArbolAVL* avl,
        GrafoSedes* grafo,
        std::vector<std::string>& errores);

    static bool cargarClientesYReservasDesdeTexto(
        const std::string& contenido,
        ArbolB* arbolB,
        TablaHash* hash,
        ArbolAVL* avlFunciones,
        std::vector<std::string>& errores);

    // ---------- Persistencia por función ----------
    static bool cargarAsientosFuncion(const std::string& ruta, MatrizAsientos* matriz);
    static bool guardarAsientosFuncion(const std::string& ruta, MatrizAsientos* matriz);
    static bool crearArchivoAsientosVacio(const std::string& ruta, int filas, int columnas);

    // ---------- Guardado legacy (JSON) ----------
    static bool guardarCartelera(const std::string& ruta, ArbolPeliculas* arbol, ArbolAVL* avl);
    static bool guardarClientes(const std::string& ruta, ArbolB* arbolB, TablaHash* hash);
    static bool guardarReservas(const std::string& ruta, TablaHash* hash);

    // ---------- Fase 3: compresión LZW a .edd ----------
    static bool guardarSedesEDD(const std::string& ruta, GrafoSedes* grafo);
    static bool guardarPeliculasEDD(const std::string& ruta, ArbolPeliculas* arbol);
    static bool guardarFuncionesEDD(const std::string& ruta, ArbolAVL* avl);

    static bool cargarSedesEDD(const std::string& ruta, GrafoSedes* grafo);
    static bool cargarPeliculasEDD(const std::string& ruta, ArbolPeliculas* arbol);
    static bool cargarFuncionesEDD(const std::string& ruta, ArbolAVL* avl);

    // ---------- Utilidades ----------
    static std::string siguienteCodigoFuncion(ArbolAVL* avl);
    static std::string siguienteCodigoReserva(TablaHash* hash);
    static std::string rutaAsientosFuncion(const std::string& codigoFuncion);
    static std::string rutaCompletaAsientos(const std::string& archivoAsientos);
    static std::string formatearCodigo(const std::string& prefijo, int numero, int ancho = 3);
};

#endif