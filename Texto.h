// Texto.h - Utilidades de texto (métodos estáticos)
#pragma once
#include <string>
#include <vector>

class Texto {
public:
    static std::string trim(const std::string& s);
    static std::vector<std::string> dividir(const std::string& s, char sep);
    static std::string minusculas(const std::string& s);          // soporta acentos UTF-8
    static std::string quitarAcentos(const std::string& s);
    static std::string limpiarEspacios(const std::string& s);      // colapsa espacios, deja "a, b"
    static std::string clave(const std::string& s);                // para comparar/ordenar
    static bool nombreValido(const std::string& n);
};