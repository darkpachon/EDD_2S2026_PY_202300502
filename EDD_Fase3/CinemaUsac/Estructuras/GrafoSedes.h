#ifndef GRAFOSEDES_H
#define GRAFOSEDES_H
#include "../Modelos/Sede.h"
#include <string>
#include <vector>
#include <map>
#include <set>
using namespace std;

struct AristaGrafo { int destino; int peso; };

struct RankingArista {
    string sedeA, sedeB;
    int peso;
};

class GrafoSedes {
private:
    vector<Sede> sedes;
    vector<vector<AristaGrafo>> ady;
    map<string,int> indice;

    void conectar(int a, int b, int peso);

public:
    GrafoSedes();

    int  agregarSede(const Sede& s);
    bool existeSede(const string& cod) const;
    int  indiceDe(const string& cod) const;
    const Sede& getSede(int idx) const;
    int  cantidadSedes() const { return (int)sedes.size(); }
    const vector<Sede>& getSedes() const { return sedes; }
    const vector<AristaGrafo>& vecinos(int idx) const { return ady.at(idx); }

    void reconstruirDesdeCatalogo(const map<string, set<string>>& catalogo);

    vector<string> caminoBFS(const string& origen, const string& destino) const;
    vector<RankingArista> rankingAristas() const;
    vector<RankingArista> sedesSimilares(const string& codigo) const;

    void vaciar();
};

#endif