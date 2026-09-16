#ifndef JSON_H
#define JSON_H
#include <string>
#include <vector>
#include <map>

// Parser y escritor JSON manual (sin dependencias externas).
// Soporta: null, bool, número, string, array, objeto. Ignora \uXXXX (lo marca como '?').
class JsonValue {
public:
    enum Tipo { NULO, BOOLEANO, NUMERO, TEXTO, ARRAY, OBJETO };

    Tipo tipo;
    bool vBooleano;
    double vNumero;
    std::string vTexto;
    std::vector<JsonValue> vArray;
    std::map<std::string, JsonValue> vObjeto;

    JsonValue();                          // NULO
    JsonValue(bool b);
    JsonValue(double n);
    JsonValue(int n);
    JsonValue(long n);
    JsonValue(const std::string& s);
    JsonValue(const char* s);

    bool esNulo()     const { return tipo == NULO; }
    bool esBooleano() const { return tipo == BOOLEANO; }
    bool esNumero()   const { return tipo == NUMERO; }
    bool esTexto()    const { return tipo == TEXTO; }
    bool esArray()    const { return tipo == ARRAY; }
    bool esObjeto()   const { return tipo == OBJETO; }

    bool tiene(const std::string& clave) const;

    // Accesos (crean nodos si no existen al escribir)
    JsonValue&       operator[](const std::string& clave);
    JsonValue&       operator[](size_t indice);
    const JsonValue& operator[](const std::string& clave) const;
    const JsonValue& operator[](size_t indice) const;

    // Accesos directos tipados con valor por defecto
    std::string getTexto(const std::string& clave, const std::string& porDefecto = "") const;
    int         getEntero(const std::string& clave, int porDefecto = 0) const;
    double      getDecimal(const std::string& clave, double porDefecto = 0) const;
    bool        getBooleano(const std::string& clave, bool porDefecto = false) const;

    // Construcción
    void agregar(const std::string& clave, const JsonValue& valor);
    void agregar(const JsonValue& valor);

    // Serialización
    std::string serializar(int indentacion = 0) const;
    bool guardarArchivo(const std::string& ruta, int indentacion = 0) const;

    // Parseo
    static JsonValue parsear(const std::string& texto);
    static JsonValue parsearArchivo(const std::string& ruta);
};

#endif