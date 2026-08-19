#include "GeneradorDot.h"
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <ctime>

namespace {
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
}

// Función principal que crea el archivo
void GeneradorDot::graficarArbol(ArbolPeliculas* arbol, std::string rutaSalida) {
    // Si el árbol está vacío, no hacemos nada
    if (arbol == nullptr || arbol->getRaiz() == nullptr) {
        return; 
    }

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/arbol_peliculas.dot");
    if (!archivo.is_open()) return;

    // Encabezado básico de Graphviz
    archivo << "digraph G {\n";
    archivo << "node [shape=box, style=\"filled,rounded\", fontname=\"Arial\"];\n";
    archivo << "edge [color=black];\n";

    // Llamamos a la función recursiva pasándole la raíz
    generarNodos(arbol->getRaiz(), archivo);

    archivo << "}\n";
    archivo.close();

    // Comando de consola para que Graphviz convierta el .dot a la imagen PNG
    std::string salida = "reportes_generados/" + rutaSalida;
    std::string comando = "dot -Tpng reportes_generados/arbol_peliculas.dot -o \"" + salida + "\"";
    if (system(comando.c_str()) != 0) std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

// Función recursiva para recorrer el árbol y dibujar las flechas
void GeneradorDot::generarNodos(NodoArbol* nodo, std::ofstream& archivo) {
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

    // Si tiene hijo izquierdo, dibujamos la flecha y aplicamos recursividad
    if (nodo->izquierdo != nullptr) {
        archivo << "Nodo" << nodo->pelicula.codigo << " -> Nodo" << nodo->izquierdo->pelicula.codigo << ";\n";
        generarNodos(nodo->izquierdo, archivo);
    }
    
    // Si tiene hijo derecho, dibujamos la flecha y aplicamos recursividad
    if (nodo->derecho != nullptr) {
        archivo << "Nodo" << nodo->pelicula.codigo << " -> Nodo" << nodo->derecho->pelicula.codigo << ";\n";
        generarNodos(nodo->derecho, archivo);
    }
}
// Agregar al final de GeneradorDot.cpp
void GeneradorDot::graficarListaDoble(ListaCircularDoble* lista, std::string rutaSalida) {
    if (lista == nullptr || lista->getPrimero() == nullptr) {
        return; 
    }

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/lista_solicitudes.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "rankdir=LR;\n"; // De izquierda a derecha
    archivo << "node [shape=ellipse, style=filled, fillcolor=lightyellow, fontname=\"Arial\"];\n";
    
    NodoListaDoble* actual = lista->getPrimero();
    int contador = 0; // Usaremos un contador para darle un ID único a cada nodo en Graphviz

    int solicitudesPendientes = 0;

    // Primera pasada: Crear los nodos
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

    // Segunda pasada: Crear las conexiones
    int totalNodos = contador;
    for (int i = 0; i < totalNodos; i++) {
        int siguiente = (i + 1) % totalNodos; // Enlaza el último con el primero mágicamente
        
        archivo << "Nodo" << i << " -> Nodo" << siguiente << " [dir=both, color=darkblue];\n";
    }

    archivo << "label=\"Solicitudes pendientes: " << solicitudesPendientes << "\";\n";
    archivo << "labelloc=t;\n";
    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    std::string comando = "dot -Tpng reportes_generados/lista_solicitudes.dot -o \"" + salida + "\"";
    if (system(comando.c_str()) != 0) std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}
// --- 1. REPORTE DE LA MATRIZ ORTOGONAL ---
void GeneradorDot::graficarMatriz(MatrizAsientos* matriz, std::string rutaSalida) {
    if (matriz == nullptr || matriz->getRaiz() == nullptr) return;

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/matriz_asientos.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "rankdir=TB;\n";
    archivo << "labelfontsize=18; label=\"Función: " << matriz->getPelicula()
            << " - " << matriz->getHorario() << " - " << matriz->getSala() << "\";\n";
    archivo << "node [fontname=\"Arial\"];\n";
    archivo << "Funcion [shape=diamond, style=filled, fillcolor=white, label=\"Funcion\"];\n";

    // Cabeceras superiores de las columnas.
    archivo << "{ rank=same; Funcion; ";
    for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
        archivo << "Columna" << columna << "; ";
    }
    archivo << "}\n";
    for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
        archivo << "Columna" << columna
                << " [shape=box, style=filled, fillcolor=\"#facc15\", label=\""
                << "C" << columna << "\"];\n";
        archivo << "Funcion -> Columna" << columna << " [color=gray, constraint=false];\n";
    }

    int reservados = 0;
    int totalAsientos = matriz->getFilas() * matriz->getColumnas();
    for (int fila = 1; fila <= matriz->getFilas(); fila++) {
        // Cada fila conserva su alineacion horizontal mediante rank=same.
        archivo << "{ rank=same; Fila" << fila << "; ";
        for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
            archivo << "Asiento_" << fila << "_" << columna << "; ";
        }
        archivo << "}\n";

        archivo << "Fila" << fila
                << " [shape=box, style=filled, fillcolor=\"#facc15\", label=\"F"
                << fila << "\"];\n";
        archivo << "Funcion -> Fila" << fila << " [color=gray, constraint=false];\n";

        for (int columna = 1; columna <= matriz->getColumnas(); columna++) {
            NodoMatriz* asiento = matriz->buscarAsiento(fila, columna);
            std::string id = "Asiento_" + std::to_string(fila) + "_" + std::to_string(columna);
            if (asiento != nullptr) {
                archivo << id << " [shape=circle, style=filled, fillcolor=\"#f87171\", label=\""
                        << asiento->cliente << "\"];\n";
                reservados++;
            } else {
                archivo << id << " [shape=box, style=dashed, color=gray, label=\"Libre\"];\n";
            }
            archivo << "Fila" << fila << " -> " << id << " [color=gray];\n";
            archivo << "Columna" << columna << " -> " << id << " [color=gray, constraint=false];\n";
        }
    }
    int libres = totalAsientos - reservados;
    archivo << "estadisticas [shape=note, style=filled, fillcolor=white, label=\"Asientos Libres: " << libres
            << "\\nAsientos Ocupados: " << reservados << "\\nTotal Asientos: " << totalAsientos << "\"];\n";
    archivo << "estadisticas -> Funcion [style=invis];\n";

    archivo << "}\n";
    archivo.close();

    std::string salida = "reportes_generados/" + rutaSalida;
    std::string comando = "dot -Tpng reportes_generados/matriz_asientos.dot -o \"" + salida + "\"";
    if (system(comando.c_str()) != 0) std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}

// --- 2. REPORTE DE LA LISTA DE LISTAS (PROMOCIONES) ---
void GeneradorDot::graficarListaListas(ListaDeListas* lista, std::string rutaSalida) {
    if (lista == nullptr || lista->getPrimero() == nullptr) return;

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/lista_promociones.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "rankdir=LR;\n"; // Principal de izquierda a derecha
    archivo << "node [shape=box, style=filled, fontname=\"Arial\"];\n";

    NodoListaListas* actualPromo = lista->getPrimero();

    archivo << "{ rank=same; ";
    NodoListaListas* promoParaAlinear = actualPromo;
    do {
        archivo << "Promo_" << promoParaAlinear->promocion.id << "; ";
        promoParaAlinear = promoParaAlinear->siguiente;
    } while (promoParaAlinear != actualPromo);
    archivo << "}\n";

    // Nodos principales: promociones en una lista circular horizontal.
    do {
        std::string idPromo = "Promo_" + std::to_string(actualPromo->promocion.id);
        archivo << idPromo << " [shape=box, style=\"filled,rounded\", fillcolor=\"#e0f2fe\", label=\"Código: "
            << actualPromo->promocion.id << "\\nNombre: " << actualPromo->promocion.descripcion
            << "\\nVigencia: " << actualPromo->promocion.fechaInicio << " a " << actualPromo->promocion.fechaFin
            << "\"];\n";

        // La ultima promocion tambien apunta a la primera para hacer visible la circularidad.
        std::string idSiguientePromo = "Promo_" + std::to_string(actualPromo->siguiente->promocion.id);
        archivo << idPromo << " -> " << idSiguientePromo
                << " [color=darkgreen, penwidth=2, constraint=false];\n";

        // Cada promocion despliega su sublista vertical de beneficios.
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
    std::string comando = "dot -Tpng reportes_generados/lista_promociones.dot -o \"" + salida + "\"";
    if (system(comando.c_str()) != 0) std::cerr << "No se pudo ejecutar Graphviz (dot).\n";
}