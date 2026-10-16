// Fecha.h - Clase Fecha (validación, comparación y aritmética)
#pragma once
#include <string>

class Fecha {
    int d_ = 1, m_ = 1, a_ = 1970;
public:
    Fecha() = default;
    Fecha(int d, int m, int a);

    int dia() const { return d_; }
    int mes() const { return m_; }
    int anio() const { return a_; }

    static bool bisiesto(int a);
    static int diasMes(int m, int a);
    static bool parsear(const std::string& s, Fecha& f);   // "dd/mm/aaaa"

    bool valida() const;
    long long dias() const;                // días desde 1970-01-01
    std::string str() const;               // "dd/mm/aaaa"
    Fecha sumarMes() const;
    int mesesHasta(const Fecha& ref) const;  // this = nacimiento: meses cumplidos hasta ref
    bool mismoMes(const Fecha& o) const;

    bool operator==(const Fecha& o) const;
    bool operator<(const Fecha& o) const;
    bool operator<=(const Fecha& o) const;
    bool operator>(const Fecha& o) const;
    bool operator>=(const Fecha& o) const;
};