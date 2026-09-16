#ifndef VALIDACIONES_H
#define VALIDACIONES_H
#include <string>
#include <cctype>
#include <sstream>
#include <ctime>
using namespace std;

namespace Val {

// Fecha válida con formato AAAA-MM-DD y día real (no acepta 2026-02-30).
inline bool esFechaValida(const string& fecha) {
    if (fecha.size() != 10) return false;
    if (fecha[4] != '-' || fecha[7] != '-') return false;
    for (int i : {0,1,2,3,5,6,8,9})
        if (!isdigit(static_cast<unsigned char>(fecha[i]))) return false;

    int anio = stoi(fecha.substr(0, 4));
    int mes  = stoi(fecha.substr(5, 2));
    int dia  = stoi(fecha.substr(8, 2));
    if (anio < 1900 || anio > 2200) return false;
    if (mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > 31) return false;

    int diasMes[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    bool bisiesto = (anio % 4 == 0 && (anio % 100 != 0 || anio % 400 == 0));
    if (bisiesto) diasMes[1] = 29;
    return dia <= diasMes[mes - 1];
}

// Entero estrictamente positivo (sin signos, sin letras).
inline bool esEnteroPositivo(const string& s) {
    if (s.empty()) return false;
    for (char c : s) if (!isdigit(static_cast<unsigned char>(c))) return false;
    try { return stoi(s) > 0; } catch (...) { return false; }
}

// Entero (puede ser negativo).
inline bool esEntero(const string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-' || s[0] == '+') ? 1 : 0;
    if (i == s.size()) return false;
    for (; i < s.size(); i++) if (!isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}

// Horario HH:MM (00:00 - 23:59).
inline bool esHorarioValido(const string& h) {
    if (h.size() != 5 || h[2] != ':') return false;
    for (int i : {0,1,3,4})
        if (!isdigit(static_cast<unsigned char>(h[i]))) return false;
    int hh = stoi(h.substr(0, 2));
    int mm = stoi(h.substr(3, 2));
    return hh >= 0 && hh <= 23 && mm >= 0 && mm <= 59;
}

// Correo con @ y dominio.
inline bool esCorreoValido(const string& c) {
    size_t at = c.find('@');
    if (at == string::npos || at == 0 || at == c.size() - 1) return false;
    size_t punto = c.find('.', at + 1);
    return punto != string::npos && punto < c.size() - 1;
}

// Teléfono: solo dígitos, 8-15 chars.
inline bool esTelefonoValido(const string& t) {
    if (t.size() < 8 || t.size() > 15) return false;
    for (char c : t) if (!isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

inline bool noVacio(const string& s) {
    for (char c : s) if (!isspace(static_cast<unsigned char>(c))) return true;
    return false;
}

inline string trim(const string& s) {
    size_t ini = 0, fin = s.size();
    while (ini < fin && isspace(static_cast<unsigned char>(s[ini]))) ini++;
    while (fin > ini && isspace(static_cast<unsigned char>(s[fin - 1]))) fin--;
    return s.substr(ini, fin - ini);
}

} // namespace Val

#endif