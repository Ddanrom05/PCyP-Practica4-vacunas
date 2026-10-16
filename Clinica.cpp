#include "Clinica.h"
#include <iostream>
using namespace std;

Clinica::Clinica(const string& archivo) : repo_(archivo), ctx_{repo_, io_, Fecha()} {
    ops_.push_back(make_unique<RegistrarPaciente>(ctx_));
    ops_.push_back(make_unique<RegistrarAplicacion>(ctx_));
    ops_.push_back(make_unique<AgregarProxima>(ctx_));
    ops_.push_back(make_unique<EliminarPaciente>(ctx_));
    ops_.push_back(make_unique<ModificarPaciente>(ctx_));
    ops_.push_back(make_unique<MostrarPaciente>(ctx_));
    ops_.push_back(make_unique<ModificarVacuna>(ctx_));
    ops_.push_back(make_unique<ListarNombres>(ctx_));
    ops_.push_back(make_unique<ListarCompleto>(ctx_));
    ops_.push_back(make_unique<BuscarPorEdad>(ctx_));
    ops_.push_back(make_unique<ProximosSieteDias>(ctx_));
    ops_.push_back(make_unique<AplicadasUltimaSemana>(ctx_));
    ops_.push_back(make_unique<ProximasSiguienteMes>(ctx_));
    ops_.push_back(make_unique<VacunadosMesActual>(ctx_));
}

void Clinica::mostrarMenu() const {
    cout << "\n=========== CLÍNICA (hoy: " << ctx_.hoy.str() << ") ===========\n";
    for (size_t i = 0; i < ops_.size(); i++)
        cout << (i + 1 < 10 ? " " : "") << i + 1 << ". " << ops_[i]->titulo() << "\n";
    cout << " 0. Cerrar clínica (guardar y salir)\n";
}

void Clinica::ejecutar() {
    try {
        ctx_.hoy = io_.leerFecha("Fecha actual de trabajo (dd/mm/aaaa): ");
        repo_.cargar();
        while (true) {
            mostrarMenu();
            int op = io_.leerEntero("Opción: ", 0, (int)ops_.size());
            if (op == 0) break;
            cout << "\n";
            ops_[op - 1]->ejecutar();   // polimorfismo
        }
    } catch (const exception& e) {
        cout << "\n[Entrada finalizada: " << e.what() << "]\n";
    }
    repo_.guardar();   // cierre: nuevos al final, modificados actualizados, orden original
}