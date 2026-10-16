#include "RepositorioPacientes.h"
#include <fstream>
#include <iostream>
#include "Texto.h"
using namespace std;

RepositorioPacientes::RepositorioPacientes(string archivo) : archivo_(std::move(archivo)) {}

void RepositorioPacientes::ordenarAlfa(vector<size_t>& idx) const {
    sort(idx.begin(), idx.end(), [&](size_t a, size_t b) {
        if (pacientes_[a].clave() != pacientes_[b].clave()) return pacientes_[a].clave() < pacientes_[b].clave();
        return a < b;
    });
}

void RepositorioPacientes::agregar(const Paciente& p) { pacientes_.push_back(p); }
void RepositorioPacientes::eliminar(size_t i) { pacientes_.erase(pacientes_.begin() + i); }

vector<size_t> RepositorioPacientes::todos() const {
    return buscar([](const Paciente&) { return true; });
}

int RepositorioPacientes::contarAplicadas(long long lo, long long hi, map<string, int>& porVac) const {
    int total = 0;
    Paralelo::rango(pacientes_.size(), [&](size_t b, size_t e) {
        int loc = 0;
        map<string, int> m;
        for (size_t i = b; i < e; i++) loc += pacientes_[i].contarAplicadasEntre(lo, hi, &m);
        lock_guard<mutex> lk(mtx_);
        total += loc;
        for (auto& kv : m) porVac[kv.first] += kv.second;
    });
    return total;
}

int RepositorioPacientes::contarProximas(const string& v, long long lo, long long hi) const {
    int total = 0;
    Paralelo::rango(pacientes_.size(), [&](size_t b, size_t e) {
        int loc = 0;                                   // cada hilo cuenta en una variable local
        for (size_t i = b; i < e; i++) loc += (int)pacientes_[i].proximasEntre(v, lo, hi).size();
        lock_guard<mutex> lk(mtx_);                    // sección crítica: sumar al total compartido
        total += loc;
    });
    return total;
}

// Carga: las líneas se interpretan en paralelo, el orden original se conserva
void RepositorioPacientes::cargar() {
    ifstream f(archivo_);
    if (!f) { cout << "Archivo '" << archivo_ << "' no encontrado: se creará al cerrar.\n"; return; }
    vector<string> lineas;
    string l;
    while (getline(f, l)) if (!Texto::trim(l).empty()) lineas.push_back(l);

    vector<Paciente> tmp(lineas.size());
    vector<char> ok(lineas.size(), 0);
    vector<string> errores;
    Paralelo::rango(lineas.size(), [&](size_t b, size_t e) {
        for (size_t i = b; i < e; i++) {
            string err;
            if (Paciente::desdeLinea(lineas[i], tmp[i], err)) ok[i] = 1;
            else {
                unique_lock<mutex> lk(mtx_);
                errores.push_back("Línea " + to_string(i + 1) + ": " + err);
            }
        }
    });
    for (size_t i = 0; i < lineas.size(); i++) {
        if (ok[i]) pacientes_.push_back(std::move(tmp[i]));
        else invalidas_.push_back({pacientes_.size(), lineas[i]});
    }
    sort(errores.begin(), errores.end());
    for (auto& e : errores) cout << "  [Aviso] " << e << " (la línea se conservará sin cambios)\n";
    cout << "Se cargaron " << pacientes_.size() << " pacientes.\n";
}

// Guardado: formateo en paralelo, escritura secuencial en el orden original
void RepositorioPacientes::guardar() {
    vector<string> lineas(pacientes_.size());
    Paralelo::rango(pacientes_.size(), [&](size_t b, size_t e) {
        for (size_t i = b; i < e; i++) lineas[i] = pacientes_[i].lineaParaGuardar();
    });
    size_t ins = 0;
    for (auto& inv : invalidas_) {
        size_t pos = min(inv.first + ins, lineas.size());
        lineas.insert(lineas.begin() + pos, inv.second);
        ins++;
    }
    ofstream f(archivo_, ios::trunc);
    if (!f) { cout << "ERROR: no se pudo escribir '" << archivo_ << "'.\n"; return; }
    for (auto& l : lineas) f << l << "\n";
    cout << "Datos guardados en '" << archivo_ << "' (" << lineas.size() << " registros).\n";
}