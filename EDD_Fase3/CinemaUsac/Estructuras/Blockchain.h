#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H
#include <string>
#include <vector>
#include "../Modelos/Json.h"

static const std::string POW_PREFIX   = "0000";
static const std::string GENESIS_PREV = "0000";

struct Bloque {
    int index = 0;
    std::string timestamp;
    long long nonce = 0;
    std::vector<JsonValue> data;
    std::string rootMerkle;
    std::string previousHash;
    std::string hash;

    std::string calcularHash() const;
    std::string calcularRootMerkle() const;
    JsonValue  aJson() const;
    static Bloque desdeJson(const JsonValue& j);
};

class Blockchain {
private:
    std::string carpetaBlocks;
    std::vector<Bloque> bloques;
    std::vector<bool> integridad;

    void guardarBloque(const Bloque& b) const;
    static std::string timestampActual();

public:
    explicit Blockchain(const std::string& carpeta = "blocks");

    void cargarDesdeDisco();
    const std::vector<Bloque>& getBloques() const { return bloques; }
    const std::vector<bool>&   getIntegridad() const { return integridad; }

    int ultimoIndiceValido() const;
    std::string ultimoHashValido() const;

    Bloque minar(const std::vector<JsonValue>& transacciones);
    JsonValue bloqueComoJson(int idx) const;
    std::vector<JsonValue> todasTransaccionesValidas() const;
};

#endif