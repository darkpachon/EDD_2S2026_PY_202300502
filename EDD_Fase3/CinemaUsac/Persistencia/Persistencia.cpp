#include "Persistencia.h"
#include "../Modelos/Json.h"
#include "../Modelos/Sede.h"
#include "../Utilidades/Validaciones.h"
#include "../Utilidades/LZW.h"
#include <fstream>
#include <set>
#include <map>
#include <iostream>
#include <functional>
#include <filesystem>

using namespace std;

// ============= Utilidades =============

string Persistencia::formatearCodigo(const string& prefijo, int numero, int ancho) {
    string s = to_string(numero);
    while ((int)s.size() < ancho) s = "0" + s;
    return prefijo + s;
}

string Persistencia::rutaAsientosFuncion(const string& codigoFuncion) {
    return codigoFuncion + "_funcion.json";
}

string Persistencia::rutaCompletaAsientos(const string& archivoAsientos) {
    return "datos/" + archivoAsientos;
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

// ============= Archivos de asientos =============

bool Persistencia::crearArchivoAsientosVacio(const string& ruta, int filas, int columnas) {
    std::filesystem::create_directories("datos");
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

// ============= Carga masiva: películas + funciones (CON SEDES) =============

bool Persistencia::cargarPeliculasYFunciones(
    const string& rutaArchivo,
    ArbolPeliculas* arbol,
    ArbolAVL* avl,
    GrafoSedes* grafo,
    vector<string>& errores)
{
    ifstream archivo(rutaArchivo, ios::binary);
    if (!archivo.is_open()) {
        errores.push_back("No se pudo abrir: " + rutaArchivo);
        return false;
    }
    ostringstream ss;
    ss << archivo.rdbuf();
    archivo.close();
    return cargarPeliculasYFuncionesDesdeTexto(ss.str(), arbol, avl, grafo, errores);
}

bool Persistencia::cargarPeliculasYFuncionesDesdeTexto(
    const string& contenido,
    ArbolPeliculas* arbol,
    ArbolAVL* avl,
    GrafoSedes* grafo,
    vector<string>& errores)
{
    errores.clear();
    if (arbol == nullptr || avl == nullptr || grafo == nullptr) return false;

    JsonValue root;
    try {
        root = JsonValue::parsear(contenido);
    } catch (const std::exception& e) {
        errores.push_back(string("No se pudo leer el JSON: ") + e.what());
        return false;
    }

    // ---- 1) Cargar sedes al grafo ----
    const JsonValue& sedes = root["sedes"];
    if (sedes.esArray()) {
        for (size_t i = 0; i < sedes.vArray.size(); i++) {
            const JsonValue& s = sedes.vArray[i];
            string cod = Val::trim(s.getTexto("codigo", ""));
            if (cod.empty()) { errores.push_back("Sede #" + to_string(i+1) + ": código vacío"); continue; }
            Sede nueva(cod,
                       s.getTexto("nombre", ""),
                       s.getTexto("direccion", ""),
                       s.getEntero("cantidad_salas", 0),
                       s.getTexto("telefono", ""));
            if (grafo->agregarSede(nueva) < 0) {
                errores.push_back("Sede " + cod + ": ya existía");
            }
        }
    }

    // ---- 2) Cargar películas y funciones ----
    const JsonValue& peliculas = root["peliculas"];
    if (!peliculas.esArray()) {
        errores.push_back("El archivo no contiene el arreglo 'peliculas'.");
        return false;
    }

    int correctos = 0;
    for (size_t i = 0; i < peliculas.vArray.size(); i++) {
        const JsonValue& p = peliculas.vArray[i];
        string prefijo = "Película #" + to_string(i + 1) + ": ";

        string codigo      = Val::trim(p.getTexto("codigo", ""));
        string titulo      = Val::trim(p.getTexto("titulo", ""));
        string genero      = Val::trim(p.getTexto("genero", ""));
        string clasif      = Val::trim(p.getTexto("clasificacion", ""));
        string idioma      = Val::trim(p.getTexto("idioma", ""));
        string fechaEst    = Val::trim(p.getTexto("fecha_estreno", ""));
        string fechaFin    = Val::trim(p.getTexto("fecha_fin", ""));
        int    duracion    = p.getEntero("duracion", 0);

        if (codigo.empty() || titulo.empty() || genero.empty() || clasif.empty() ||
            idioma.empty() || duracion <= 0 ||
            !Val::esFechaValida(fechaEst) || !Val::esFechaValida(fechaFin)) {
            errores.push_back(prefijo + "campos inválidos");
            continue;
        }

        string soloNum = codigo;
        if (soloNum.size() > 1 && (soloNum[0] == 'P' || soloNum[0] == 'p')) soloNum = soloNum.substr(1);
        if (!Val::esEnteroPositivo(soloNum)) {
            errores.push_back(prefijo + "código no numérico");
            continue;
        }
        int codigoNum = stoi(soloNum);

        if (arbol->buscar(codigoNum) != nullptr) {
            errores.push_back(prefijo + "código ya existe, se omite");
            continue;
        }

        Pelicula nueva(codigoNum, codigo, titulo, genero, duracion, clasif, idioma, fechaEst, fechaFin);

        const JsonValue& funciones = p["funciones"];
        if (funciones.esArray()) {
            for (size_t j = 0; j < funciones.vArray.size(); j++) {
                const JsonValue& f = funciones.vArray[j];
                string prefF = prefijo + "función #" + to_string(j + 1) + ": ";

                string codFun  = Val::trim(f.getTexto("codigo_funcion", ""));
                string horario = Val::trim(f.getTexto("horario", ""));
                string sala    = Val::trim(f.getTexto("sala", ""));
                string codSede = Val::trim(f.getTexto("sede", ""));
                int filas      = f.getEntero("filas", 0);
                int cols       = f.getEntero("columnas", 0);

                if (codSede.empty() || !grafo->existeSede(codSede)) {
                    errores.push_back(prefF + "sede inexistente " + codSede + ", función rechazada");
                    continue;
                }
                if (codFun.empty()) codFun = siguienteCodigoFuncion(avl);
                if (!Val::esHorarioValido(horario)) { errores.push_back(prefF + "horario inválido"); continue; }
                if (sala.empty())   { errores.push_back(prefF + "sala vacía"); continue; }
                if (filas <= 0)     { errores.push_back(prefF + "filas inválidas"); continue; }
                if (cols  <= 0)     { errores.push_back(prefF + "columnas inválidas"); continue; }
                if (avl->buscar(codFun) != nullptr) {
                    errores.push_back(prefF + "código de función ya existe");
                    continue;
                }

                string archivoAsientos = rutaAsientosFuncion(codFun);
                Funcion nf(codFun, codigo, titulo, horario, sala, filas, cols, archivoAsientos, codSede);
                avl->insertar(nf);
                crearArchivoAsientosVacio(rutaCompletaAsientos(archivoAsientos), filas, cols);
                nueva.funciones.push_back(codFun);
            }
        }

        arbol->insertar(nueva);
        correctos++;
    }

    return correctos > 0;
}

// ============= Carga masiva: clientes + reservas =============

bool Persistencia::cargarClientesYReservas(
    const string& rutaArchivo,
    ArbolB* arbolB,
    TablaHash* hash,
    ArbolAVL* avlFunciones,
    vector<string>& errores)
{
    ifstream archivo(rutaArchivo, ios::binary);
    if (!archivo.is_open()) {
        errores.push_back("No se pudo abrir: " + rutaArchivo);
        return false;
    }
    ostringstream ss;
    ss << archivo.rdbuf();
    archivo.close();
    return cargarClientesYReservasDesdeTexto(ss.str(), arbolB, hash, avlFunciones, errores);
}

bool Persistencia::cargarClientesYReservasDesdeTexto(
    const string& contenido,
    ArbolB* arbolB,
    TablaHash* hash,
    ArbolAVL* avlFunciones,
    vector<string>& errores)
{
    errores.clear();
    if (arbolB == nullptr || hash == nullptr || avlFunciones == nullptr) return false;

    JsonValue root;
    try {
        root = JsonValue::parsear(contenido);
    } catch (const std::exception& e) {
        errores.push_back(string("No se pudo leer el JSON: ") + e.what());
        return false;
    }

    const JsonValue& clientes = root["clientes"];
    if (!clientes.esArray()) {
        errores.push_back("El archivo no contiene el arreglo 'clientes'.");
        return false;
    }

    set<string> correosUsados;
    for (const auto& c : arbolB->listar()) correosUsados.insert(c.correo);

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

        if (id.empty() || nombre.empty() || !Val::esCorreoValido(correo) ||
            !Val::esTelefonoValido(tel) || pass.empty()) {
            errores.push_back(prefijo + "campos inválidos");
            continue;
        }
        if (arbolB->buscar(id) != nullptr) { errores.push_back(prefijo + "id ya existe"); continue; }
        if (correosUsados.count(correo) > 0) { errores.push_back(prefijo + "correo duplicado"); continue; }

        Cliente nuevo(id, nombre, correo, tel, pass, tipo.empty() ? "cliente" : tipo);

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

                if (codRes.empty() || codFun.empty() || fila <= 0 || col <= 0 ||
                    !Val::esFechaValida(fechaRes)) {
                    errores.push_back(prefR + "campos inválidos");
                    continue;
                }
                Funcion* func = avlFunciones->buscar(codFun);
                if (func == nullptr) { errores.push_back(prefR + "función " + codFun + " no existe"); continue; }
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

    for (const auto& r : reservasValidas) hash->insertar(r);

    for (auto& kv : reservasPorFuncion) {
        const string& codFun = kv.first;
        Funcion* func = avlFunciones->buscar(codFun);
        if (func == nullptr) continue;
        MatrizAsientos temp;
        cargarAsientosFuncion(rutaCompletaAsientos(func->archivoAsientos), &temp);
        temp.setCodigoFuncion(codFun);
        for (const auto& r : kv.second)
            temp.insertarAsiento(r.fila, r.columna, r.codigoReserva);
        guardarAsientosFuncion(rutaCompletaAsientos(func->archivoAsientos), &temp);
    }
    return correctos > 0;
}

// ============= Guardado legacy (JSON) =============

bool Persistencia::guardarCartelera(const string& ruta, ArbolPeliculas* arbol, ArbolAVL* avl) {
    if (arbol == nullptr) return false;
    JsonValue root;
    JsonValue arrPelis;
    arrPelis.tipo = JsonValue::ARRAY;

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
                    fj["codigo_funcion"]   = f.codigoFuncion;
                    fj["horario"]          = f.horario;
                    fj["sala"]             = f.sala;
                    fj["filas"]            = f.filas;
                    fj["columnas"]         = f.columnas;
                    fj["archivo_asientos"] = f.archivoAsientos;
                    fj["sede"]             = f.sede;
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

bool Persistencia::guardarClientes(const string& ruta, ArbolB* arbolB, TablaHash* hash) {
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
        for (const auto& codRes : c.codigosReserva) {
            if (hash == nullptr) break;
            Reserva* r = hash->buscar(codRes);
            if (r == nullptr) continue;
            JsonValue rj;
            rj["codigo_reserva"] = r->codigoReserva;
            rj["codigo_funcion"] = r->codigoFuncion;
            rj["fila"]           = r->fila;
            rj["columna"]        = r->columna;
            rj["fecha_reserva"]  = r->fechaReserva;
            rArr.agregar(rj);
        }
        cj["reservas"] = rArr;
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

// ============= Fase 3: compresión LZW a .edd =============

static bool guardarTextoComprimido(const string& ruta, const string& json) {
    string comprimido = lzwComprimir(json);
    std::filesystem::create_directories(std::filesystem::path(ruta).parent_path());
    ofstream f(ruta, ios::binary);
    if (!f.is_open()) return false;
    f.write(comprimido.data(), comprimido.size());
    f.close();
    return true;
}

static bool leerTextoDescomprimido(const string& ruta, string& jsonOut) {
    ifstream f(ruta, ios::binary);
    if (!f.is_open()) return false;
    ostringstream ss; ss << f.rdbuf();
    f.close();
    jsonOut = lzwDescomprimir(ss.str());
    return !jsonOut.empty();
}

bool Persistencia::guardarSedesEDD(const string& ruta, GrafoSedes* grafo) {
    if (grafo == nullptr) return false;
    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;
    for (int i = 0; i < grafo->cantidadSedes(); i++) {
        const Sede& s = grafo->getSede(i);
        JsonValue j;
        j["codigo"]         = s.codigo;
        j["nombre"]         = s.nombre;
        j["direccion"]      = s.direccion;
        j["cantidad_salas"] = s.cantidadSalas;
        j["telefono"]       = s.telefono;
        arr.agregar(j);
    }
    return guardarTextoComprimido(ruta, arr.serializar(0));
}

bool Persistencia::guardarPeliculasEDD(const string& ruta, ArbolPeliculas* arbol) {
    if (arbol == nullptr) return false;
    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;
    std::function<void(NodoArbol*)> rec = [&](NodoArbol* n) {
        if (!n) return;
        rec(n->izquierdo);
        JsonValue p;
        p["codigo"]        = n->pelicula.codigoOriginal;
        p["titulo"]        = n->pelicula.titulo;
        p["genero"]        = n->pelicula.genero;
        p["duracion"]      = n->pelicula.duracion;
        p["clasificacion"] = n->pelicula.clasificacion;
        p["idioma"]        = n->pelicula.idioma;
        p["fecha_estreno"] = n->pelicula.fecha_estreno;
        p["fecha_fin"]     = n->pelicula.fecha_fin;
        arr.agregar(p);
        rec(n->derecho);
    };
    rec(arbol->getRaiz());
    return guardarTextoComprimido(ruta, arr.serializar(0));
}

bool Persistencia::guardarFuncionesEDD(const string& ruta, ArbolAVL* avl) {
    if (avl == nullptr) return false;
    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;
    for (const auto& f : avl->inorden()) {
        JsonValue fj;
        fj["codigo_funcion"]   = f.codigoFuncion;
        fj["codigo_pelicula"]  = f.codigoPelicula;
        fj["titulo_pelicula"]  = f.tituloPelicula;
        fj["horario"]          = f.horario;
        fj["sala"]             = f.sala;
        fj["filas"]            = f.filas;
        fj["columnas"]         = f.columnas;
        fj["archivo_asientos"] = f.archivoAsientos;
        fj["sede"]             = f.sede;
        arr.agregar(fj);
    }
    return guardarTextoComprimido(ruta, arr.serializar(0));
}

bool Persistencia::cargarSedesEDD(const string& ruta, GrafoSedes* grafo) {
    if (grafo == nullptr) return false;
    string json;
    if (!leerTextoDescomprimido(ruta, json)) return false;
    try {
        JsonValue arr = JsonValue::parsear(json);
        if (!arr.esArray()) return false;
        for (const auto& j : arr.vArray) {
            Sede s(j.getTexto("codigo", ""),
                   j.getTexto("nombre", ""),
                   j.getTexto("direccion", ""),
                   j.getEntero("cantidad_salas", 0),
                   j.getTexto("telefono", ""));
            if (!s.codigo.empty()) grafo->agregarSede(s);
        }
        return true;
    } catch (...) { return false; }
}

bool Persistencia::cargarPeliculasEDD(const string& ruta, ArbolPeliculas* arbol) {
    if (arbol == nullptr) return false;
    string json;
    if (!leerTextoDescomprimido(ruta, json)) return false;
    try {
        JsonValue arr = JsonValue::parsear(json);
        if (!arr.esArray()) return false;
        for (const auto& j : arr.vArray) {
            string codStr = j.getTexto("codigo", "");
            string soloNum = codStr;
            if (soloNum.size() > 1 && (soloNum[0] == 'P' || soloNum[0] == 'p')) soloNum = soloNum.substr(1);
            int codigo = 0;
            try { codigo = stoi(soloNum); } catch (...) { continue; }
            Pelicula p(codigo, codStr,
                       j.getTexto("titulo", ""),
                       j.getTexto("genero", ""),
                       j.getEntero("duracion", 0),
                       j.getTexto("clasificacion", ""),
                       j.getTexto("idioma", ""),
                       j.getTexto("fecha_estreno", ""),
                       j.getTexto("fecha_fin", ""));
            arbol->insertar(p);
        }
        return true;
    } catch (...) { return false; }
}

bool Persistencia::cargarFuncionesEDD(const string& ruta, ArbolAVL* avl) {
    if (avl == nullptr) return false;
    string json;
    if (!leerTextoDescomprimido(ruta, json)) return false;
    try {
        JsonValue arr = JsonValue::parsear(json);
        if (!arr.esArray()) return false;
        for (const auto& j : arr.vArray) {
            Funcion f(j.getTexto("codigo_funcion", ""),
                      j.getTexto("codigo_pelicula", ""),
                      j.getTexto("titulo_pelicula", ""),
                      j.getTexto("horario", ""),
                      j.getTexto("sala", ""),
                      j.getEntero("filas", 0),
                      j.getEntero("columnas", 0),
                      j.getTexto("archivo_asientos", ""),
                      j.getTexto("sede", ""));
            if (!f.codigoFuncion.empty()) avl->insertar(f);
        }
        return true;
    } catch (...) { return false; }
}