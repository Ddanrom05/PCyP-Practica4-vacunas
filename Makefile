CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -pthread
LDFLAGS  = -pthread
OBJS     = main.o Clinica.o Operaciones.o RepositorioPacientes.o Paciente.o Vacuna.o Fecha.o Consola.o Texto.o
TARGET   = practica4

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Dependencias de encabezados (recompila al cambiar un .h)
main.o Clinica.o: Clinica.h Operaciones.h RepositorioPacientes.h Paciente.h Vacuna.h Fecha.h Consola.h Paralelo.h
Operaciones.o: Operaciones.h RepositorioPacientes.h Paciente.h Vacuna.h Fecha.h Consola.h Texto.h Paralelo.h
RepositorioPacientes.o: RepositorioPacientes.h Paciente.h Vacuna.h Fecha.h Texto.h Paralelo.h
Paciente.o: Paciente.h Vacuna.h Fecha.h Texto.h
Vacuna.o: Vacuna.h Fecha.h Texto.h
Fecha.o: Fecha.h Texto.h
Consola.o: Consola.h Fecha.h Texto.h Vacuna.h
Texto.o: Texto.h

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean