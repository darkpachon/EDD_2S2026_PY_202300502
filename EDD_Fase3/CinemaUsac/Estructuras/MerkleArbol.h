#ifndef MERKLEARBOL_H
#define MERKLEARBOL_H
#include <string>
#include <vector>
#include <memory>

struct NodoMerkle {
    std::string hash;
    std::shared_ptr<NodoMerkle> izq, der;
    bool esHoja() const { return !izq && !der; }
};

class MerkleArbol {
private:
    std::shared_ptr<NodoMerkle> raiz;

public:
    void construir(const std::vector<std::string>& datos);
    std::string getRaizHash() const { return raiz ? raiz->hash : ""; }
    std::shared_ptr<NodoMerkle> getRaiz() const { return raiz; }
    void vaciar() { raiz.reset(); }
};

#endif