#include "Json.h"
#include <fstream>
#include <sstream>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <stdexcept>

// ============================ Constructores ============================

JsonValue::JsonValue() : tipo(NULO), vBooleano(false), vNumero(0) {}
JsonValue::JsonValue(bool b) : tipo(BOOLEANO), vBooleano(b), vNumero(0) {}
JsonValue::JsonValue(double n) : tipo(NUMERO), vBooleano(false), vNumero(n) {}
JsonValue::JsonValue(int n) : tipo(NUMERO), vBooleano(false), vNumero(static_cast<double>(n)) {}
JsonValue::JsonValue(long n) : tipo(NUMERO), vBooleano(false), vNumero(static_cast<double>(n)) {}
JsonValue::JsonValue(const std::string& s) : tipo(TEXTO), vBooleano(false), vNumero(0), vTexto(s) {}
JsonValue::JsonValue(const char* s) : tipo(TEXTO), vBooleano(false), vNumero(0), vTexto(s ? s : "") {}

// ============================ Accesos ============================

bool JsonValue::tiene(const std::string& clave) const {
    return tipo == OBJETO && vObjeto.find(clave) != vObjeto.end();
}

JsonValue& JsonValue::operator[](const std::string& clave) {
    if (tipo != OBJETO) { tipo = OBJETO; vObjeto.clear(); }
    return vObjeto[clave];
}

JsonValue& JsonValue::operator[](size_t indice) {
    if (tipo != ARRAY) { tipo = ARRAY; vArray.clear(); }
    if (indice >= vArray.size()) vArray.resize(indice + 1);
    return vArray[indice];
}

const JsonValue& JsonValue::operator[](const std::string& clave) const {
    static JsonValue nulo;
    auto it = vObjeto.find(clave);
    return (it != vObjeto.end()) ? it->second : nulo;
}

const JsonValue& JsonValue::operator[](size_t indice) const {
    static JsonValue nulo;
    return (indice < vArray.size()) ? vArray[indice] : nulo;
}

std::string JsonValue::getTexto(const std::string& clave, const std::string& porDefecto) const {
    auto it = vObjeto.find(clave);
    if (it == vObjeto.end() || it->second.tipo != TEXTO) return porDefecto;
    return it->second.vTexto;
}

int JsonValue::getEntero(const std::string& clave, int porDefecto) const {
    auto it = vObjeto.find(clave);
    if (it == vObjeto.end() || it->second.tipo != NUMERO) return porDefecto;
    return static_cast<int>(it->second.vNumero);
}

double JsonValue::getDecimal(const std::string& clave, double porDefecto) const {
    auto it = vObjeto.find(clave);
    if (it == vObjeto.end() || it->second.tipo != NUMERO) return porDefecto;
    return it->second.vNumero;
}

bool JsonValue::getBooleano(const std::string& clave, bool porDefecto) const {
    auto it = vObjeto.find(clave);
    if (it == vObjeto.end() || it->second.tipo != BOOLEANO) return porDefecto;
    return it->second.vBooleano;
}

void JsonValue::agregar(const std::string& clave, const JsonValue& valor) {
    if (tipo != OBJETO) { tipo = OBJETO; vObjeto.clear(); }
    vObjeto[clave] = valor;
}

void JsonValue::agregar(const JsonValue& valor) {
    if (tipo != ARRAY) { tipo = ARRAY; vArray.clear(); }
    vArray.push_back(valor);
}

// ============================ Parser ============================

namespace {

class ParserJson {
public:
    ParserJson(const std::string& t) : texto(t), pos(0) {}

    JsonValue parsear() {
        saltarEspacios();
        JsonValue v = parsearValor();
        saltarEspacios();
        if (pos < texto.size())
            throw std::runtime_error("Datos sobrantes tras el JSON");
        return v;
    }

private:
    const std::string& texto;
    size_t pos;

    void saltarEspacios() {
        while (pos < texto.size() && std::isspace(static_cast<unsigned char>(texto[pos]))) pos++;
    }

    char verActual() {
        if (pos >= texto.size()) throw std::runtime_error("Fin inesperado del JSON");
        return texto[pos];
    }

    void consumir(const std::string& esperado) {
        if (texto.substr(pos, esperado.size()) != esperado)
            throw std::runtime_error("Se esperaba: " + esperado);
        pos += esperado.size();
    }

    JsonValue parsearValor() {
        saltarEspacios();
        char c = verActual();
        if (c == '{') return parsearObjeto();
        if (c == '[') return parsearArray();
        if (c == '"') return JsonValue(parsearTexto());
        if (c == 't' || c == 'f') return parsearBooleano();
        if (c == 'n') { consumir("null"); return JsonValue(); }
        return parsearNumero();
    }

    JsonValue parsearObjeto() {
        JsonValue obj;
        obj.tipo = JsonValue::OBJETO;
        consumir("{");
        saltarEspacios();
        if (verActual() == '}') { pos++; return obj; }
        while (true) {
            saltarEspacios();
            std::string clave = parsearTexto();
            saltarEspacios();
            consumir(":");
            JsonValue valor = parsearValor();
            obj.vObjeto[clave] = valor;
            saltarEspacios();
            char c = verActual();
            if (c == ',') { pos++; continue; }
            if (c == '}') { pos++; break; }
            throw std::runtime_error("Se esperaba ',' o '}' en objeto");
        }
        return obj;
    }

    JsonValue parsearArray() {
        JsonValue arr;
        arr.tipo = JsonValue::ARRAY;
        consumir("[");
        saltarEspacios();
        if (verActual() == ']') { pos++; return arr; }
        while (true) {
            JsonValue valor = parsearValor();
            arr.vArray.push_back(valor);
            saltarEspacios();
            char c = verActual();
            if (c == ',') { pos++; continue; }
            if (c == ']') { pos++; break; }
            throw std::runtime_error("Se esperaba ',' o ']' en array");
        }
        return arr;
    }

    std::string parsearTexto() {
        consumir("\"");
        std::string res;
        while (pos < texto.size()) {
            char c = texto[pos++];
            if (c == '"') return res;
            if (c == '\\') {
                if (pos >= texto.size()) throw std::runtime_error("Escape incompleto");
                char e = texto[pos++];
                switch (e) {
                    case '"':  res += '"'; break;
                    case '\\': res += '\\'; break;
                    case '/':  res += '/'; break;
                    case 'b':  res += '\b'; break;
                    case 'f':  res += '\f'; break;
                    case 'n':  res += '\n'; break;
                    case 'r':  res += '\r'; break;
                    case 't':  res += '\t'; break;
                    case 'u': {
                        if (pos + 4 > texto.size())
                            throw std::runtime_error("\\u incompleto");
                        pos += 4;
                        res += '?';
                        break;
                    }
                    default: throw std::runtime_error("Escape desconocido");
                }
            } else {
                res += c;
            }
        }
        throw std::runtime_error("Texto sin cierre");
    }

    JsonValue parsearBooleano() {
        if (texto.substr(pos, 4) == "true")  { pos += 4; return JsonValue(true); }
        if (texto.substr(pos, 5) == "false") { pos += 5; return JsonValue(false); }
        throw std::runtime_error("Booleano inválido");
    }

    JsonValue parsearNumero() {
        size_t inicio = pos;
        if (pos < texto.size() && (texto[pos] == '-' || texto[pos] == '+')) pos++;
        while (pos < texto.size() && std::isdigit(static_cast<unsigned char>(texto[pos]))) pos++;
        if (pos < texto.size() && texto[pos] == '.') {
            pos++;
            while (pos < texto.size() && std::isdigit(static_cast<unsigned char>(texto[pos]))) pos++;
        }
        if (pos < texto.size() && (texto[pos] == 'e' || texto[pos] == 'E')) {
            pos++;
            if (pos < texto.size() && (texto[pos] == '-' || texto[pos] == '+')) pos++;
            while (pos < texto.size() && std::isdigit(static_cast<unsigned char>(texto[pos]))) pos++;
        }
        if (inicio == pos) throw std::runtime_error("Número inválido");
        try {
            return JsonValue(std::stod(texto.substr(inicio, pos - inicio)));
        } catch (...) {
            throw std::runtime_error("Número mal formado");
        }
    }
};

std::string escaparTexto(const std::string& s) {
    std::string r;
    for (char c : s) {
        switch (c) {
            case '"':  r += "\\\""; break;
            case '\\': r += "\\\\"; break;
            case '\n': r += "\\n";  break;
            case '\r': r += "\\r";  break;
            case '\t': r += "\\t";  break;
            default:   r += c;
        }
    }
    return r;
}

std::string numeroATexto(double n) {
    if (std::floor(n) == n && std::abs(n) < 1e15) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(0) << n;
        return oss.str();
    }
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

void serializarRec(const JsonValue& v, std::ostringstream& os, int indent, int nivel) {
    std::string pad(indent * nivel, ' ');
    std::string padHijo(indent * (nivel + 1), ' ');

    switch (v.tipo) {
        case JsonValue::NULO:     os << "null"; break;
        case JsonValue::BOOLEANO: os << (v.vBooleano ? "true" : "false"); break;
        case JsonValue::NUMERO:   os << numeroATexto(v.vNumero); break;
        case JsonValue::TEXTO:    os << '"' << escaparTexto(v.vTexto) << '"'; break;
        case JsonValue::ARRAY: {
            if (v.vArray.empty()) { os << "[]"; break; }
            os << "[\n";
            for (size_t i = 0; i < v.vArray.size(); i++) {
                os << padHijo;
                serializarRec(v.vArray[i], os, indent, nivel + 1);
                if (i + 1 < v.vArray.size()) os << ",";
                os << "\n";
            }
            os << pad << "]";
            break;
        }
        case JsonValue::OBJETO: {
            if (v.vObjeto.empty()) { os << "{}"; break; }
            os << "{\n";
            size_t i = 0;
            for (const auto& kv : v.vObjeto) {
                os << padHijo << '"' << escaparTexto(kv.first) << "\": ";
                serializarRec(kv.second, os, indent, nivel + 1);
                if (++i < v.vObjeto.size()) os << ",";
                os << "\n";
            }
            os << pad << "}";
            break;
        }
    }
}

} // namespace

// ============================ API pública ============================

std::string JsonValue::serializar(int indentacion) const {
    std::ostringstream os;
    serializarRec(*this, os, indentacion, 0);
    return os.str();
}

bool JsonValue::guardarArchivo(const std::string& ruta, int indentacion) const {
    std::ofstream archivo(ruta);
    if (!archivo.is_open()) return false;
    archivo << serializar(indentacion);
    archivo.close();
    return true;
}

JsonValue JsonValue::parsear(const std::string& texto) {
    ParserJson parser(texto);
    return parser.parsear();
}

JsonValue JsonValue::parsearArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo.is_open())
        throw std::runtime_error("No se pudo abrir: " + ruta);
    std::ostringstream ss;
    ss << archivo.rdbuf();
    archivo.close();
    return parsear(ss.str());
}