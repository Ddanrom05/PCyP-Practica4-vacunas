// Vacuna.h - CatalogoVacunas (validación de nombres) y Dosis (vacuna + fecha)
#pragma once
#include <string>
#include <vector>
#include "Fecha.h"

class CatalogoVacunas {
public:
    static const std::vector<std::string>& lista();
    // Valida sin distinguir mayúsculas/acentos y devuelve el nombre canónico
    static bool validar(const std::string& entrada, std::string& canon);
};

class Dosis {
    std::string vacuna_;          // tal como se escribió (se conserva sin cambios)
    std::string vclave_, canon_;  // clave de comparación y nombre canónico (solo para comparar/reportar)
    Fecha fecha_;
    void actualizar();
public:
    Dosis() = default;
    Dosis(std::string v, Fecha f);

    const std::string& vacuna() const { return vacuna_; }
    const Fecha& fecha() const { return fecha_; }
    void setVacuna(const std::string& v);
    // Compara sin distinguir mayúsculas ni acentos
    bool mismaVacuna(const std::string& otra) const;
    const std::string& canonica() const { return canon_; }   // nombre del catálogo (para reportes)
    void setFecha(const Fecha& f) { fecha_ = f; }
    std::string str() const;   // "vacuna, dd/mm/aaaa"

    // "vac, fecha, vac, fecha" -> vector<Dosis>
    static bool parsearLista(const std::string& sec, std::vector<Dosis>& out, std::string& err);
    static std::string listaStr(const std::vector<Dosis>& v);
};