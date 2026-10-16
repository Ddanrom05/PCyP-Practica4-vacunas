#include "Vacuna.h"
#include "Texto.h"
using namespace std;

const vector<string>& CatalogoVacunas::lista() {
    static const vector<string> v = {
        "BCG", "hepatitis B", "hepatitis A", "pentavalente", "hexavalente", "rotavirus",
        "neumococo", "influenza", "sarampión", "rubéola", "paperas", "triple viral", "SRP",
        "varicela", "tétanos", "difteria", "tos ferina", "DPT", "polio", "VPH",
        "meningococo", "fiebre amarilla", "COVID-19"};
    return v;
}

bool CatalogoVacunas::validar(const string& entrada, string& canon) {
    string k = Texto::clave(entrada);
    if (k.empty()) return false;
    for (auto& c : lista())
        if (Texto::clave(c) == k) { canon = c; return true; }
    return false;
}

Dosis::Dosis(string v, Fecha f) : vacuna_(std::move(v)), fecha_(f) { actualizar(); }

void Dosis::actualizar() {
    vclave_ = Texto::clave(vacuna_);
    string c;
    canon_ = CatalogoVacunas::validar(vacuna_, c) ? c : vacuna_;
}

void Dosis::setVacuna(const string& v) { vacuna_ = v; actualizar(); }

bool Dosis::mismaVacuna(const string& otra) const { return vclave_ == Texto::clave(otra); }

string Dosis::str() const { return vacuna_ + ", " + fecha_.str(); }

bool Dosis::parsearLista(const string& sec, vector<Dosis>& out, string& err) {
    string t = Texto::trim(sec);
    if (t.empty()) return true;
    auto tk = Texto::dividir(t, ',');
    while (!tk.empty() && Texto::trim(tk.back()).empty()) tk.pop_back();
    if (tk.size() % 2) { err = "lista de vacunas incompleta"; return false; }
    for (size_t i = 0; i < tk.size(); i += 2) {
        string v = Texto::trim(tk[i]);   // se conserva tal cual está en el archivo
        Fecha f;
        if (v.empty() || !Fecha::parsear(tk[i + 1], f)) { err = "vacuna/fecha inválida: " + v; return false; }
        out.emplace_back(v, f);
    }
    return true;
}

string Dosis::listaStr(const vector<Dosis>& v) {
    string r;
    for (size_t i = 0; i < v.size(); i++) r += (i ? ", " : "") + v[i].str();
    return r;
}