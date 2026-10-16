// Operaciones.h - Patrón Comando: cada opción del menú es una clase derivada de Operacion
#pragma once
#include <string>
#include <vector>
#include "Consola.h"
#include "Fecha.h"
#include "Paciente.h"
#include "RepositorioPacientes.h"

// Contexto compartido por todas las operaciones
struct Contexto {
    RepositorioPacientes& repo;
    Consola& io;
    Fecha hoy;
};

// ----------------------------- Clase base abstracta -----------------------------
class Operacion {
protected:
    Contexto& c_;

    std::vector<size_t> buscarPorNombre();
    int seleccionar(const std::vector<size_t>& idx);          // índice o -1
    bool fechaDosisOk(const Paciente& p, const Fecha& f, bool aplicada);
    void capturarDosis(Paciente& p, bool aplicada);
public:
    explicit Operacion(Contexto& c) : c_(c) {}
    virtual ~Operacion() = default;
    virtual std::string titulo() const = 0;
    virtual void ejecutar() = 0;
};

// Declaración de las 14 operaciones derivadas
#define DECLARAR_OPERACION(Nombre)                         \
    class Nombre : public Operacion {                      \
    public:                                                \
        using Operacion::Operacion;                        \
        std::string titulo() const override;               \
        void ejecutar() override;                          \
    };

DECLARAR_OPERACION(RegistrarPaciente)       // 1
DECLARAR_OPERACION(RegistrarAplicacion)     // 2
DECLARAR_OPERACION(AgregarProxima)          // 3
DECLARAR_OPERACION(EliminarPaciente)        // 4
DECLARAR_OPERACION(ModificarPaciente)       // 5
DECLARAR_OPERACION(MostrarPaciente)         // 6
DECLARAR_OPERACION(ModificarVacuna)         // 7
DECLARAR_OPERACION(ListarNombres)           // 8
DECLARAR_OPERACION(ListarCompleto)          // 9
DECLARAR_OPERACION(BuscarPorEdad)           // 10
DECLARAR_OPERACION(ProximosSieteDias)       // 11
DECLARAR_OPERACION(AplicadasUltimaSemana)   // 12
DECLARAR_OPERACION(ProximasSiguienteMes)    // 13
DECLARAR_OPERACION(VacunadosMesActual)      // 14

#undef DECLARAR_OPERACION