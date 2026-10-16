// Consola.h - Entrada/salida validada
#pragma once
#include <string>
#include "Fecha.h"

class Consola {
public:
    std::string leer(const std::string& msg) const;
    int leerEntero(const std::string& msg, int mn, int mx) const;
    // Si 'porDefecto' no es nulo, Enter devuelve ese valor
    Fecha leerFecha(const std::string& msg, const Fecha* porDefecto = nullptr) const;
    std::string leerVacuna(const std::string& msg) const;
    std::string leerNombre(const std::string& msg) const;
    bool confirmar(const std::string& msg) const;
};