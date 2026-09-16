#include "GeneradorDot.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <queue>
#include <map>
#include <string>

namespace {
// Convierte una fecha AAAA-MM-DD a una estructura utilizable por la biblioteca de tiempo.
bool convertirFecha(const std::string& texto, std::tm& fecha) {
    if (texto.size() != 10 || texto[4] != '-' || texto[7] != '-') return false;

    try {
        fecha = {};
        fecha.tm_year = std::stoi(texto.substr(0, 4)) - 1900;
        fecha.tm_mon = std::stoi(texto.substr(5, 2)) - 1;
        fecha.tm_mday = std::stoi(texto.substr(8, 2));
        fecha.tm_hour = 12;
        return true;
    } catch (...) {
        return false;
    }
}

int diasHasta(const std::string& texto) {
    // Calcula los días restantes para clasificar el estado de una película.
    std::tm fechaFinal{};
    if (!convertirFecha(texto, fechaFinal)) return -1;

    std::time_t ahoraTiempo = std::time(nullptr);
    std::tm ahora = *std::localtime(&ahoraTiempo);
    ahora.tm_hour = 12;

    std::time_t finalTiempo = std::mktime(&fechaFinal);
    std::time_t actualTiempo = std::mktime(&ahora);
    if (finalTiempo == static_cast<std::time_t>(-1) || actualTiempo == static_cast<std::time_t>(-1)) return -1;
    return static_cast<int>(std::difftime(finalTiempo, actualTiempo) / (60 * 60 * 24));
}
} // namespace

// ============================================================================
//  Método auxiliar para ejecutar Graphviz
// ============================================================================
bool GeneradorDot::ejecutarGraphviz(const std::string& dot, const std::string& png) {
    std::string comando = "dot -Tpng \"" + dot + "\" -o \"" + png + "\"";
    return system(comando.c_str()) == 0;
}

// ============================================================================
//  REPORTE 1: Árbol Binario de Búsqueda de Películas (Fase 1, sin cambios)
// ============================================================================
void GeneradorDot::graficarArbol(ArbolPeliculas* arbol, std::string rutaSalida) {
    // Crea el DOT del árbol y solicita a Graphviz su conversión a PNG.
    if (arbol == nullptr || arbol->getRaiz() == nullptr) {
        return;
    }

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/arbol_peliculas.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "node [shape=box, style=\"filled,rounded\", fontname=\"Arial\"];\n";
    archivo << "edge [color=black];\n";
    archivo << "labelloc=t; label=\"Árbol Binario de Búsqueda - Cartelera\";\n";

    generarNodos(arbol->getRaiz(), archivo);

    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    if (!ejecutarGraphviz("reportes_generados/arbol_peliculas.dot", salida))
        std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

void GeneradorDot::generarNodos(NodoArbol* nodo, std::ofstream& archivo) {
    // Escribe recursivamente nodos, colores y enlaces del árbol.
    if (nodo == nullptr) return;

    int diasFin = diasHasta(nodo->pelicula.fecha_fin);
    int diasEstreno = diasHasta(nodo->pelicula.fecha_estreno);
    std::string color = "#d1d5db";
    if (diasEstreno <= 0 && diasFin >= 0 && diasFin < 7) {
        color = "#fde047";
    } else if (diasEstreno <= 0 && diasFin >= 7) {
        color = "#86efac";
    }

    archivo << "Nodo" << nodo->pelicula.codigo
            << " [style=\"filled,rounded\", fillcolor=\"" << color << "\", label=\""
            << nodo->pelicula.codigoOriginal << "\\n"
            << nodo->pelicula.titulo << "\\n"
            << nodo->pelicula.duracion << " min\\n"
            << nodo->pelicula.clasificacion << "\"];\n";

    if (nodo->izquierdo != nullptr) {
        archivo << "Nodo" << nodo->pelicula.codigo << " -> Nodo" << nodo->izquierdo->pelicula.codigo << ";\n";
        generarNodos(nodo->izquierdo, archivo);
    }

    if (nodo->derecho != nullptr) {
        archivo << "Nodo" << nodo->pelicula.codigo << " -> Nodo" << nodo->derecho->pelicula.codigo << ";\n";
        generarNodos(nodo->derecho, archivo);
    }
}

// ============================================================================
//  REPORTE de Lista Circular Doble (Solicitudes) - Fase 1, sin cambios
// ============================================================================
void GeneradorDot::graficarListaDoble(ListaCircularDoble* lista, std::string rutaSalida) {
    // Genera el reporte circular doble de solicitudes.
    if (lista == nullptr || lista->getPrimero() == nullptr) {
        return;
    }

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/lista_solicitudes.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "rankdir=LR;\n";
    archivo << "node [shape=ellipse, style=filled, fillcolor=lightyellow, fontname=\"Arial\"];\n";

    NodoListaDoble* actual = lista->getPrimero();
    int contador = 0;

    int solicitudesPendientes = 0;

    do {
        if (actual->solicitud.estado == "Pendiente") solicitudesPendientes++;
        archivo << "Nodo" << contador << " [shape=ellipse, label=\"Solicitud: " << actual->solicitud.numero
            << "\\nCliente: " << actual->solicitud.cliente
            << "\\nTipo: " << actual->solicitud.tipo
            << "\\nPrioridad: " << actual->solicitud.prioridad
            << "\\nEstado: " << actual->solicitud.estado << "\"];\n";
        actual = actual->siguiente;
        contador++;
    } while (actual != lista->getPrimero());

    int totalNodos = contador;
    for (int i = 0; i < totalNodos; i++) {
        int siguiente = (i + 1) % totalNodos;
        archivo << "Nodo" << i << " -> Nodo" << siguiente << " [dir=both, color=darkblue];\n";
    }

    archivo << "label=\"Solicitudes pendientes: " << solicitudesPendientes << "\";\n";
    archivo << "labelloc=t;\n";
    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    if (!ejecutarGraphviz("reportes_generados/lista_solicitudes.dot", salida))
        std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

// ============================================================================
//  REPORTE 5: Matriz Dispersa (Mapa de Asientos)
//  ACTUALIZADO Fase 2:
//   - Ahora usa codigoReserva en lugar de cliente
//   - Muestra el código de función en el título
// ============================================================================
void GeneradorDot::graficarMatriz(MatrizAsientos* matriz, std::string rutaSalida) {
    // Representa cabeceras, asientos libres, reservas y estadísticas.
    if (matriz == nullptr || matriz->getRaiz() == nullptr) return;

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/matriz_asientos.dot");
    if (!archivo.is_open()) return;

    std::string codFun = matriz->getCodigoFuncion();

    archivo << "digraph G {\n";
    archivo << "rankdir=TB;\n";
    archivo << "labelfontsize=18; label=\"Función " << codFun << ": " << matriz->getPelicula()
            << " - " << matriz->getHorario() << " - " << matriz->getSala() << "\";\n";
    archivo << "nodesep=0.45; ranksep=0.65; splines=ortho;\n";
    archivo << "node [fontname=\"Arial\"];\n";
    archivo << "Funcion [shape=diamond, style=filled, fillcolor=white, label=\"Funcion\"];\n";

    archivo << "{ rank=same; Funcion; ";
    for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
        archivo << "Columna" << columna << "; ";
    }
    archivo << "}\n";
    for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
        archivo << "Columna" << columna
                << " [shape=box, style=filled, fillcolor=\"#facc15\", label=\""
                << "C" << columna << "\"];\n";
        if (columna == 1) {
            archivo << "Funcion -> Columna" << columna << " [color=gray];\n";
        } else {
            archivo << "Columna" << (columna - 1) << " -> Columna" << columna << " [color=gray];\n";
        }
    }

    int reservados = 0;
    int totalAsientos = matriz->getFilas() * matriz->getColumnas();
    for (int fila = 1; fila <= matriz->getFilas(); fila++) {
        archivo << "{ rank=same; Fila" << fila << "; ";
        for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
            archivo << "Asiento_" << fila << "_" << columna << "; ";
        }
        archivo << "}\n";

        archivo << "Fila" << fila
                << " [shape=box, style=filled, fillcolor=\"#facc15\", label=\"F"
                << fila << "\"];\n";
        if (fila == 1) {
            archivo << "Funcion -> Fila" << fila << " [color=gray];\n";
        } else {
            archivo << "Fila" << (fila - 1) << " -> Fila" << fila << " [color=gray];\n";
        }

        for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
            NodoMatriz* asiento = matriz->buscarAsiento(fila, columna);
            std::string id = "Asiento_" + std::to_string(fila) + "_" + std::to_string(columna);
            if (asiento != nullptr) {
                // Fase 2: el nodo guarda codigoReserva, no el nombre del cliente
                archivo << id << " [shape=circle, style=filled, fillcolor=\"#f87171\", label=\""
                        << asiento->codigoReserva << "\"];\n";
                reservados++;
            } else {
                archivo << id << " [shape=box, style=dashed, color=gray, label=\"Libre\"];\n";
            }
            if (columna == 1) {
                archivo << "Fila" << fila << " -> " << id << " [color=gray];\n";
            } else {
                archivo << "Asiento_" << fila << "_" << (columna - 1) << " -> " << id << " [color=gray];\n";
            }

            if (fila == 1) {
                archivo << "Columna" << columna << " -> " << id << " [color=gray];\n";
            } else {
                archivo << "Asiento_" << (fila - 1) << "_" << columna << " -> " << id << " [color=gray, constraint=false];\n";
            }
        }
    }
    int libres = totalAsientos - reservados;
    archivo << "estadisticas [shape=note, style=filled, fillcolor=white, label=\"Asientos Libres: " << libres
            << "\\nAsientos Ocupados: " << reservados << "\\nTotal Asientos: " << totalAsientos << "\"];\n";
    archivo << "estadisticas -> Funcion [style=invis];\n";

    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    if (!ejecutarGraphviz("reportes_generados/matriz_asientos.dot", salida))
        std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

// ============================================================================
//  REPORTE de Lista de Listas (Promociones) - Fase 1, sin cambios
// ============================================================================
void GeneradorDot::graficarListaListas(ListaDeListas* lista, std::string rutaSalida) {
    // Representa promociones en ciclo y sus beneficios enlazados.
    if (lista == nullptr || lista->getPrimero() == nullptr) return;

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/lista_promociones.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "rankdir=LR;\n";
    archivo << "node [shape=box, style=filled, fontname=\"Arial\"];\n";

    NodoListaListas* actualPromo = lista->getPrimero();

    archivo << "{ rank=same; ";
    NodoListaListas* promoParaAlinear = actualPromo;
    do {
        archivo << "Promo_" << promoParaAlinear->promocion.id << "; ";
        promoParaAlinear = promoParaAlinear->siguiente;
    } while (promoParaAlinear != actualPromo);
    archivo << "}\n";

    do {
        std::string idPromo = "Promo_" + std::to_string(actualPromo->promocion.id);
        archivo << idPromo << " [shape=box, style=\"filled,rounded\", fillcolor=\"#e0f2fe\", label=\"Código: "
            << actualPromo->promocion.id << "\\nNombre: " << actualPromo->promocion.descripcion
            << "\\nVigencia: " << actualPromo->promocion.fechaInicio << " a " << actualPromo->promocion.fechaFin
            << "\"];\n";

        std::string idSiguientePromo = "Promo_" + std::to_string(actualPromo->siguiente->promocion.id);
        archivo << idPromo << " -> " << idSiguientePromo
                << " [color=darkgreen, penwidth=2, constraint=false];\n";

        NodoBeneficio* actualBen = actualPromo->listaBeneficios;
        int contBen = 0;
        std::string nodoAnterior = idPromo;
        while (actualBen != nullptr) {
            std::string idBen = "Ben_" + std::to_string(actualPromo->promocion.id) + "_" + std::to_string(contBen);
            archivo << idBen << " [shape=box, style=filled, fillcolor=\"#fef9c3\", label=\"Tipo: "
                    << actualBen->tipo << "\\nDescripción: " << actualBen->beneficio
                    << "\\nValor: " << actualBen->valor << "\"];\n";
            archivo << nodoAnterior << " -> " << idBen << " [color=darkblue];\n";
            if (nodoAnterior != idPromo) {
                archivo << nodoAnterior << " -> " << idBen << " [dir=back, color=darkblue];\n";
            }
            nodoAnterior = idBen;
            actualBen = actualBen->siguiente;
            contBen++;
        }

        std::string idNull = "Null_" + std::to_string(actualPromo->promocion.id);
        archivo << idNull << " [shape=box, style=filled, fillcolor=\"#fef9c3\", label=\"null\"];\n";
        archivo << nodoAnterior << " -> " << idNull << " [dir=both, color=darkblue];\n";

        actualPromo = actualPromo->siguiente;
    } while (actualPromo != lista->getPrimero());

    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    if (!ejecutarGraphviz("reportes_generados/lista_promociones.dot", salida))
        std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

// ============================================================================
//  REPORTE 2: Árbol AVL de Funciones (Fase 2 - NUEVO)
//  - Nodos circulares
//  - Verde: función programada (horario futuro)
//  - Rojo:  función ya realizada
// ============================================================================
void GeneradorDot::generarNodosAVL(NodoAVL* nodo, std::ofstream& archivo) {
    if (nodo == nullptr) return;

    // Determinar color según horario (comparando con la hora actual)
    std::time_t ahora = std::time(nullptr);
    std::tm tmActual = *std::localtime(&ahora);

    std::string horario = nodo->funcion.horario;
    int hh = 0, mm = 0;
    if (horario.size() == 5 && horario[2] == ':') {
        try {
            hh = std::stoi(horario.substr(0, 2));
            mm = std::stoi(horario.substr(3, 2));
        } catch (...) {}
    }
    bool yaPaso = (tmActual.tm_hour > hh) || (tmActual.tm_hour == hh && tmActual.tm_min > mm);
    std::string color = yaPaso ? "#f87171" : "#86efac";

    archivo << "NodoAVL" << nodo->funcion.codigoFuncion
            << " [shape=circle, style=filled, fillcolor=\"" << color << "\", label=\""
            << nodo->funcion.codigoFuncion << "\\n"
            << nodo->funcion.tituloPelicula << "\\n"
            << nodo->funcion.horario << "\\n"
            << nodo->funcion.sala << "\"];\n";

    if (nodo->izquierdo != nullptr) {
        archivo << "NodoAVL" << nodo->funcion.codigoFuncion
                << " -> NodoAVL" << nodo->izquierdo->funcion.codigoFuncion << ";\n";
        generarNodosAVL(nodo->izquierdo, archivo);
    }
    if (nodo->derecho != nullptr) {
        archivo << "NodoAVL" << nodo->funcion.codigoFuncion
                << " -> NodoAVL" << nodo->derecho->funcion.codigoFuncion << ";\n";
        generarNodosAVL(nodo->derecho, archivo);
    }
}

void GeneradorDot::graficarAVL(ArbolAVL* arbol, std::string rutaSalida) {
    if (arbol == nullptr || arbol->getRaiz() == nullptr) return;

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/arbol_avl.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph AVL {\n";
    archivo << "node [shape=circle, fontname=\"Arial\"];\n";
    archivo << "edge [color=black];\n";
    archivo << "labelloc=t; label=\"Árbol AVL de Funciones\";\n";
    generarNodosAVL(arbol->getRaiz(), archivo);
    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    if (!ejecutarGraphviz("reportes_generados/arbol_avl.dot", salida))
        std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

// ============================================================================
//  REPORTE 3: Árbol B de orden 4 de Clientes (Fase 2 - NUEVO)
//  - Cada nodo es una caja con compartimentos (hasta 3 claves)
//  - Colores distintos: raíz / interno / hoja
// ============================================================================
void GeneradorDot::generarNodosB(NodoB* nodo, std::ofstream& archivo, int& contador) {
    if (nodo == nullptr) return;

    int miId = contador++;
    std::string color = nodo->hoja ? "#a7f3d0" : "#fde68a";
    if (miId == 0) color = "#93c5fd"; // raíz

    // Construir la etiqueta con compartimentos (record shape)
    archivo << "NodoB" << miId << " [shape=record, style=filled, fillcolor=\"" << color
            << "\", label=\"";
    if (nodo->n == 0) {
        archivo << "vacío";
    } else {
        for (int i = 0; i < nodo->n; i++) {
            if (i > 0) archivo << "|";
            archivo << "{" << nodo->claves[i].id << "|" << nodo->claves[i].nombre << "}";
        }
    }
    archivo << "\"];\n";

    for (int i = 0; i <= nodo->n; i++) {
        if (nodo->hijos[i] != nullptr) {
            int hijoId = contador; // el hijo tomará este ID al ser procesado
            generarNodosB(nodo->hijos[i], archivo, contador);
            archivo << "NodoB" << miId << " -> NodoB" << hijoId << ";\n";
        }
    }
}

void GeneradorDot::graficarArbolB(ArbolB* arbol, std::string rutaSalida) {
    if (arbol == nullptr || arbol->getRaiz() == nullptr) return;

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/arbol_b.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph BTree {\n";
    archivo << "node [fontname=\"Arial\"];\n";
    archivo << "labelloc=t; label=\"Árbol B de orden 4 - Clientes\";\n";

    int contador = 0;
    generarNodosB(arbol->getRaiz(), archivo, contador);

    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    if (!ejecutarGraphviz("reportes_generados/arbol_b.dot", salida))
        std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

// ============================================================================
//  REPORTE 4: Tabla Hash de Reservas (Fase 2 - NUEVO)
//  - Buckets numerados (cajas)
//  - Colisiones representadas en la misma etiqueta record
//  - Estadísticas al final
// ============================================================================
void GeneradorDot::graficarHash(TablaHash* hash, std::string rutaSalida) {
    if (hash == nullptr) return;

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/tabla_hash.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph Hash {\n";
    archivo << "rankdir=LR;\n";
    archivo << "node [fontname=\"Arial\", shape=record];\n";
    archivo << "labelloc=t; label=\"Tabla Hash de Reservas\";\n";

    NodoHash** tabla = hash->getTabla();

    for (int i = 0; i < TAM_HASH; i++) {
        std::string bucketId = "Bucket" + std::to_string(i);
        bool vacio = (tabla[i] == nullptr);
        std::string color = vacio ? "#e5e7eb" : "#bfdbfe";

        archivo << bucketId << " [label=\"{" << i << "|";
        if (vacio) {
            archivo << "vacío";
        } else {
            NodoHash* actual = tabla[i];
            bool primero = true;
            while (actual != nullptr) {
                if (!primero) archivo << "|";
                archivo << "{" << actual->reserva.codigoReserva
                        << "|" << actual->reserva.idCliente
                        << "|" << actual->reserva.codigoFuncion << "}";
                primero = false;
                actual = actual->siguiente;
            }
        }
        archivo << "}\", style=filled, fillcolor=\"" << color << "\"];\n";
    }

    // Resaltar colisiones
    for (int i = 0; i < TAM_HASH; i++) {
        if (tabla[i] != nullptr && tabla[i]->siguiente != nullptr) {
            std::string idCol = "Colision" + std::to_string(i);
            archivo << idCol << " [shape=note, style=filled, fillcolor=\"#fca5a5\", label=\"Colisión\"];\n";
            archivo << "Bucket" << i << " -> " << idCol << " [style=dashed, color=red];\n";
        }
    }

    // Estadísticas
    archivo << "Stats [shape=note, style=filled, fillcolor=white, label=\"Tamaño: "
            << TAM_HASH << "\\nReservas: " << hash->getTotal()
            << "\\nBuckets ocupados: " << hash->getBucketsOcupados()
            << "\\nColisiones: " << hash->getColisiones() << "\"];\n";

    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    if (!ejecutarGraphviz("reportes_generados/tabla_hash.dot", salida))
        std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}