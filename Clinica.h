// Clinica.h - Coordina repositorio, consola y operaciones
#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Consola.h"
#include "Operaciones.h"
#include "RepositorioPacientes.h"

class Clinica {
    RepositorioPacientes repo_;
    Consola io_;
    Contexto ctx_;
    std::vector<std::unique_ptr<Operacion>> ops_;

    void mostrarMenu() const;
public:
    explicit Clinica(const std::string& archivo);
    void ejecutar();   // abre la clínica, atiende el menú y la cierra guardando los datos
};