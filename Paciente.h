// Paciente.h - Clase Paciente (encapsula datos y consultas sobre sus vacunas)
#pragma once
#include <iosfwd>
#include <map>
#include <string>
#include <vector>
#include "Fecha.h"
#include "Vacuna.h"

class Paciente {
    std::string nombre_, clave_;
    Fecha nac_;
    std::vector<Dosis> aplicadas_, proximas_;
    std::string original_, firma_;   // línea original del archivo y su reconstrucción (para detectar cambios)
public:
    Paciente() = default;
    Paciente(const std::string& nombre, const Fecha& nac);

    // ---- acceso ----
    const std::string& nombre() const { return nombre_; }
    const std::string& clave() const { return clave_; }
    const Fecha& nacimiento() const { return nac_; }
    void setNombre(const std::string& n);
    void setNacimiento(const Fecha& f) { nac_ = f; }
    std::vector<Dosis>& aplicadas() { return aplicadas_; }
    std::vector<Dosis>& proximas() { return proximas_; }
    const std::vector<Dosis>& aplicadas() const { return aplicadas_; }
    const std::vector<Dosis>& proximas() const { return proximas_; }

    // ---- edad ----
    int edadMeses(const Fecha& hoy) const;
    int edadAnios(const Fecha& hoy) const;
    std::string edadStr(const Fecha& hoy) const;

    // ---- vacunas ----
    void agregarAplicada(const Dosis& d) { aplicadas_.push_back(d); }
    void agregarProxima(const Dosis& d) { proximas_.push_back(d); }
    bool tieneAplicada(const std::string& v, const Fecha& f) const;
    bool retirarProximaMasCercana(const std::string& v);

    // ---- consultas (solo lectura: seguras para hilos) ----
    bool coincideNombre(const std::string& k) const;
    int contarAplicadasEntre(long long lo, long long hi, std::map<std::string, int>* porVac = nullptr) const;
    std::vector<Dosis> proximasEntre(const std::string& v, long long lo, long long hi) const;
    std::vector<Dosis> aplicadasEnMes(const Fecha& hoy) const;

    // ---- presentación y persistencia ----
    void mostrar(std::ostream& os, const Fecha& hoy) const;
    std::string aLinea() const;
    // Si el paciente no cambió, devuelve la línea exactamente como estaba en el archivo
    std::string lineaParaGuardar() const;
    static bool desdeLinea(const std::string& linea, Paciente& p, std::string& err);
};