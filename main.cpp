// Práctica 4 - Programación Concurrente y Paralela (POO con .h / .cpp)
// Compilar con: make      Ejecutar: ./practica4 [archivo.txt]
#include "Clinica.h"

int main(int argc, char** argv) {
    Clinica clinica(argc > 1 ? argv[1] : "pacientes.txt");
    clinica.ejecutar();
    return 0;
}