#include "Paciente.h"
#include <algorithm>
#include <iostream>
#include "Texto.h"
using namespace std;

Paciente::Paciente(const string& nombre, const Fecha& nac) : nac_(nac) { setNombre(nombre); }

void Paciente::setNombre(const string& n) {
    nombre_ = Texto::trim(n);          // se conserva tal como se escribió
    clave_ = Texto::clave(nombre_);    // solo para buscar/ordenar (sin mayúsculas ni acentos)
}

int Paciente::edadMeses(const Fecha& hoy) const { return nac_.mesesHasta(hoy); }
int Paciente::edadAnios(const Fecha& hoy) const { return edadMeses(hoy) / 12; }

string Paciente::edadStr(const Fecha& hoy) const {
    int m = max(0, edadMeses(hoy));
    return to_string(m / 12) + " año(s) " + to_string(m % 12) + " mes(es)";
}

bool Paciente::tieneAplicada(const string& v, const Fecha& f) const {
    for (auto& d : aplicadas_) if (d.mismaVacuna(v) && d.fecha() == f) return true;
    return false;
}

bool Paciente::retirarProximaMasCercana(const string& v) {
    int mejor = -1;
    for (size_t k = 0; k < proximas_.size(); k++)
        if (proximas_[k].mismaVacuna(v) && (mejor < 0 || proximas_[k].fecha() < proximas_[mejor].fecha()))
            mejor = (int)k;
    if (mejor < 0) return false;
    proximas_.erase(proximas_.begin() + mejor);
    return true;
}

bool Paciente::coincideNombre(const string& k) const { return clave_.find(k) != string::npos; }

int Paciente::contarAplicadasEntre(long long lo, long long hi, map<string, int>* porVac) const {
    int n = 0;
    for (auto& d : aplicadas_) {
        long long x = d.fecha().dias();
        if (x >= lo && x <= hi) { n++; if (porVac) (*porVac)[d.canonica()]++; }
    }
    return n;
}

vector<Dosis> Paciente::proximasEntre(const string& v, long long lo, long long hi) const {
    vector<Dosis> r;
    for (auto& d : proximas_) {
        long long x = d.fecha().dias();
        if (d.mismaVacuna(v) && x >= lo && x <= hi) r.push_back(d);
    }
    return r;
}

vector<Dosis> Paciente::aplicadasEnMes(const Fecha& hoy) const {
    vector<Dosis> r;
    for (auto& d : aplicadas_)
        if (d.fecha().mismoMes(hoy) && d.fecha() <= hoy) r.push_back(d);
    return r;
}

void Paciente::mostrar(ostream& os, const Fecha& hoy) const {
    os << "  Nombre: " << nombre_ << "\n  Nacimiento: " << nac_.str()
       << "   Edad: " << edadStr(hoy) << "\n  Vacunas aplicadas:\n";
    if (aplicadas_.empty()) os << "    (ninguna)\n";
    for (auto& d : aplicadas_) os << "    - " << d.vacuna() << " (" << d.fecha().str() << ")\n";
    os << "  Vacunas próximas:\n";
    if (proximas_.empty()) os << "    (ninguna)\n";
    for (auto& d : proximas_) os << "    - " << d.vacuna() << " (" << d.fecha().str() << ")\n";
}

string Paciente::aLinea() const {
    return nombre_ + "; " + nac_.str() + "; " + Dosis::listaStr(aplicadas_) + "; " +
           Dosis::listaStr(proximas_) + ".";
}

string Paciente::lineaParaGuardar() const {
    string actual = aLinea();
    return (!original_.empty() && actual == firma_) ? original_ : actual;
}

bool Paciente::desdeLinea(const string& linea, Paciente& p, string& err) {
    string l = Texto::trim(linea);
    if (!l.empty() && l.back() == '.') l.pop_back();
    auto s = Texto::dividir(l, ';');
    if (s.size() < 3 || s.size() > 4) { err = "formato incorrecto"; return false; }
    if (s.size() == 3) s.push_back("");
    Paciente t;
    t.setNombre(s[0]);
    if (!Texto::nombreValido(t.nombre_)) { err = "nombre inválido"; return false; }
    if (!Fecha::parsear(s[1], t.nac_)) { err = "fecha de nacimiento inválida"; return false; }
    if (!Dosis::parsearLista(s[2], t.aplicadas_, err)) return false;
    if (!Dosis::parsearLista(s[3], t.proximas_, err)) return false;
    t.firma_ = t.aLinea();
    t.original_ = linea;
    while (!t.original_.empty() && (t.original_.back() == '\r' || t.original_.back() == '\n')) t.original_.pop_back();
    p = std::move(t);
    return true;
}