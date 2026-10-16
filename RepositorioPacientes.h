// RepositorioPacientes.h - Colección de pacientes en memoria + archivo de texto
#pragma once
#include <algorithm>
#include <map>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include "Paciente.h"
#include "Paralelo.h"

class RepositorioPacientes {
    std::string archivo_;
    std::vector<Paciente> pacientes_;                        // orden original del archivo
    std::vector<std::pair<size_t, std::string>> invalidas_;  // líneas no interpretables (se conservan)
    mutable std::mutex mtx_;                                 // protege resultados compartidos entre hilos

    void ordenarAlfa(std::vector<size_t>& idx) const;
public:
    explicit RepositorioPacientes(std::string archivo);

    size_t size() const { return pacientes_.size(); }
    Paciente& at(size_t i) { return pacientes_[i]; }
    const Paciente& at(size_t i) const { return pacientes_[i]; }
    void agregar(const Paciente& p);
    void eliminar(size_t i);

    // Búsqueda paralela: devuelve índices en orden alfabético
    template <class Pred>
    std::vector<size_t> buscar(Pred pred) const {
        std::vector<size_t> res;
        Paralelo::rango(pacientes_.size(), [&](size_t b, size_t e) {
            std::vector<size_t> loc;
            for (size_t i = b; i < e; i++) if (pred(pacientes_[i])) loc.push_back(i);
            std::lock_guard<std::mutex> lk(mtx_);
            res.insert(res.end(), loc.begin(), loc.end());
        });
        ordenarAlfa(res);
        return res;
    }
    std::vector<size_t> todos() const;

    // Conteos paralelos
    int contarAplicadas(long long lo, long long hi, std::map<std::string, int>& porVac) const;
    int contarProximas(const std::string& v, long long lo, long long hi) const;

    // Persistencia
    void cargar();
    void guardar();
};
