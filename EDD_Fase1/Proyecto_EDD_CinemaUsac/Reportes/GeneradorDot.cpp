#include "GeneradorDot.h"
#include <fstream>
#include <cstdlib>
#include <iostream>

// Función principal que crea el archivo
void GeneradorDot::graficarArbol(ArbolPeliculas* arbol, std::string rutaSalida) {
    // Si el árbol está vacío, no hacemos nada
    if (arbol == nullptr || arbol->getRaiz() == nullptr) {
        return; 
    }

    std::ofstream archivo("arbol_peliculas.dot");
    if (!archivo.is_open()) return;

    // Encabezado básico de Graphviz
    archivo << "digraph G {\n";
    archivo << "node [shape=record, style=filled, fillcolor=lightcyan, fontname=\"Arial\"];\n";
    archivo << "edge [color=darkblue];\n";

    // Llamamos a la función recursiva pasándole la raíz
    generarNodos(arbol->getRaiz(), archivo);

    archivo << "}\n";
    archivo.close();

    // Comando de consola para que Graphviz convierta el .dot a la imagen PNG
    std::string comando = "dot -Tpng arbol_peliculas.dot -o " + rutaSalida;
    system(comando.c_str());
}

// Función recursiva para recorrer el árbol y dibujar las flechas
void GeneradorDot::generarNodos(NodoArbol* nodo, std::ofstream& archivo) {
    if (nodo == nullptr) return;

    // Dibujamos el nodo actual (Ejemplo: Nodo34 [label="P034\nInception"])
    archivo << "Nodo" << nodo->pelicula.codigo 
            << " [label=\"" << nodo->pelicula.codigoOriginal << "\\n" 
            << nodo->pelicula.titulo << "\"];\n";

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

    std::ofstream archivo("lista_solicitudes.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "rankdir=LR;\n"; // De izquierda a derecha
    archivo << "node [shape=record, style=filled, fillcolor=lightyellow, fontname=\"Arial\"];\n";
    
    NodoListaDoble* actual = lista->getPrimero();
    int contador = 0; // Usaremos un contador para darle un ID único a cada nodo en Graphviz

    // Primera pasada: Crear los nodos
    do {
        archivo << "Nodo" << contador << " [label=\"{Cliente: " << actual->solicitud.cliente 
                << " | Tipo: " << actual->solicitud.tipo << "}\"];\n";
        actual = actual->siguiente;
        contador++;
    } while (actual != lista->getPrimero());

    // Segunda pasada: Crear las conexiones
    int totalNodos = contador;
    for (int i = 0; i < totalNodos; i++) {
        int siguiente = (i + 1) % totalNodos; // Enlaza el último con el primero mágicamente
        
        // Flecha de ida (siguiente)
        archivo << "Nodo" << i << " -> Nodo" << siguiente << " [color=blue];\n";
        // Flecha de vuelta (anterior)
        archivo << "Nodo" << siguiente << " -> Nodo" << i << " [color=red];\n";
    }

    archivo << "}\n";
    archivo.close();

    std::string comando = "dot -Tpng lista_solicitudes.dot -o " + rutaSalida;
    system(comando.c_str());
}
// --- 1. REPORTE DE LA MATRIZ ORTOGONAL ---
void GeneradorDot::graficarMatriz(MatrizAsientos* matriz, std::string rutaSalida) {
    if (matriz == nullptr || matriz->getRaiz() == nullptr) return;

    std::ofstream archivo("matriz_asientos.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "node [shape=box, style=filled, fillcolor=white, fontname=\"Arial\"];\n";
    archivo << "edge [dir=both];\n"; // Flechas en ambas direcciones

    NodoMatriz* actualFila = matriz->getRaiz();
    
    // Paso 1: Crear los nodos y alinearlos por filas (rank=same)
    while (actualFila != nullptr) {
        NodoMatriz* actualColumna = actualFila;
        
        archivo << "{ rank=same; ";
        while (actualColumna != nullptr) {
            std::string nombreNodo = "Nodo_" + std::to_string(actualColumna->fila) + "_" + std::to_string(actualColumna->columna);
            
            std::string label = "";
            if (actualColumna->fila == 0 && actualColumna->columna == 0) label = "Raiz";
            else if (actualColumna->fila == 0) label = "Col " + std::to_string(actualColumna->columna);
            else if (actualColumna->columna == 0) label = "Fila " + std::to_string(actualColumna->fila);
            else label = "Asiento\\n" + actualColumna->cliente;

            archivo << nombreNodo << " [label=\"" << label << "\"]; ";
            actualColumna = actualColumna->derecha;
        }
        archivo << "}\n";
        actualFila = actualFila->abajo;
    }

    // Paso 2: Conectar los nodos horizontal y verticalmente
    actualFila = matriz->getRaiz();
    while (actualFila != nullptr) {
        NodoMatriz* actualColumna = actualFila;
        while (actualColumna != nullptr) {
            std::string nodoActual = "Nodo_" + std::to_string(actualColumna->fila) + "_" + std::to_string(actualColumna->columna);
            
            // Conexión Horizontal
            if (actualColumna->derecha != nullptr) {
                std::string nodoDerecha = "Nodo_" + std::to_string(actualColumna->derecha->fila) + "_" + std::to_string(actualColumna->derecha->columna);
                archivo << nodoActual << " -> " << nodoDerecha << ";\n";
            }
            // Conexión Vertical
            if (actualColumna->abajo != nullptr) {
                std::string nodoAbajo = "Nodo_" + std::to_string(actualColumna->abajo->fila) + "_" + std::to_string(actualColumna->abajo->columna);
                archivo << nodoActual << " -> " << nodoAbajo << ";\n";
            }
            actualColumna = actualColumna->derecha;
        }
        actualFila = actualFila->abajo;
    }

    archivo << "}\n";
    archivo.close();

    std::string comando = "dot -Tpng matriz_asientos.dot -o " + rutaSalida;
    system(comando.c_str());
}

// --- 2. REPORTE DE LA LISTA DE LISTAS (PROMOCIONES) ---
void GeneradorDot::graficarListaListas(ListaDeListas* lista, std::string rutaSalida) {
    if (lista == nullptr || lista->getPrimero() == nullptr) return;

    std::ofstream archivo("lista_promociones.dot");
    if (!archivo.is_open()) return;

    archivo << "digraph G {\n";
    archivo << "rankdir=LR;\n"; // Principal de izquierda a derecha
    archivo << "node [shape=record, style=filled, fontname=\"Arial\"];\n";

    NodoListaListas* actualPromo = lista->getPrimero();
    
    // Recorremos la lista principal (Promociones)
    do {
        std::string idPromo = "Promo_" + std::to_string(actualPromo->promocion.id);
        archivo << idPromo << " [label=\"{" << actualPromo->promocion.descripcion 
                << " | Desc: " << actualPromo->promocion.porcentajeDescuento << "%}\", fillcolor=lightgreen];\n";

        // Conectar con la siguiente promoción
        std::string idSiguientePromo = "Promo_" + std::to_string(actualPromo->siguiente->promocion.id);
        archivo << idPromo << " -> " << idSiguientePromo << " [color=darkgreen, penwidth=2];\n";

        // Recorremos la sub-lista (Beneficios)
        NodoBeneficio* actualBen = actualPromo->listaBeneficios;
        int contBen = 0;
        
        if (actualBen != nullptr) {
            std::string nodoAnterior = idPromo;
            
            archivo << "{ rank=same; " << idPromo << "; "; // Alinear la promo con sus beneficios hacia abajo
            while (actualBen != nullptr) {
                std::string idBen = "Ben_" + std::to_string(actualPromo->promocion.id) + "_" + std::to_string(contBen);
                archivo << idBen << " [label=\"" << actualBen->beneficio << "\", fillcolor=lightcyan];\n";
                archivo << idBen << "; ";
                
                archivo << nodoAnterior << " -> " << idBen << " [color=blue];\n";
                nodoAnterior = idBen;
                
                actualBen = actualBen->siguiente;
                contBen++;
            }
            archivo << "}\n";
        }

        actualPromo = actualPromo->siguiente;
    } while (actualPromo != lista->getPrimero());

    archivo << "}\n";
    archivo.close();

    std::string comando = "dot -Tpng lista_promociones.dot -o " + rutaSalida;
    system(comando.c_str());
}