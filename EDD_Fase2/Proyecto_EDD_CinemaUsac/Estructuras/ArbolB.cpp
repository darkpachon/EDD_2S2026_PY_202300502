#include "ArbolB.h"
using namespace std;

ArbolB::ArbolB() : raiz(nullptr) {}

ArbolB::~ArbolB() { destruirRec(raiz); }

void ArbolB::destruirRec(NodoB* nodo) {
    if (nodo == nullptr) return;
    for (int i = 0; i <= nodo->n; i++) destruirRec(nodo->hijos[i]);
    delete nodo;
}

// ---------------------------------------------------------------------------
//  Inserción
// ---------------------------------------------------------------------------

void ArbolB::dividirHijo(NodoB* padre, int i) {
    NodoB* hijo = padre->hijos[i];
    NodoB* nuevo = new NodoB(hijo->hoja);

    // t = 2. La clave del medio (índice 1) sube al padre.
    // El nuevo nodo toma la última clave (índice 2).
    nuevo->n = 1;
    nuevo->claves[0] = hijo->claves[2];

    if (!hijo->hoja) {
        nuevo->hijos[0] = hijo->hijos[2];
        nuevo->hijos[1] = hijo->hijos[3];
    }

    hijo->n = 1; // se queda con la clave 0

    // Hacer espacio en el padre
    for (int j = padre->n; j > i; j--)
        padre->claves[j] = padre->claves[j - 1];
    for (int j = padre->n + 1; j > i + 1; j--)
        padre->hijos[j] = padre->hijos[j - 1];

    padre->claves[i]     = hijo->claves[1];
    padre->hijos[i + 1]  = nuevo;
    padre->n++;
}

void ArbolB::insertarNoLleno(NodoB* nodo, Cliente c) {
    int i = nodo->n - 1;

    if (nodo->hoja) {
        while (i >= 0 && c.id < nodo->claves[i].id) {
            nodo->claves[i + 1] = nodo->claves[i];
            i--;
        }
        nodo->claves[i + 1] = c;
        nodo->n++;
    } else {
        while (i >= 0 && c.id < nodo->claves[i].id) i--;
        i++;

        if (nodo->hijos[i]->n == MAX_CLAVES_B) {
            dividirHijo(nodo, i);
            if (c.id > nodo->claves[i].id) i++;
        }
        insertarNoLleno(nodo->hijos[i], c);
    }
}

void ArbolB::insertar(Cliente c) {
    if (raiz == nullptr) {
        raiz = new NodoB(true);
        raiz->claves[0] = c;
        raiz->n = 1;
        return;
    }

    if (raiz->n == MAX_CLAVES_B) {
        NodoB* nuevaRaiz = new NodoB(false);
        nuevaRaiz->hijos[0] = raiz;
        dividirHijo(nuevaRaiz, 0);
        raiz = nuevaRaiz;
    }

    insertarNoLleno(raiz, c);
}

// ---------------------------------------------------------------------------
//  Búsqueda y recorrido
// ---------------------------------------------------------------------------

Cliente* ArbolB::buscarRec(NodoB* nodo, const string& id) {
    if (nodo == nullptr) return nullptr;

    int i = 0;
    while (i < nodo->n && id > nodo->claves[i].id) i++;
    if (i < nodo->n && nodo->claves[i].id == id) return &(nodo->claves[i]);
    if (nodo->hoja) return nullptr;
    return buscarRec(nodo->hijos[i], id);
}

Cliente* ArbolB::buscar(const string& id) {
    return buscarRec(raiz, id);
}

void ArbolB::recorrerRec(NodoB* nodo, vector<Cliente>& out) {
    if (nodo == nullptr) return;
    int i;
    for (i = 0; i < nodo->n; i++) {
        if (!nodo->hoja) recorrerRec(nodo->hijos[i], out);
        out.push_back(nodo->claves[i]);
    }
    if (!nodo->hoja) recorrerRec(nodo->hijos[i], out);
}

vector<Cliente> ArbolB::listar() {
    vector<Cliente> r;
    recorrerRec(raiz, r);
    return r;
}

// ---------------------------------------------------------------------------
//  Eliminación (garantiza que cada nodo visitado tenga >= t=2 claves)
// ---------------------------------------------------------------------------

int ArbolB::encontrarClave(NodoB* nodo, const string& id) {
    for (int i = 0; i < nodo->n; i++)
        if (nodo->claves[i].id == id) return i;
    return -1;
}

Cliente ArbolB::obtenerPredecesor(NodoB* nodo, int idx) {
    NodoB* actual = nodo->hijos[idx];
    while (!actual->hoja) actual = actual->hijos[actual->n];
    return actual->claves[actual->n - 1];
}

Cliente ArbolB::obtenerSucesor(NodoB* nodo, int idx) {
    NodoB* actual = nodo->hijos[idx + 1];
    while (!actual->hoja) actual = actual->hijos[0];
    return actual->claves[0];
}

void ArbolB::tomarDeAnterior(NodoB* nodo, int idx) {
    NodoB* hijo    = nodo->hijos[idx];
    NodoB* hermano = nodo->hijos[idx - 1];

    for (int i = hijo->n - 1; i >= 0; i--)
        hijo->claves[i + 1] = hijo->claves[i];
    if (!hijo->hoja)
        for (int i = hijo->n; i >= 0; i--)
            hijo->hijos[i + 1] = hijo->hijos[i];

    hijo->claves[0] = nodo->claves[idx - 1];
    nodo->claves[idx - 1] = hermano->claves[hermano->n - 1];

    if (!hijo->hoja)
        hijo->hijos[0] = hermano->hijos[hermano->n];

    hijo->n++;
    hermano->n--;
}

void ArbolB::tomarDeSiguiente(NodoB* nodo, int idx) {
    NodoB* hijo    = nodo->hijos[idx];
    NodoB* hermano = nodo->hijos[idx + 1];

    hijo->claves[hijo->n] = nodo->claves[idx];
    nodo->claves[idx] = hermano->claves[0];

    if (!hijo->hoja)
        hijo->hijos[hijo->n + 1] = hermano->hijos[0];

    for (int i = 1; i < hermano->n; i++)
        hermano->claves[i - 1] = hermano->claves[i];
    if (!hermano->hoja)
        for (int i = 1; i <= hermano->n; i++)
            hermano->hijos[i - 1] = hermano->hijos[i];

    hijo->n++;
    hermano->n--;
}

void ArbolB::fusionar(NodoB* nodo, int idx) {
    NodoB* hijo    = nodo->hijos[idx];
    NodoB* hermano = nodo->hijos[idx + 1];

    // La clave separadora del padre baja al hijo
    hijo->claves[hijo->n] = nodo->claves[idx];

    // Copiar claves del hermano
    for (int i = 0; i < hermano->n; i++)
        hijo->claves[hijo->n + 1 + i] = hermano->claves[i];

    // Copiar hijos del hermano
    if (!hijo->hoja)
        for (int i = 0; i <= hermano->n; i++)
            hijo->hijos[hijo->n + 1 + i] = hermano->hijos[i];

    hijo->n += hermano->n + 1;

    // Quitar clave e hijo del padre
    for (int i = idx + 1; i < nodo->n; i++)
        nodo->claves[i - 1] = nodo->claves[i];
    for (int i = idx + 2; i <= nodo->n; i++)
        nodo->hijos[i - 1] = nodo->hijos[i];
    nodo->n--;

    delete hermano;
}

void ArbolB::llenar(NodoB* nodo, int idx) {
    if (idx != 0 && nodo->hijos[idx - 1]->n >= MIN_CLAVES_B + 1)
        tomarDeAnterior(nodo, idx);
    else if (idx != nodo->n && nodo->hijos[idx + 1]->n >= MIN_CLAVES_B + 1)
        tomarDeSiguiente(nodo, idx);
    else if (idx != nodo->n)
        fusionar(nodo, idx);
    else
        fusionar(nodo, idx - 1);
}

void ArbolB::eliminarDeNoHoja(NodoB* nodo, int idx) {
    Cliente k = nodo->claves[idx];

    if (nodo->hijos[idx]->n >= MIN_CLAVES_B + 1) {
        Cliente pred = obtenerPredecesor(nodo, idx);
        nodo->claves[idx] = pred;
        eliminarDeNodo(nodo->hijos[idx], pred.id);
    } else if (nodo->hijos[idx + 1]->n >= MIN_CLAVES_B + 1) {
        Cliente suc = obtenerSucesor(nodo, idx);
        nodo->claves[idx] = suc;
        eliminarDeNodo(nodo->hijos[idx + 1], suc.id);
    } else {
        fusionar(nodo, idx);
        eliminarDeNodo(nodo->hijos[idx], k.id);
    }
}

void ArbolB::eliminarDeNodo(NodoB* nodo, const string& id) {
    int idx = encontrarClave(nodo, id);

    if (idx != -1) {
        if (nodo->hoja) {
            for (int i = idx + 1; i < nodo->n; i++)
                nodo->claves[i - 1] = nodo->claves[i];
            nodo->n--;
        } else {
            eliminarDeNoHoja(nodo, idx);
        }
        return;
    }

    if (nodo->hoja) return;

    int i = 0;
    while (i < nodo->n && nodo->claves[i].id < id) i++;
    bool ultimo = (i == nodo->n);

    if (nodo->hijos[i]->n < MIN_CLAVES_B + 1)
        llenar(nodo, i);

    if (ultimo && i > nodo->n)
        eliminarDeNodo(nodo->hijos[i - 1], id);
    else
        eliminarDeNodo(nodo->hijos[i], id);
}

bool ArbolB::eliminar(const string& id) {
    if (raiz == nullptr) return false;
    if (buscar(id) == nullptr) return false;

    eliminarDeNodo(raiz, id);

    // Reducir la altura si la raíz quedó sin claves
    if (raiz->n == 0) {
        NodoB* temp = raiz;
        if (raiz->hoja) raiz = nullptr;
        else            raiz = raiz->hijos[0];
        delete temp;
    }
    return true;
}