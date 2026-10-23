#include "GrafoSedes.h"
#include <queue>
#include <algorithm>

GrafoSedes::GrafoSedes() {}

void GrafoSedes::vaciar() {
    sedes.clear();
    ady.clear();
    indice.clear();
}

int GrafoSedes::agregarSede(const Sede& s) {
    if (indice.count(s.codigo)) return -1;
    int idx = (int)sedes.size();
    sedes.push_back(s);
    ady.push_back({});
    indice[s.codigo] = idx;
    return idx;
}

bool GrafoSedes::existeSede(const string& cod) const { return indice.count(cod) > 0; }

int GrafoSedes::indiceDe(const string& cod) const {
    auto it = indice.find(cod);
    return it == indice.end() ? -1 : it->second;
}

const Sede& GrafoSedes::getSede(int idx) const { return sedes.at(idx); }

void GrafoSedes::conectar(int a, int b, int peso) {
    ady[a].push_back({b, peso});
    ady[b].push_back({a, peso});
}

void GrafoSedes::reconstruirDesdeCatalogo(const map<string, set<string>>& catalogo) {
    ady.assign(sedes.size(), {});
    for (size_t i = 0; i < sedes.size(); ++i) {
        auto itA = catalogo.find(sedes[i].codigo);
        if (itA == catalogo.end()) continue;
        for (size_t j = i + 1; j < sedes.size(); ++j) {
            auto itB = catalogo.find(sedes[j].codigo);
            if (itB == catalogo.end()) continue;
            int comunes = 0;
            for (const auto& p : itA->second)
                if (itB->second.count(p)) comunes++;
            if (comunes > 0) conectar((int)i, (int)j, comunes);
        }
    }
}

vector<string> GrafoSedes::caminoBFS(const string& origen, const string& destino) const {
    int s = indiceDe(origen), t = indiceDe(destino);
    if (s < 0 || t < 0) return {};
    if (s == t) return { sedes[s].codigo };

    vector<int> prev(sedes.size(), -1);
    vector<bool> visto(sedes.size(), false);
    queue<int> q;
    q.push(s); visto[s] = true;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        if (u == t) break;
        for (const auto& a : ady[u]) {
            if (!visto[a.destino]) {
                visto[a.destino] = true;
                prev[a.destino] = u;
                q.push(a.destino);
            }
        }
    }
    if (!visto[t]) return {};

    vector<string> camino;
    for (int c = t; c != -1; c = prev[c]) camino.push_back(sedes[c].codigo);
    reverse(camino.begin(), camino.end());
    return camino;
}

vector<RankingArista> GrafoSedes::rankingAristas() const {
    vector<RankingArista> r;
    for (size_t i = 0; i < ady.size(); ++i)
        for (const auto& a : ady[i])
            if ((size_t)a.destino > i)
                r.push_back({ sedes[i].codigo, sedes[a.destino].codigo, a.peso });
    sort(r.begin(), r.end(),
         [](const RankingArista& x, const RankingArista& y){ return x.peso > y.peso; });
    return r;
}

vector<RankingArista> GrafoSedes::sedesSimilares(const string& codigo) const {
    vector<RankingArista> r;
    int idx = indiceDe(codigo);
    if (idx < 0) return r;
    for (const auto& a : ady[idx])
        r.push_back({ sedes[idx].codigo, sedes[a.destino].codigo, a.peso });
    sort(r.begin(), r.end(),
         [](const RankingArista& x, const RankingArista& y){ return x.peso > y.peso; });
    return r;
}