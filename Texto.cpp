#include "Texto.h"
#include <cctype>
#include <sstream>
using namespace std;

string Texto::trim(const string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

vector<string> Texto::dividir(const string& s, char sep) {
    vector<string> r;
    string cur;
    for (char c : s) {
        if (c == sep) { r.push_back(cur); cur.clear(); }
        else cur += c;
    }
    r.push_back(cur);
    return r;
}

string Texto::minusculas(const string& s) {
    string r;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = s[i];
        if (c == 0xC3 && i + 1 < s.size()) {
            unsigned char d = s[i + 1];
            if (d >= 0x80 && d <= 0x9E && d != 0x97) d += 0x20;
            r += (char)c; r += (char)d; i++;
        } else r += (char)tolower(c);
    }
    return r;
}

string Texto::quitarAcentos(const string& s) {
    string r;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = s[i];
        if (c == 0xC3 && i + 1 < s.size()) {
            unsigned char d = s[i + 1];
            char o = 0;
            switch (d) {
                case 0xA1: case 0x81: o = 'a'; break;
                case 0xA9: case 0x89: o = 'e'; break;
                case 0xAD: case 0x8D: o = 'i'; break;
                case 0xB3: case 0x93: o = 'o'; break;
                case 0xBA: case 0x9A: case 0xBC: case 0x9C: o = 'u'; break;
                case 0xB1: case 0x91: o = 'n'; break;
            }
            if (o) { r += o; i++; continue; }
        }
        r += (char)c;
    }
    return r;
}

string Texto::limpiarEspacios(const string& s) {
    string o;
    bool sp = false;
    for (char c : trim(s)) {
        if (isspace((unsigned char)c)) { sp = true; continue; }
        if (c == ',') { o += ','; sp = true; continue; }
        if (sp && !o.empty()) o += ' ';
        sp = false;
        o += c;
    }
    return o;
}

string Texto::clave(const string& s) { return minusculas(quitarAcentos(limpiarEspacios(s))); }

bool Texto::nombreValido(const string& n) {
    if (n.empty()) return false;
    bool hayLetra = false;
    for (unsigned char c : n) {
        if (isalpha(c) || c >= 0x80) hayLetra = true;
        else if (c != ' ' && c != ',' && c != '\'' && c != '-') return false;
    }
    return hayLetra;
}