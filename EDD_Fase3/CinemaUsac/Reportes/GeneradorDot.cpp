#include "GeneradorDot.h"
#include "../Estructuras/MerkleArbol.h"
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <memory>

namespace {
bool convertirFecha(const std::string& texto, std::tm& fecha) {
    if (texto.size() != 10 || texto[4] != '-' || texto[7] != '-') return false;
    try {
        fecha = {};
        fecha.tm_year = std::stoi(texto.substr(0, 4)) - 1900;
        fecha.tm_mon  = std::stoi(texto.substr(5, 2)) - 1;
        fecha.tm_mday = std::stoi(texto.substr(8, 2));
        fecha.tm_hour = 12;
        return true;
    } catch (...) { return false; }
}

int diasHasta(const std::string& texto) {
    std::tm fechaFinal{};
    if (!convertirFecha(texto, fechaFinal)) return -1;
    std::time_t ahoraTiempo = std::time(nullptr);
    std::tm ahora = *std::localtime(&ahoraTiempo);
    ahora.tm_hour = 12;
    std::time_t finalTiempo = std::mktime(&fechaFinal);
    std::time_t actualTiempo = std::mktime(&ahora);
    if (finalTiempo == (std::time_t)-1 || actualTiempo == (std::time_t)-1) return -1;
    return (int)(std::difftime(finalTiempo, actualTiempo) / (60 * 60 * 24));
}

std::string acortar(const std::string& s, size_t n = 12) {
    return s.size() <= n ? s : s.substr(0, n) + "...";
}
}

bool GeneradorDot::ejecutarGraphviz(const std::string& dot, const std::string& png) {
    std::string cmd = "dot -Tpng \"" + dot + "\" -o \"" + png + "\"";
    return std::system(cmd.c_str()) == 0;
}

// ============ Fase 1-2 ============

void GeneradorDot::graficarArbol(ArbolPeliculas* arbol, std::string rutaSalida) {
    if (arbol == nullptr || arbol->getRaiz() == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/arbol_peliculas.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph G {\n";
    archivo << "node [shape=box, style=\"filled,rounded\", fontname=\"Arial\"];\n";
    archivo << "labelloc=t; label=\"BST Películas\";\n";
    generarNodos(arbol->getRaiz(), archivo);
    archivo << "}\n";
    archivo.close();
    ejecutarGraphviz("reportes_generados/arbol_peliculas.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::generarNodos(NodoArbol* nodo, std::ofstream& archivo) {
    if (nodo == nullptr) return;
    int diasFin = diasHasta(nodo->pelicula.fecha_fin);
    int diasEstreno = diasHasta(nodo->pelicula.fecha_estreno);
    std::string color = "#d1d5db";
    if (diasEstreno <= 0 && diasFin >= 0 && diasFin < 7) color = "#fde047";
    else if (diasEstreno <= 0 && diasFin >= 7) color = "#86efac";

    archivo << "N" << nodo->pelicula.codigo
            << " [fillcolor=\"" << color << "\", label=\""
            << nodo->pelicula.codigoOriginal << "\\n" << nodo->pelicula.titulo
            << "\\n" << nodo->pelicula.duracion << " min\\n"
            << nodo->pelicula.clasificacion << "\"];\n";
    if (nodo->izquierdo) {
        archivo << "N" << nodo->pelicula.codigo << " -> N" << nodo->izquierdo->pelicula.codigo << ";\n";
        generarNodos(nodo->izquierdo, archivo);
    }
    if (nodo->derecho) {
        archivo << "N" << nodo->pelicula.codigo << " -> N" << nodo->derecho->pelicula.codigo << ";\n";
        generarNodos(nodo->derecho, archivo);
    }
}

void GeneradorDot::graficarListaDoble(ListaCircularDoble* lista, std::string rutaSalida) {
    if (lista == nullptr || lista->getPrimero() == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/lista_solicitudes.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph G {\nrankdir=LR;\nnode [shape=ellipse, style=filled, fillcolor=lightyellow];\n";
    NodoListaDoble* actual = lista->getPrimero();
    int i = 0;
    do {
        archivo << "N" << i << " [label=\"Sol: " << actual->solicitud.numero
                << "\\n" << actual->solicitud.cliente
                << "\\n" << actual->solicitud.estado << "\"];\n";
        actual = actual->siguiente; i++;
    } while (actual != lista->getPrimero());
    for (int k = 0; k < i; k++)
        archivo << "N" << k << " -> N" << ((k + 1) % i) << " [dir=both, color=darkblue];\n";
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/lista_solicitudes.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::graficarMatriz(MatrizAsientos* matriz, std::string rutaSalida) {
    if (matriz == nullptr || matriz->getRaiz() == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/matriz_asientos.dot");
    if (!archivo.is_open()) return;
    int filas = matriz->getFilas(), cols = matriz->getColumnas();
    archivo << "digraph G {\nrankdir=TB; splines=ortho;\n";
    archivo << "label=\"Función " << matriz->getCodigoFuncion() << " - " << matriz->getPelicula() << "\";\n";
    archivo << "node [fontname=\"Arial\", fixedsize=true, width=0.7, height=0.4];\n";
    int ocupados = 0;
    for (int f = 1; f <= filas; f++) {
        archivo << "{rank=same; ";
        for (int c = 1; c <= cols; c++) archivo << "C_" << f << "_" << c << "; ";
        archivo << "}\n";
        for (int c = 1; c <= cols; c++) {
            NodoMatriz* a = matriz->buscarAsiento(f, c);
            std::string id = "C_" + std::to_string(f) + "_" + std::to_string(c);
            if (a) {
                archivo << id << " [shape=circle, style=filled, fillcolor=\"#f87171\", label=\"" 
                        << a->codigoReserva << "\"];\n";
                ocupados++;
            } else {
                archivo << id << " [shape=circle, style=filled, fillcolor=\"#86efac\", label=\"L\"];\n";
            }
        }
    }
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/matriz_asientos.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::graficarListaListas(ListaDeListas* lista, std::string rutaSalida) {
    if (lista == nullptr || lista->getPrimero() == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/lista_promociones.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph G {\nrankdir=LR; node [shape=box, style=filled];\n";
    NodoListaListas* actual = lista->getPrimero();
    do {
        std::string id = "P_" + std::to_string(actual->promocion.id);
        archivo << id << " [fillcolor=\"#e0f2fe\", label=\"" << actual->promocion.id
                << "\\n" << actual->promocion.descripcion << "\"];\n";
        archivo << id << " -> P_" << actual->siguiente->promocion.id << " [color=darkgreen];\n";
        NodoBeneficio* b = actual->listaBeneficios;
        int k = 0;
        while (b) {
            std::string bid = "B_" + std::to_string(actual->promocion.id) + "_" + std::to_string(k++);
            archivo << bid << " [fillcolor=\"#fef9c3\", label=\"" << b->beneficio << "\\n" << b->valor << "\"];\n";
            archivo << id << " -> " << bid << ";\n";
            b = b->siguiente;
        }
        actual = actual->siguiente;
    } while (actual != lista->getPrimero());
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/lista_promociones.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::generarNodosAVL(NodoAVL* nodo, std::ofstream& archivo) {
    if (nodo == nullptr) return;
    std::time_t ahora = std::time(nullptr);
    std::tm tmActual = *std::localtime(&ahora);
    int hh = 0;
    if (nodo->funcion.horario.size() >= 2) {
        try { hh = std::stoi(nodo->funcion.horario.substr(0, 2)); } catch (...) {}
    }
    std::string color = (tmActual.tm_hour > hh) ? "#f87171" : "#86efac";
    archivo << "A" << nodo->funcion.codigoFuncion
            << " [shape=circle, style=filled, fillcolor=\"" << color << "\", label=\""
            << nodo->funcion.codigoFuncion << "\\n" << nodo->funcion.tituloPelicula
            << "\\n" << nodo->funcion.horario << "\\n" << nodo->funcion.sede << "\"];\n";
    if (nodo->izquierdo) {
        archivo << "A" << nodo->funcion.codigoFuncion << " -> A" << nodo->izquierdo->funcion.codigoFuncion << ";\n";
        generarNodosAVL(nodo->izquierdo, archivo);
    }
    if (nodo->derecho) {
        archivo << "A" << nodo->funcion.codigoFuncion << " -> A" << nodo->derecho->funcion.codigoFuncion << ";\n";
        generarNodosAVL(nodo->derecho, archivo);
    }
}

void GeneradorDot::graficarAVL(ArbolAVL* arbol, std::string rutaSalida) {
    if (arbol == nullptr || arbol->getRaiz() == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/arbol_avl.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph AVL {\nnode [shape=circle];\nlabelloc=t; label=\"AVL Funciones\";\n";
    generarNodosAVL(arbol->getRaiz(), archivo);
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/arbol_avl.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::generarNodosB(NodoB* nodo, std::ofstream& archivo, int& contador) {
    if (nodo == nullptr) return;
    int miId = contador++;
    std::string color = nodo->hoja ? "#a7f3d0" : "#fde68a";
    if (miId == 0) color = "#93c5fd";
    archivo << "B" << miId << " [shape=record, style=filled, fillcolor=\"" << color << "\", label=\"";
    if (nodo->n == 0) archivo << "vacío";
    else for (int i = 0; i < nodo->n; i++) {
        if (i > 0) archivo << "|";
        archivo << "{" << nodo->claves[i].id << "|" << nodo->claves[i].nombre << "}";
    }
    archivo << "\"];\n";
    for (int i = 0; i <= nodo->n; i++) {
        if (nodo->hijos[i]) {
            int hijoId = contador;
            generarNodosB(nodo->hijos[i], archivo, contador);
            archivo << "B" << miId << " -> B" << hijoId << ";\n";
        }
    }
}

void GeneradorDot::graficarArbolB(ArbolB* arbol, std::string rutaSalida) {
    if (arbol == nullptr || arbol->getRaiz() == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/arbol_b.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph BTree {\nlabelloc=t; label=\"Árbol B Clientes\";\n";
    int contador = 0;
    generarNodosB(arbol->getRaiz(), archivo, contador);
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/arbol_b.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::graficarHash(TablaHash* hash, std::string rutaSalida) {
    if (hash == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/tabla_hash.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph Hash {\nrankdir=LR; node [shape=record];\nlabelloc=t; label=\"Tabla Hash\";\n";
    NodoHash** tabla = hash->getTabla();
    for (int i = 0; i < TAM_HASH; i++) {
        std::string vacio = (tabla[i] == nullptr) ? "vacío" : "";
        std::string color = (tabla[i] == nullptr) ? "#e5e7eb" : "#bfdbfe";
        archivo << "H" << i << " [fillcolor=\"" << color << "\", style=filled, label=\"{" << i << "|";
        if (tabla[i] == nullptr) archivo << "vacío";
        else {
            NodoHash* a = tabla[i];
            bool first = true;
            while (a) {
                if (!first) archivo << "|";
                archivo << "{" << a->reserva.codigoReserva << "|" << a->reserva.idCliente << "}";
                first = false; a = a->siguiente;
            }
        }
        archivo << "}\"];\n";
    }
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/tabla_hash.dot", "reportes_generados/" + rutaSalida);
}

// ============ FASE 3: NUEVOS REPORTES ============

void GeneradorDot::graficarGrafoSedes(GrafoSedes* grafo, std::string rutaSalida) {
    if (grafo == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/grafo_sedes.dot");
    if (!archivo.is_open()) return;
    archivo << "graph Sedes {\n";
    archivo << "graph [rankdir=LR, overlap=false, splines=true];\n";
    archivo << "node [shape=circle, style=filled, fillcolor=\"#a8d8ea\", fontsize=9];\n";
    archivo << "labelloc=t; label=\"Grafo de Sedes (peso = películas en común)\";\n";
    for (int i = 0; i < grafo->cantidadSedes(); i++) {
        const Sede& s = grafo->getSede(i);
        archivo << "N" << i << " [label=\"" << s.codigo << "\\n" << s.nombre
                << "\\n" << s.direccion << "\\nSalas: " << s.cantidadSalas
                << "\\nTel: " << s.telefono << "\"];\n";
    }
    for (int i = 0; i < grafo->cantidadSedes(); i++) {
        for (const auto& a : grafo->vecinos(i)) {
            if (a.destino > i) {
                archivo << "N" << i << " -- N" << a.destino
                        << " [color=\"#e07b39\", penwidth=2, label=\"" << a.peso << "\"];\n";
            }
        }
    }
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/grafo_sedes.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::graficarListaAdyacencia(GrafoSedes* grafo, std::string rutaSalida) {
    if (grafo == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/lista_adyacencia.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph Ady {\nrankdir=LR; node [shape=box, fontsize=9];\n";
    archivo << "labelloc=t; label=\"Lista de Adyacencia\";\n";
    for (int i = 0; i < grafo->cantidadSedes(); i++) {
        const Sede& s = grafo->getSede(i);
        archivo << "H" << i << " [style=filled, fillcolor=\"#a8d8ea\", label=\""
                << s.codigo << " " << s.nombre << "\"];\n";
        int k = 0;
        for (const auto& a : grafo->vecinos(i)) {
            archivo << "A" << i << "_" << k << " [fillcolor=\"#ffe9a8\", style=filled, label=\""
                    << grafo->getSede(a.destino).codigo << " | peso " << a.peso << "\"];\n";
            archivo << "H" << i << " -> A" << i << "_" << k << ";\n";
            k++;
        }
        if (grafo->vecinos(i).empty())
            archivo << "H" << i << " -> V" << i << " [label=\"sin conexiones\"];\n";
    }
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/lista_adyacencia.dot", "reportes_generados/" + rutaSalida);
}

void GeneradorDot::graficarBlockchain(Blockchain* bc, std::string rutaSalida) {
    if (bc == nullptr) return;
    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/blockchain.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph BC {\nrankdir=LR; node [shape=record, fontsize=9];\n";
    archivo << "labelloc=t; label=\"Blockchain\";\n";
    for (size_t i = 0; i < bc->getBloques().size(); i++) {
        const Bloque& b = bc->getBloques()[i];
        std::string color = bc->getIntegridad()[i] ? "#b6f2b6" : "#f7b6b6";
        archivo << "B" << i << " [fillcolor=\"" << color << "\", style=filled, label=\""
                << "{ Bloque " << b.index
                << " | Nonce: " << b.nonce
                << " | Hash: " << acortar(b.hash)
                << " | Prev: " << acortar(b.previousHash) << " }\"];\n";
        if (i > 0) archivo << "B" << (i - 1) << " -> B" << i << ";\n";
    }
    if (bc->getBloques().empty())
        archivo << "V [label=\"Sin bloques\"];\n";
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/blockchain.dot", "reportes_generados/" + rutaSalida);
}

namespace {
void escribirMerkleRec(std::ofstream& f, const std::shared_ptr<NodoMerkle>& n,
                       const std::string& id, int& contador) {
    if (!n) return;
    f << "  " << id << " [label=\"" << acortar(n->hash, 14)
      << "\", shape=box, style=filled, fillcolor=\"#ffe9a8\", fontsize=9];\n";
    if (n->izq) {
        std::string hi = "m" + std::to_string(++contador);
        f << "  " << id << " -> " << hi << ";\n";
        escribirMerkleRec(f, n->izq, hi, contador);
    }
    if (n->der) {
        std::string hd = "m" + std::to_string(++contador);
        f << "  " << id << " -> " << hd << ";\n";
        escribirMerkleRec(f, n->der, hd, contador);
    }
}
}

void GeneradorDot::graficarMerkle(Blockchain* bc, int indiceBloque, std::string rutaSalida) {
    if (bc == nullptr || indiceBloque < 0 || indiceBloque >= (int)bc->getBloques().size()) return;
    const Bloque& b = bc->getBloques()[indiceBloque];
    std::vector<std::string> hojas;
    for (const auto& t : b.data) hojas.push_back(t.serializar(0));
    MerkleArbol arbol;
    arbol.construir(hojas);

    std::filesystem::create_directories("reportes_generados");
    std::ofstream archivo("reportes_generados/merkle.dot");
    if (!archivo.is_open()) return;
    archivo << "digraph Merkle {\n node [shape=box, style=filled, fillcolor=\"#ffe9a8\", fontsize=9];\n";
    archivo << "labelloc=t; label=\"Merkle del Bloque " << indiceBloque << "\";\n";
    int cont = 0;
    escribirMerkleRec(archivo, arbol.getRaiz(), "m0", cont);
    archivo << "}\n"; archivo.close();
    ejecutarGraphviz("reportes_generados/merkle.dot", "reportes_generados/" + rutaSalida);
}