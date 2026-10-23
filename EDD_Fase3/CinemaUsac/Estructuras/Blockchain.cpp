#include "Blockchain.h"
#include "MerkleArbol.h"
#include "../Utilidades/SHA256.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <ctime>
#include <stdexcept>
#include <algorithm>

namespace fs = std::filesystem;

std::string Bloque::calcularHash() const {
    return sha256Hex(std::to_string(index) + timestamp + std::to_string(nonce) +
                     rootMerkle + previousHash);
}

std::string Bloque::calcularRootMerkle() const {
    std::vector<std::string> hojas;
    for (const auto& t : data) hojas.push_back(t.serializar(0));
    MerkleArbol m;
    m.construir(hojas);
    return m.getRaizHash();
}

JsonValue Bloque::aJson() const {
    JsonValue j;
    j["INDEX"]        = (int)index;
    j["TIMESTAMP"]    = timestamp;
    j["NONCE"]        = (long)nonce;
    JsonValue arr;
    arr.tipo = JsonValue::ARRAY;
    for (const auto& t : data) arr.agregar(t);
    j["DATA"]         = arr;
    j["ROOTMERKLE"]   = rootMerkle;
    j["PREVIOUSHASH"] = previousHash;
    j["HASH"]         = hash;
    return j;
}

Bloque Bloque::desdeJson(const JsonValue& j) {
    Bloque b;
    b.index        = j.getEntero("INDEX", 0);
    b.timestamp    = j.getTexto("TIMESTAMP", "");
    b.nonce        = (long long)j.getDecimal("NONCE", 0);
    if (j.tiene("DATA") && j["DATA"].esArray())
        for (const auto& t : j["DATA"].vArray) b.data.push_back(t);
    b.rootMerkle   = j.getTexto("ROOTMERKLE", "");
    b.previousHash = j.getTexto("PREVIOUSHASH", "");
    b.hash         = j.getTexto("HASH", "");
    return b;
}

Blockchain::Blockchain(const std::string& carpeta) : carpetaBlocks(carpeta) {
    fs::create_directories(carpetaBlocks);
}

std::string Blockchain::timestampActual() {
    std::time_t ahora = std::time(nullptr);
    std::tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &ahora);
#else
    localtime_r(&ahora, &tmv);
#endif
    char buf[32];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y::%H:%M:%S", &tmv);
    return buf;
}

void Blockchain::cargarDesdeDisco() {
    bloques.clear();
    integridad.clear();

    for (int i = 0;; ++i) {
        std::string ruta = carpetaBlocks + "/block" + std::to_string(i) + ".json";
        std::ifstream f(ruta, std::ios::binary);
        if (!f.good()) break;
        std::stringstream ss; ss << f.rdbuf();
        try {
            JsonValue j = JsonValue::parsear(ss.str());
            bloques.push_back(Bloque::desdeJson(j));
        } catch (...) { break; }
    }

    std::string prev = GENESIS_PREV;
    for (size_t i = 0; i < bloques.size(); ++i) {
        const Bloque& b = bloques[i];
        bool ok = true;
        if (b.index != (int)i)                          ok = false;
        if (b.previousHash != prev)                     ok = false;
        if (b.calcularRootMerkle() != b.rootMerkle)     ok = false;
        std::string h = b.calcularHash();
        if (h != b.hash)                                ok = false;
        if (h.rfind(POW_PREFIX, 0) != 0)                ok = false;
        integridad.push_back(ok);
        prev = b.hash;
    }
}

int Blockchain::ultimoIndiceValido() const {
    int ult = -1;
    for (size_t i = 0; i < integridad.size(); ++i) {
        if (!integridad[i]) break;
        ult = (int)i;
    }
    return ult;
}

std::string Blockchain::ultimoHashValido() const {
    int i = ultimoIndiceValido();
    return i >= 0 ? bloques[i].hash : GENESIS_PREV;
}

void Blockchain::guardarBloque(const Bloque& b) const {
    std::ofstream f(carpetaBlocks + "/block" + std::to_string(b.index) + ".json");
    f << b.aJson().serializar(2) << "\n";
}

Bloque Blockchain::minar(const std::vector<JsonValue>& transacciones) {
    if (transacciones.empty())
        throw std::runtime_error("No hay transacciones pendientes para minar");

    int ult = ultimoIndiceValido();
    bloques.resize(ult + 1);
    integridad.resize(ult + 1);

    Bloque b;
    b.index        = (int)bloques.size();
    b.timestamp    = timestampActual();
    b.data         = transacciones;
    b.rootMerkle   = b.calcularRootMerkle();
    b.previousHash = ultimoHashValido();
    b.nonce        = 0;

    while (true) {
        b.hash = b.calcularHash();
        if (b.hash.rfind(POW_PREFIX, 0) == 0) break;
        ++b.nonce;
    }

    guardarBloque(b);
    bloques.push_back(b);
    integridad.push_back(true);
    return b;
}

JsonValue Blockchain::bloqueComoJson(int idx) const {
    if (idx < 0 || idx >= (int)bloques.size())
        throw std::runtime_error("Indice de bloque invalido");
    return bloques[idx].aJson();
}

std::vector<JsonValue> Blockchain::todasTransaccionesValidas() const {
    std::vector<JsonValue> t;
    int ult = ultimoIndiceValido();
    for (int i = 0; i <= ult; ++i)
        for (const auto& tx : bloques[i].data) t.push_back(tx);
    return t;
}