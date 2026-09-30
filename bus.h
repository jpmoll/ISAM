#ifndef TABLA_BUS_H
#define TABLA_BUS_H

#include "../registro.h"

#define TAM_PLACA 11

// Struct REAL de la tabla `bus` (columnas tal cual transporte.sql).
// Esto es lo único que cambia si en vez de `bus` usan `viaje`, `persona`, etc.
typedef struct {
    int  id_bus;              // <-- clave primaria / clave de indexación
    char placa[TAM_PLACA];
    int  capacidad;
    int  id_estado;
    int  id_empresa;
    int  id_modelo;
} Bus;

// Convierte una fila real de `bus` al Registro genérico que usa el motor.
Registro bus_a_registro(Bus b);

// Convierte un Registro genérico de vuelta a una fila real de `bus`.
// (Esta misma lógica es la que luego van a reutilizar para armar el
// INSERT/leer el resultado de una fila al integrar con PostgreSQL vía libpq).
Bus registro_a_bus(Registro r);

#endif
