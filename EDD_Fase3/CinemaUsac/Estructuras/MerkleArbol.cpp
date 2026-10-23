#include "MerkleArbol.h"
#include "../Utilidades/SHA256.h"

void MerkleArbol::construir(const std::vector<std::string>& datos) {
    raiz.reset();
    if (datos.empty()) return;

    std::vector<std::shared_ptr<NodoMerkle>> nivel;
    for (const auto& d : datos) {
        auto n = std::make_shared<NodoMerkle>();
        n->hash = sha256Hex(d);
        nivel.push_back(n);
    }

    while (nivel.size() > 1) {
        std::vector<std::shared_ptr<NodoMerkle>> sig;
        for (size_t i = 0; i < nivel.size(); i += 2) {
            auto padre = std::make_shared<NodoMerkle>();
            padre->izq = nivel[i];
            padre->der = (i + 1 < nivel.size()) ? nivel[i + 1] : nivel[i];
            padre->hash = sha256Hex(padre->izq->hash + padre->der->hash);
            sig.push_back(padre);
        }
        nivel = sig;
    }
    raiz = nivel[0];
}