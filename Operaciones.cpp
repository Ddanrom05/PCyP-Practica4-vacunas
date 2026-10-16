#include "Operaciones.h"
#include <iostream>
#include <map>
#include "Texto.h"
using namespace std;

// ================================ Base ================================
vector<size_t> Operacion::buscarPorNombre() {
    string k = Texto::clave(c_.io.leer("Nombre del paciente (o parte): "));
    if (k.empty()) return {};
    return c_.repo.buscar([&](const Paciente& p) { return p.coincideNombre(k); });
}

int Operacion::seleccionar(const vector<size_t>& idx) {
    if (idx.empty()) { cout << "  No se encontraron pacientes.\n"; return -1; }
    if (idx.size() == 1) return (int)idx[0];
    cout << "  Se encontraron " << idx.size() << " coincidencias:\n";
    for (size_t i = 0; i < idx.size(); i++)
        cout << "   " << i + 1 << ") " << c_.repo.at(idx[i]).nombre() << " ("
             << c_.repo.at(idx[i]).nacimiento().str() << ")\n";
    int k = c_.io.leerEntero("  Elija número: ", 1, (int)idx.size());
    return (int)idx[k - 1];
}

bool Operacion::fechaDosisOk(const Paciente& p, const Fecha& f, bool aplicada) {
    if (aplicada) {
        if (f < p.nacimiento()) { cout << "  La fecha es anterior al nacimiento.\n"; return false; }
        if (f > c_.hoy) { cout << "  Una vacuna aplicada no puede tener fecha futura.\n"; return false; }
    } else if (f < c_.hoy) {
        cout << "  La fecha de una vacuna próxima no puede ser anterior a hoy (" << c_.hoy.str() << ").\n";
        return false;
    }
    return true;
}

void Operacion::capturarDosis(Paciente& p, bool aplicada) {
    string v = c_.io.leerVacuna("  Nombre de la vacuna: ");
    Fecha f;
    do { f = c_.io.leerFecha("  Fecha (dd/mm/aaaa): "); } while (!fechaDosisOk(p, f, aplicada));
    if (aplicada) p.agregarAplicada(Dosis(v, f)); else p.agregarProxima(Dosis(v, f));
}

// ============================ 1. Registrar paciente ============================
string RegistrarPaciente::titulo() const { return "Registrar nuevo paciente"; }
void RegistrarPaciente::ejecutar() {
    string n = c_.io.leerNombre("Nombre completo (apellidos separados por coma): ");
    Fecha f = c_.io.leerFecha("Fecha de nacimiento (dd/mm/aaaa): ");
    if (f > c_.hoy) { cout << "  La fecha de nacimiento no puede ser futura.\n"; return; }
    string k = Texto::clave(n);
    if (!c_.repo.buscar([&](const Paciente& p) { return p.clave() == k && p.nacimiento() == f; }).empty()) {
        cout << "  Ya existe un paciente con ese nombre y fecha de nacimiento.\n";
        return;
    }
    Paciente p(n, f);
    while (c_.io.confirmar("¿Agregar vacuna aplicada?")) capturarDosis(p, true);
    while (c_.io.confirmar("¿Agregar vacuna próxima?")) capturarDosis(p, false);
    c_.repo.agregar(p);
    cout << "  Paciente registrado.\n";
}

// ============================ 2. Registrar aplicación ============================
string RegistrarAplicacion::titulo() const { return "Registrar aplicación de vacuna"; }
void RegistrarAplicacion::ejecutar() {
    int i = seleccionar(buscarPorNombre());
    if (i < 0) return;
    Paciente& p = c_.repo.at(i);
    string v = c_.io.leerVacuna("  Vacuna aplicada: ");
    Fecha f;
    do { f = c_.io.leerFecha("  Fecha de aplicación (Enter = hoy): ", &c_.hoy); } while (!fechaDosisOk(p, f, true));
    if (p.tieneAplicada(v, f)) { cout << "  Esa aplicación ya estaba registrada.\n"; return; }
    p.agregarAplicada(Dosis(v, f));
    if (p.retirarProximaMasCercana(v)) cout << "  Se retiró de las vacunas próximas.\n";
    cout << "  Aplicación registrada.\n";
}

// ============================ 3. Agregar próxima ============================
string AgregarProxima::titulo() const { return "Agregar vacuna próxima"; }
void AgregarProxima::ejecutar() {
    int i = seleccionar(buscarPorNombre());
    if (i < 0) return;
    capturarDosis(c_.repo.at(i), false);
    cout << "  Vacuna próxima agregada.\n";
}

// ============================ 4. Eliminar paciente ============================
string EliminarPaciente::titulo() const { return "Eliminar paciente"; }
void EliminarPaciente::ejecutar() {
    int i = seleccionar(buscarPorNombre());
    if (i < 0) return;
    c_.repo.at(i).mostrar(cout, c_.hoy);
    if (c_.io.confirmar("¿Eliminar este paciente?")) {
        c_.repo.eliminar(i);
        cout << "  Paciente eliminado.\n";
    }
}

// ============================ 5. Modificar paciente ============================
string ModificarPaciente::titulo() const { return "Modificar nombre o fecha de nacimiento"; }
void ModificarPaciente::ejecutar() {
    cout << "Buscar por: 1) Nombre  2) Fecha de nacimiento\n";
    vector<size_t> idx;
    if (c_.io.leerEntero("Opción: ", 1, 2) == 1) idx = buscarPorNombre();
    else {
        Fecha f = c_.io.leerFecha("Fecha de nacimiento a buscar: ");
        idx = c_.repo.buscar([&](const Paciente& p) { return p.nacimiento() == f; });
    }
    int i = seleccionar(idx);
    if (i < 0) return;
    Paciente& p = c_.repo.at(i);
    p.mostrar(cout, c_.hoy);
    cout << "Modificar: 1) Nombre  2) Fecha de nacimiento\n";
    if (c_.io.leerEntero("Opción: ", 1, 2) == 1) p.setNombre(c_.io.leerNombre("Nuevo nombre: "));
    else {
        Fecha f = c_.io.leerFecha("Nueva fecha de nacimiento: ");
        if (f > c_.hoy) { cout << "  La fecha no puede ser futura.\n"; return; }
        for (auto& d : p.aplicadas())
            if (d.fecha() < f) { cout << "  Hay vacunas aplicadas anteriores a esa fecha.\n"; return; }
        p.setNacimiento(f);
    }
    cout << "  Datos actualizados.\n";
}

// ============================ 6. Mostrar paciente ============================
string MostrarPaciente::titulo() const { return "Mostrar información de paciente"; }
void MostrarPaciente::ejecutar() {
    auto idx = buscarPorNombre();
    if (idx.empty()) { cout << "  No se encontraron pacientes.\n"; return; }
    for (size_t i : idx) { cout << "  ----------------------------\n"; c_.repo.at(i).mostrar(cout, c_.hoy); }
}

// ============================ 7. Modificar/eliminar vacuna ============================
string ModificarVacuna::titulo() const { return "Modificar o eliminar vacuna de un paciente"; }
void ModificarVacuna::ejecutar() {
    int i = seleccionar(buscarPorNombre());
    if (i < 0) return;
    Paciente& p = c_.repo.at(i);
    struct Ref { bool ap; size_t pos; };
    vector<Ref> rs;
    for (size_t k = 0; k < p.aplicadas().size(); k++) rs.push_back({true, k});
    for (size_t k = 0; k < p.proximas().size(); k++) rs.push_back({false, k});
    if (rs.empty()) { cout << "  El paciente no tiene vacunas registradas.\n"; return; }
    for (size_t k = 0; k < rs.size(); k++) {
        const Dosis& d = rs[k].ap ? p.aplicadas()[rs[k].pos] : p.proximas()[rs[k].pos];
        cout << "   " << k + 1 << ") [" << (rs[k].ap ? "Aplicada" : "Próxima ") << "] "
             << d.vacuna() << " - " << d.fecha().str() << "\n";
    }
    Ref r = rs[c_.io.leerEntero("Número de vacuna: ", 1, (int)rs.size()) - 1];
    vector<Dosis>& lista = r.ap ? p.aplicadas() : p.proximas();
    cout << "1) Modificar nombre  2) Modificar fecha  3) Eliminar\n";
    int a = c_.io.leerEntero("Opción: ", 1, 3);
    if (a == 1) lista[r.pos].setVacuna(c_.io.leerVacuna("Nueva vacuna: "));
    else if (a == 2) {
        while (true) {
            Fecha f = c_.io.leerFecha("Nueva fecha: ");
            if (fechaDosisOk(p, f, r.ap)) { lista[r.pos].setFecha(f); break; }
        }
    } else if (c_.io.confirmar("¿Eliminar esta vacuna?")) lista.erase(lista.begin() + r.pos);
    else return;
    cout << "  Operación realizada.\n";
}

// ============================ 8. Listar nombres ============================
string ListarNombres::titulo() const { return "Pacientes en orden alfabético (nombres)"; }
void ListarNombres::ejecutar() {
    auto idx = c_.repo.todos();
    if (idx.empty()) { cout << "  No hay pacientes.\n"; return; }
    int n = 1;
    for (size_t i : idx) cout << "  " << n++ << ". " << c_.repo.at(i).nombre() << "\n";
}

// ============================ 9. Listar completo (5 en 5) ============================
string ListarCompleto::titulo() const { return "Pacientes en orden alfabético (toda la información, de 5 en 5)"; }
void ListarCompleto::ejecutar() {
    auto idx = c_.repo.todos();
    if (idx.empty()) { cout << "  No hay pacientes.\n"; return; }
    for (size_t k = 0; k < idx.size(); k++) {
        cout << "  ---------------- Paciente " << k + 1 << " de " << idx.size() << " ----------------\n";
        c_.repo.at(idx[k]).mostrar(cout, c_.hoy);
        if ((k + 1) % 5 == 0 && k + 1 < idx.size()) {
            string s = c_.io.leer("  [Enter = siguientes 5, q = salir] ");
            if (!s.empty() && (s[0] == 'q' || s[0] == 'Q')) return;
        }
    }
}

// ============================ 10. Intervalo de edades ============================
string BuscarPorEdad::titulo() const { return "Buscar pacientes por intervalo de edades"; }
void BuscarPorEdad::ejecutar() {
    int mn = c_.io.leerEntero("Edad mínima (años): ", 0, 120);
    int mx = c_.io.leerEntero("Edad máxima (años): ", 0, 120);
    if (mn > mx) swap(mn, mx);
    Fecha hoy = c_.hoy;
    auto idx = c_.repo.buscar([&](const Paciente& p) {
        int e = p.edadAnios(hoy);
        return e >= mn && e <= mx;
    });
    if (idx.empty()) { cout << "  Ningún paciente en ese intervalo.\n"; return; }
    for (size_t i : idx) {
        const Paciente& p = c_.repo.at(i);
        cout << "  " << p.nombre() << " - " << p.nacimiento().str() << " - " << p.edadStr(hoy) << "\n";
    }
}

// ============================ 11. Próximos 7 días ============================
string ProximosSieteDias::titulo() const { return "Pacientes con una vacuna en los próximos 7 días"; }
void ProximosSieteDias::ejecutar() {
    string v = c_.io.leerVacuna("Vacuna: ");
    long long lo = c_.hoy.dias(), hi = lo + 7;
    auto idx = c_.repo.buscar([&](const Paciente& p) { return !p.proximasEntre(v, lo, hi).empty(); });
    if (idx.empty()) { cout << "  Ningún paciente tiene '" << v << "' programada en los próximos 7 días.\n"; return; }
    for (size_t i : idx)
        for (auto& d : c_.repo.at(i).proximasEntre(v, lo, hi))
            cout << "  " << c_.repo.at(i).nombre() << " - " << d.fecha().str() << "\n";
}

// ============================ 12. Aplicadas última semana ============================
string AplicadasUltimaSemana::titulo() const { return "Vacunas aplicadas en la última semana"; }
void AplicadasUltimaSemana::ejecutar() {
    map<string, int> porVac;
    int total = c_.repo.contarAplicadas(c_.hoy.dias() - 7, c_.hoy.dias(), porVac);
    cout << "  Vacunas aplicadas en la última semana: " << total << "\n";
    for (auto& kv : porVac) cout << "    - " << kv.first << ": " << kv.second << "\n";
}

// ============================ 13. Próximas siguiente mes ============================
string ProximasSiguienteMes::titulo() const { return "Cantidad de una vacuna próxima (siguiente mes)"; }
void ProximasSiguienteMes::ejecutar() {
    string v = c_.io.leerVacuna("Vacuna: ");
    Fecha fin = c_.hoy.sumarMes();
    int n = c_.repo.contarProximas(v, c_.hoy.dias(), fin.dias());
    cout << "  Dosis de '" << v << "' por aplicar entre " << c_.hoy.str() << " y " << fin.str() << ": " << n << "\n";
}

// ============================ 14. Vacunados en el mes actual ============================
string VacunadosMesActual::titulo() const { return "Pacientes vacunados en el mes actual"; }
void VacunadosMesActual::ejecutar() {
    Fecha hoy = c_.hoy;
    auto idx = c_.repo.buscar([&](const Paciente& p) { return !p.aplicadasEnMes(hoy).empty(); });
    if (idx.empty()) { cout << "  Ningún paciente se vacunó en el mes actual.\n"; return; }
    for (size_t i : idx) {
        const Paciente& p = c_.repo.at(i);
        cout << "  " << p.nombre() << " | Edad: " << p.edadStr(hoy) << "\n";
        for (auto& d : p.aplicadasEnMes(hoy)) cout << "      - " << d.vacuna() << " (" << d.fecha().str() << ")\n";
    }
}