#include "Consola.h"
#include <iostream>
#include <stdexcept>
#include "Texto.h"
#include "Vacuna.h"
using namespace std;

string Consola::leer(const string& msg) const {
    cout << msg;
    string s;
    if (!getline(cin, s)) throw runtime_error("fin de entrada");
    return Texto::trim(s);
}

int Consola::leerEntero(const string& msg, int mn, int mx) const {
    while (true) {
        string s = leer(msg);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size() && v >= mn && v <= mx) return v;
        } catch (...) {}
        cout << "  Entrada inválida (" << mn << "-" << mx << ").\n";
    }
}

Fecha Consola::leerFecha(const string& msg, const Fecha* porDefecto) const {
    while (true) {
        string s = leer(msg);
        if (s.empty() && porDefecto) return *porDefecto;
        Fecha f;
        if (Fecha::parsear(s, f)) return f;
        cout << "  Fecha inválida. Use dd/mm/aaaa (ej. 27/09/2020).\n";
    }
}

string Consola::leerVacuna(const string& msg) const {
    while (true) {
        string c, s = leer(msg);
        if (CatalogoVacunas::validar(s, c)) return s;   // se acepta sin importar mayúsculas/acentos y se guarda como se escribió
        cout << "  Vacuna no reconocida. Válidas: ";
        const auto& l = CatalogoVacunas::lista();
        for (size_t i = 0; i < l.size(); i++) cout << (i ? ", " : "") << l[i];
        cout << "\n";
    }
}

string Consola::leerNombre(const string& msg) const {
    while (true) {
        string n = leer(msg);
        if (Texto::nombreValido(n)) return n;   // se guarda tal como se escribió
        cout << "  Nombre inválido (solo letras, espacios, comas, guion y apóstrofo).\n";
    }
}

bool Consola::confirmar(const string& msg) const {
    string s = Texto::clave(leer(msg + " (s/n): "));
    return s == "s" || s == "si";
}