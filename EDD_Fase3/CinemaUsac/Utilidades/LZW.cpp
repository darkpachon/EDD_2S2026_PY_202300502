#include "LZW.h"
#include <map>
#include <vector>

std::string lzwComprimir(const std::string& entrada) {
    if (entrada.empty()) return "";
    std::map<std::string,int> dic;
    for (int i = 0; i < 256; i++) dic[std::string(1, (char)i)] = i;
    int sig = 256;
    std::string out, w;

    auto emitir = [&](int cod) {
        out.push_back((char)((cod >> 8) & 0xff));
        out.push_back((char)(cod & 0xff));
    };

    for (char c : entrada) {
        std::string wc = w + c;
        if (dic.count(wc)) w = wc;
        else {
            emitir(dic[w]);
            if (sig < 65535) dic[wc] = sig++;
            w = std::string(1, c);
        }
    }
    if (!w.empty()) emitir(dic[w]);
    return out;
}

std::string lzwDescomprimir(const std::string& entrada) {
    if (entrada.size() < 2) return "";
    std::vector<std::string> dic(256);
    for (int i = 0; i < 256; i++) dic[i] = std::string(1, (char)i);
    int sig = 256;

    auto leer = [&](size_t pos) -> int {
        return ((unsigned char)entrada[pos] << 8) | (unsigned char)entrada[pos+1];
    };

    int cod = leer(0);
    std::string out = dic[cod];
    std::string w = out;

    for (size_t i = 2; i + 1 < entrada.size(); i += 2) {
        int c = leer(i);
        std::string ent;
        if (c < (int)dic.size() && c < sig) ent = dic[c];
        else ent = w + w[0];

        out += ent;
        if (sig < 65535) { dic.push_back(w + ent[0]); sig++; }
        w = ent;
    }
    return out;
}