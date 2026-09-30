#ifndef TABLA_COORDENADA_H
#define TABLA_COORDENADA_H

#include "../registro.h"

// Struct REAL de la tabla `coordenadas` (columnas tal cual transporte.sql).
typedef struct {
    int    id_coordenada;   // <-- clave de indexación por defecto (PK real)
    double latitud;
    double longitud;
    int    id_ruta;
    char   sentido[8];      // "IDA" o "VUELTA"
    int    id_paradero;     // -1 si en la tabla original es NULL
    int    orden;           // orden del punto dentro de la ruta
} Coordenada;

// Convierte una fila real de `coordenadas` al Registro genérico.
//
// OJO: la clave que usa el ISAM para ordenar/indexar es `id_coordenada`.
// Si en vez de eso quieren hacer rangos por `orden` dentro de una ruta
// (p.ej. "dame los puntos con orden entre 10 y 30 de la ruta 1"), cambien
// la línea `r.clave = c.id_coordenada;` por `r.clave = c.orden;` en el
// .c — pero entonces el archivo quedaría ordenado/indexado por orden
// GLOBAL, mezclando rutas distintas si `orden` se repite entre rutas.
// Para ese caso, lo más simple es tener un archivo .dat por ruta, o usar
// una clave compuesta tipo `id_ruta * 10000 + orden`.
Registro coordenada_a_registro(Coordenada c);
Coordenada registro_a_coordenada(Registro r);

#endif
