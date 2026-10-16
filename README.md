# Práctica 4 – Grafos de dependencia y bloques COBEGIN–COEND

Compilar: `g++ -std=c++17 -O2 -pthread practica4.cpp -o practica4`

Recursos compartidos y su protección:

| Recurso | Acceso concurrente | Protección |
|---|---|---|
| `pacientes` (vector) durante búsquedas | Solo lectura por varios hilos | No requiere exclusión: los datos no se modifican mientras hay hilos activos (todas las modificaciones ocurren en el hilo principal, después del `join`) |
| Vector de resultados `res` | Escritura de varios hilos | `mutex mtxRes` + `lock_guard` |
| `total`, `porVac` (op. 12) | Acumulación de varios hilos | `mutex mtxRes` + `lock_guard` (reducción local → global) |
| `total` (op. 13) | Acumulación de varios hilos | `atomic<int>` |
| `errores` (carga) | Escritura de varios hilos | `mutex mtxRes` + `unique_lock` |
| `tmp[i]`, `ok[i]`, `lineas[i]` (carga/guardado) | Cada hilo escribe posiciones distintas | Sin exclusión (particionado disjunto) |

---

## 1. Flujo general del programa

```
S0: leer fecha actual
S1: cargar archivo (concurrente, ver §2)
S2: ciclo de menú (secuencial, interacción con el usuario)
      └─ cada operación de búsqueda/consulta usa §3, §4 o §5
S3: guardar archivo (concurrente, ver §6)
```

```mermaid
graph LR
  S0[Leer fecha actual] --> S1[Cargar archivo]
  S1 --> S2[Menú / operaciones]
  S2 --> S2
  S2 --> S3[Guardar archivo]
```

---

## 2. Carga del archivo (`cargar`)

Grafo de dependencia:

```mermaid
graph TD
  A[Leer líneas del archivo] --> P1[Parsear bloque 1]
  A --> P2[Parsear bloque 2]
  A --> P3[Parsear bloque k]
  P1 --> C[Compactar en orden original + reportar errores]
  P2 --> C
  P3 --> C
```

Bloque concurrente:

```
S1: leer todas las líneas a  lineas[0..n-1]
COBEGIN
    Hilo_1: for i in [0, n/k)         parsear(lineas[i]) -> tmp[i], ok[i]
    Hilo_2: for i in [n/k, 2n/k)      parsear(lineas[i]) -> tmp[i], ok[i]
    ...
    Hilo_k: for i in [(k-1)n/k, n)    parsear(lineas[i]) -> tmp[i], ok[i]
COEND
S3: recorrer tmp en orden y construir 'pacientes' (conserva orden original)
```

Los hilos escriben posiciones distintas (sin condición de carrera); los errores se agregan con `unique_lock(mtxRes)`.

---

## 3. Búsqueda paralela genérica (`buscarIdx`)

Usada en las operaciones **2, 3, 4, 5, 6, 7** (buscar paciente), **8, 9** (listar), **10, 11, 14** (filtros) y en validación de duplicados (op. 1).

```mermaid
graph TD
  I[Dividir pacientes en k bloques] --> B1[Hilo 1: filtrar bloque 1]
  I --> B2[Hilo 2: filtrar bloque 2]
  I --> B3[Hilo k: filtrar bloque k]
  B1 --> M1[lock mtxRes: mezclar locales]
  B2 --> M1
  B3 --> M1
  M1 --> O[Ordenar alfabéticamente]
  O --> R[Mostrar / seleccionar]
```

```
S0: dividir [0,N) en k bloques
COBEGIN
    Hilo_j (j=1..k):
        loc = {}
        for i in bloque_j: if pred(pacientes[i]) loc.push(i)
        lock_guard(mtxRes);  res.insert(loc);     // sección crítica corta
COEND
S2: ordenar(res)  por nombre (sin acentos/mayúsculas)
S3: mostrar resultados
```

---

## 4. Conteos paralelos (operaciones 12 y 13)

```mermaid
graph TD
  I[Dividir pacientes en k bloques] --> C1[Hilo 1: contar]
  I --> C2[Hilo 2: contar]
  I --> C3[Hilo k: contar]
  C1 --> T[Acumular total: mutex / atomic]
  C2 --> T
  C3 --> T
  T --> R[Mostrar total]
```

```
S0: lo = hoy - 7 (op.12)  |  lo = hoy, hi = hoy + 1 mes (op.13)
COBEGIN
    Hilo_j: loc = contar dosis en rango dentro de su bloque
            // op.12: lock_guard(mtxRes); total += loc; porVac += mapa_local
            // op.13: total += loc           (atomic<int>)
COEND
S2: imprimir total
```

---

## 5. Operaciones de modificación (1–7)

Se ejecutan en el hilo principal porque dependen de la entrada del usuario y modifican `pacientes`. La **parte de búsqueda** sí es concurrente (§3).

```mermaid
graph LR
  E[Leer criterio] --> B[Búsqueda paralela §3] --> S[Seleccionar paciente] --> V[Validar datos] --> M[Modificar] 
```

Dependencia: `Leer → Buscar → Seleccionar → Validar → Modificar` (cadena estrictamente secuencial; la modificación no puede iniciarse antes de terminar la búsqueda).

---

## 6. Guardado (`guardar`)

```mermaid
graph TD
  I[Dividir pacientes en k bloques] --> F1[Hilo 1: formatear líneas]
  I --> F2[Hilo 2: formatear líneas]
  I --> F3[Hilo k: formatear líneas]
  F1 --> W[Insertar líneas no interpretables + escribir archivo secuencial]
  F2 --> W
  F3 --> W
```

```
COBEGIN
    Hilo_j: for i in bloque_j: lineas[i] = formatearLinea(pacientes[i])
COEND
S2: insertar líneas inválidas conservadas
S3: escribir lineas[] en el archivo (orden original; nuevos al final)
```

---

## 7. Dependencias entre operaciones del menú

```mermaid
graph TD
  O1[1 Registrar paciente] --> O2[2 Aplicar vacuna]
  O1 --> O3[3 Agregar próxima]
  O2 --> O7[7 Modificar/eliminar vacuna]
  O3 --> O7
  O2 --> O12[12 Vacunas última semana]
  O2 --> O14[14 Vacunados en el mes]
  O3 --> O11[11 Próximos 7 días]
  O3 --> O13[13 Próximo mes]
  O1 --> O8[8/9 Listados]
  O1 --> O10[10 Intervalo de edades]
  O4[4 Eliminar] --> O8
  O5[5 Modificar paciente] --> O8
```

Las operaciones de consulta (8–14) no dependen entre sí y podrían ejecutarse simultáneamente (todas son de solo lectura); las de modificación (1–5, 7) se serializan por naturaleza al ser interactivas.

---

## 8. Notas de formato del archivo

```
Nombre, Apellido1, Apellido2; dd/mm/aaaa; vacuna, fecha, vacuna, fecha; vacuna, fecha.
```

- Una línea por paciente. Sección de aplicadas o próximas vacía → `; .` / `; ;`.
- Los datos NO se normalizan: se conservan tal como están en el archivo (mayúsculas, acentos, espacios). Búsquedas, comparaciones y validación de vacunas ignoran mayúsculas y acentos. Los pacientes sin cambios se reescriben idénticos.
- Las líneas con formato inválido se conservan sin cambios al guardar.