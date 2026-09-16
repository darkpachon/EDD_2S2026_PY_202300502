#include "Persistencia.h"
#include "../Modelos/Json.h"
#include "../Utilidades/Validaciones.h"
#include <fstream>
#include <set>
#include <iostream>

using namespace std;

// ============================ Utilidades ============================

string Persistencia::formatearCodigo(const string& prefijo, int numero, int ancho) {
    string s = to_string(numero);
    while (static_cast<int>(s.size()) < ancho) s = "0" + s;
    return prefijo + s;
}

string Persistencia::rutaAsientosFuncion(const string& codigoFuncion) {
    return codigoFuncion + "_funcion.json";
}

string Persistencia::siguienteCodigoFuncion(ArbolAVL* avl) {
    int maxNum = 0;
    if (avl != nullptr) {
        for (const auto& f : avl->inorden()) {
            if (f.codigoFuncion.size() > 1 && (f.codigoFuncion[0] == 'F' || f.codigoFuncion[0] == 'f')) {
                try {
                    int n = stoi(f.codigoFuncion.substr(1));
                    if (n > maxNum) maxNum = n;
                } catch (...) {}
            }
        }
    }
    return formatearCodigo("F", maxNum + 1);
}

string Persistencia::siguienteCodigoReserva(TablaHash* hash) {
    int maxNum = 0;
    if (hash != nullptr) {
        for (const auto& r : hash->listar()) {
            if (r.codigoReserva.size() > 1 && (r.codigoReserva[0] == 'R' || r.codigoReserva[0] == 'r')) {
                try {
                    int n = stoi(r.codigoReserva.substr(1));
                    if (n > maxNum) maxNum = n;
                } catch (...) {}
            }
        }
    }
    return formatearCodigo("R", maxNum + 1);
}

// ============================ Archivo de asientos ============================

bool Persistencia::crearArchivoAsientosVacio(const string& ruta, int filas, int columnas) {
    JsonValue raiz;
    raiz["codigo_funcion"] = string("");
    raiz["filas"] = filas;
    raiz["columnas"] = columnas;
    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;
    raiz["asientos_ocupados"] = arr;
    return raiz.guardarArchivo(ruta, 2);
}

bool Persistencia::cargarAsientosFuncion(const string& ruta, MatrizAsientos* matriz) {
    if (matriz == nullptr) return false;
    try {
        JsonValue root = JsonValue::parsearArchivo(ruta);
        int filas = root.getEntero("filas", 0);
        int cols  = root.getEntero("columnas", 0);
        if (filas <= 0 || cols <= 0) return false;

        matriz->vaciar();
        matriz->configurar(filas, cols);

        const JsonValue& arr = root["asientos_ocupados"];
        if (arr.esArray()) {
            for (size_t i = 0; i < arr.vArray.size(); i++) {
                const JsonValue& a = arr.vArray[i];
                int f = a.getEntero("fila", -1);
                int c = a.getEntero("columna", -1);
                string codRes = a.getTexto("codigo_reserva", "");
                if (f > 0 && c > 0 && !codRes.empty())
                    matriz->insertarAsiento(f, c, codRes);
            }
        }
        return true;
    } catch (const std::exception& e) {
        cerr << "Error cargando asientos: " << e.what() << endl;
        return false;
    }
}

bool Persistencia::guardarAsientosFuncion(const string& ruta, MatrizAsientos* matriz) {
    if (matriz == nullptr) return false;
    JsonValue root;
    root["codigo_funcion"] = matriz->getCodigoFuncion();
    root["filas"] = matriz->getFilas();
    root["columnas"] = matriz->getColumnas();

    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;
    for (const auto& t : matriz->obtenerReservas()) {
        JsonValue a;
        a["codigo_reserva"] = std::get<2>(t);
        a["fila"] = std::get<0>(t);
        a["columna"] = std::get<1>(t);
        arr.agregar(a);
    }
    root["asientos_ocupados"] = arr;
    return root.guardarArchivo(ruta, 2);
}

// ============================ Carga masiva: películas + funciones ============================

bool Persistencia::cargarPeliculasYFunciones(
    const string& rutaArchivo,
    ArbolPeliculas* arbol,
    ArbolAVL* avl,
    vector<string>& errores)
{
    errores.clear();
    if (arbol == nullptr || avl == nullptr) return false;

    JsonValue root;
    try {
        root = JsonValue::parsearArchivo(rutaArchivo);
    } catch (const std::exception& e) {
        errores.push_back(string("No se pudo leer el archivo: ") + e.what());
        return false;
    }

    const JsonValue& peliculas = root["peliculas"];
    if (!peliculas.esArray()) {
        errores.push_back("El archivo no contiene el arreglo 'peliculas'.");
        return false;
    }

    int correctos = 0;
    for (size_t i = 0; i < peliculas.vArray.size(); i++) {
        const JsonValue& p = peliculas.vArray[i];
        string prefijo = "Película #" + to_string(i + 1) + ": ";

        // --- Validación de campos obligatorios ---
        string codigo      = Val::trim(p.getTexto("codigo", ""));
        string titulo      = Val::trim(p.getTexto("titulo", ""));
        string genero      = Val::trim(p.getTexto("genero", ""));
        string clasif      = Val::trim(p.getTexto("clasificacion", ""));
        string idioma      = Val::trim(p.getTexto("idioma", ""));
        string fechaEst    = Val::trim(p.getTexto("fecha_estreno", ""));
        string fechaFin    = Val::trim(p.getTexto("fecha_fin", ""));
        int    duracion    = p.getEntero("duracion", 0);

        if (codigo.empty())   { errores.push_back(prefijo + "codigo vacío"); continue; }
        if (titulo.empty())   { errores.push_back(prefijo + "titulo vacío"); continue; }
        if (genero.empty())   { errores.push_back(prefijo + "genero vacío"); continue; }
        if (clasif.empty())   { errores.push_back(prefijo + "clasificacion vacía"); continue; }
        if (idioma.empty())   { errores.push_back(prefijo + "idioma vacío"); continue; }
        if (duracion <= 0)    { errores.push_back(prefijo + "duración inválida"); continue; }
        if (!Val::esFechaValida(fechaEst)) { errores.push_back(prefijo + "fecha_estreno inválida"); continue; }
        if (!Val::esFechaValida(fechaFin)) { errores.push_back(prefijo + "fecha_fin inválida"); continue; }

        // Convertir código Pxxx -> int
        string soloNum = codigo;
        if (soloNum.size() > 1 && (soloNum[0] == 'P' || soloNum[0] == 'p')) soloNum = soloNum.substr(1);
        if (!Val::esEnteroPositivo(soloNum)) {
            errores.push_back(prefijo + "codigo no numérico");
            continue;
        }
        int codigoNum = stoi(soloNum);

        if (arbol->buscar(codigoNum) != nullptr) {
            errores.push_back(prefijo + "código ya existe, se omite");
            continue;
        }

        Pelicula nueva(codigoNum, codigo, titulo, genero, duracion, clasif, idioma, fechaEst, fechaFin);

        // --- Funciones ---
        const JsonValue& funciones = p["funciones"];
        if (funciones.esArray()) {
            for (size_t j = 0; j < funciones.vArray.size(); j++) {
                const JsonValue& f = funciones.vArray[j];
                string prefF = prefijo + "función #" + to_string(j + 1) + ": ";

                string codFun  = Val::trim(f.getTexto("codigo_funcion", ""));
                string horario = Val::trim(f.getTexto("horario", ""));
                string sala    = Val::trim(f.getTexto("sala", ""));
                int filas      = f.getEntero("filas", 0);
                int cols       = f.getEntero("columnas", 0);

                if (codFun.empty()) codFun = siguienteCodigoFuncion(avl);
                if (!Val::esHorarioValido(horario)) { errores.push_back(prefF + "horario inválido"); continue; }
                if (sala.empty())     { errores.push_back(prefF + "sala vacía"); continue; }
                if (filas <= 0)       { errores.push_back(prefF + "filas inválidas"); continue; }
                if (cols  <= 0)       { errores.push_back(prefF + "columnas inválidas"); continue; }

                if (avl->buscar(codFun) != nullptr) {
                    errores.push_back(prefF + "código de función ya existe");
                    continue;
                }

                string archivoAsientos = rutaAsientosFuncion(codFun);
                Funcion nf(codFun, codigo, titulo, horario, sala, filas, cols, archivoAsientos);
                avl->insertar(nf);
                crearArchivoAsientosVacio(archivoAsientos, filas, cols);
                nueva.funciones.push_back(codFun);
            }
        }

        arbol->insertar(nueva);
        correctos++;
    }

    return correctos > 0;
}

// ============================ Carga masiva: clientes + reservas ============================

bool Persistencia::cargarClientesYReservas(
    const string& rutaArchivo,
    ArbolB* arbolB,
    TablaHash* hash,
    ArbolAVL* avlFunciones,
    vector<string>& errores)
{
    errores.clear();
    if (arbolB == nullptr || hash == nullptr || avlFunciones == nullptr) return false;

    JsonValue root;
    try {
        root = JsonValue::parsearArchivo(rutaArchivo);
    } catch (const std::exception& e) {
        errores.push_back(string("No se pudo leer el archivo: ") + e.what());
        return false;
    }

    const JsonValue& clientes = root["clientes"];
    if (!clientes.esArray()) {
        errores.push_back("El archivo no contiene el arreglo 'clientes'.");
        return false;
    }

    // Set de correos para evitar duplicados
    set<string> correosUsados;
    for (const auto& c : arbolB->listar()) correosUsados.insert(c.correo);

    // Agrupamos reservas por función para hacer un solo I/O por archivo
    struct ResTemp { Reserva r; };
    map<string, vector<Reserva>> reservasPorFuncion;
    vector<Reserva> reservasValidas;

    int correctos = 0;
    for (size_t i = 0; i < clientes.vArray.size(); i++) {
        const JsonValue& c = clientes.vArray[i];
        string prefijo = "Cliente #" + to_string(i + 1) + ": ";

        string id     = Val::trim(c.getTexto("id", ""));
        string nombre = Val::trim(c.getTexto("nombre", ""));
        string correo = Val::trim(c.getTexto("correo", ""));
        string tel    = Val::trim(c.getTexto("telefono", ""));
        string pass   = c.getTexto("password", "");
        string tipo   = Val::trim(c.getTexto("tipo", "cliente"));

        if (id.empty())                  { errores.push_back(prefijo + "id vacío"); continue; }
        if (nombre.empty())              { errores.push_back(prefijo + "nombre vacío"); continue; }
        if (!Val::esCorreoValido(correo)){ errores.push_back(prefijo + "correo inválido"); continue; }
        if (!Val::esTelefonoValido(tel)) { errores.push_back(prefijo + "teléfono inválido"); continue; }
        if (pass.empty())                { errores.push_back(prefijo + "password vacío"); continue; }
        if (arbolB->buscar(id) != nullptr) { errores.push_back(prefijo + "id ya existe"); continue; }
        if (correosUsados.count(correo) > 0) { errores.push_back(prefijo + "correo ya registrado"); continue; }

        Cliente nuevo(id, nombre, correo, tel, pass, tipo.empty() ? "cliente" : tipo);

        // --- Reservas ---
        const JsonValue& reservas = c["reservas"];
        if (reservas.esArray()) {
            for (size_t j = 0; j < reservas.vArray.size(); j++) {
                const JsonValue& r = reservas.vArray[j];
                string prefR = prefijo + "reserva #" + to_string(j + 1) + ": ";

                string codRes   = Val::trim(r.getTexto("codigo_reserva", ""));
                string codFun   = Val::trim(r.getTexto("codigo_funcion", ""));
                int fila        = r.getEntero("fila", 0);
                int col         = r.getEntero("columna", 0);
                string fechaRes = Val::trim(r.getTexto("fecha_reserva", ""));

                if (codRes.empty())             { errores.push_back(prefR + "codigo_reserva vacío"); continue; }
                if (codFun.empty())             { errores.push_back(prefR + "codigo_funcion vacío"); continue; }
                if (fila <= 0 || col <= 0)      { errores.push_back(prefR + "fila/columna inválidas"); continue; }
                if (!Val::esFechaValida(fechaRes)) { errores.push_back(prefR + "fecha_reserva inválida"); continue; }

                Funcion* func = avlFunciones->buscar(codFun);
                if (func == nullptr) { errores.push_back(prefR + "la función " + codFun + " no existe"); continue; }
                if (fila > func->filas || col > func->columnas) {
                    errores.push_back(prefR + "asiento fuera de rango");
                    continue;
                }
                if (hash->buscar(codRes) != nullptr) { errores.push_back(prefR + "codigo_reserva duplicado"); continue; }

                Reserva nr(codRes, codFun, id, fila, col, fechaRes);
                reservasValidas.push_back(nr);
                reservasPorFuncion[codFun].push_back(nr);
                nuevo.codigosReserva.push_back(codRes);
            }
        }

        arbolB->insertar(nuevo);
        correosUsados.insert(correo);
        correctos++;
    }

    // Insertar reservas en Hash
    for (const auto& r : reservasValidas) hash->insertar(r);

    // Actualizar los archivos de asientos por función
    for (auto& kv : reservasPorFuncion) {
        const string& codFun = kv.first;
        Funcion* func = avlFunciones->buscar(codFun);
        if (func == nullptr) continue;

        // Cargar el archivo de asientos de esa función en una matriz temporal
        MatrizAsientos temp;
        cargarAsientosFuncion(func->archivoAsientos, &temp);
        temp.setCodigoFuncion(codFun);

        for (const auto& r : kv.second) {
            if (!temp.insertarAsiento(r.fila, r.columna, r.codigoReserva)) {
                errores.push_back("Asiento " + to_string(r.fila) + "," + to_string(r.columna)
                                  + " ya estaba ocupado en " + codFun + " (reserva " + r.codigoReserva + ")");
            }
        }
        guardarAsientosFuncion(func->archivoAsientos, &temp);
    }

    return correctos > 0;
}

// ============================ Guardado masivo (opcional) ============================

bool Persistencia::guardarCartelera(const string& ruta, ArbolPeliculas* arbol, ArbolAVL* avl) {
    if (arbol == nullptr) return false;
    JsonValue root;
    JsonValue arrPelis;
    arrPelis.tipo = JsonValue::ARRAY;

    // Recorremos el BST en inorden
    std::function<void(NodoArbol*)> rec = [&](NodoArbol* n) {
        if (n == nullptr) return;
        rec(n->izquierdo);

        JsonValue p;
        p["codigo"]         = n->pelicula.codigoOriginal;
        p["titulo"]         = n->pelicula.titulo;
        p["genero"]         = n->pelicula.genero;
        p["duracion"]       = n->pelicula.duracion;
        p["clasificacion"]  = n->pelicula.clasificacion;
        p["idioma"]         = n->pelicula.idioma;
        p["fecha_estreno"]  = n->pelicula.fecha_estreno;
        p["fecha_fin"]      = n->pelicula.fecha_fin;

        JsonValue fArr;
        fArr.tipo = JsonValue::ARRAY;
        if (avl != nullptr) {
            for (const auto& f : avl->inorden()) {
                if (f.codigoPelicula == n->pelicula.codigoOriginal) {
                    JsonValue fj;
                    fj["codigo_funcion"] = f.codigoFuncion;
                    fj["horario"]        = f.horario;
                    fj["sala"]           = f.sala;
                    fj["filas"]          = f.filas;
                    fj["columnas"]       = f.columnas;
                    fj["archivo_asientos"] = f.archivoAsientos;
                    fArr.agregar(fj);
                }
            }
        }
        p["funciones"] = fArr;
        arrPelis.agregar(p);

        rec(n->derecho);
    };
    rec(arbol->getRaiz());

    root["peliculas"] = arrPelis;
    return root.guardarArchivo(ruta, 2);
}

bool Persistencia::guardarClientes(const string& ruta, ArbolB* arbolB) {
    if (arbolB == nullptr) return false;
    JsonValue root;
    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;

    for (const auto& c : arbolB->listar()) {
        JsonValue cj;
        cj["id"]       = c.id;
        cj["nombre"]   = c.nombre;
        cj["correo"]   = c.correo;
        cj["telefono"] = c.telefono;
        cj["password"] = c.password;
        cj["tipo"]     = c.tipo;

        JsonValue rArr;
        rArr.tipo = JsonValue::ARRAY;
        for (const auto& rc : c.codigosReserva) {
            rArr.agregar(rc);
        }
        cj["codigos_reserva"] = rArr;
        arr.agregar(cj);
    }
    root["clientes"] = arr;
    return root.guardarArchivo(ruta, 2);
}

bool Persistencia::guardarReservas(const string& ruta, TablaHash* hash) {
    if (hash == nullptr) return false;
    JsonValue root;
    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;

    for (const auto& r : hash->listar()) {
        JsonValue rj;
        rj["codigo_reserva"] = r.codigoReserva;
        rj["codigo_funcion"] = r.codigoFuncion;
        rj["id_cliente"]     = r.idCliente;
        rj["fila"]           = r.fila;
        rj["columna"]        = r.columna;
        rj["fecha_reserva"]  = r.fechaReserva;
        arr.agregar(rj);
    }
    root["reservas"] = arr;
    return root.guardarArchivo(ruta, 2);
}