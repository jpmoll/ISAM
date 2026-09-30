#ifndef CONFIG_H
#define CONFIG_H

// Tamaño de página en bytes: es la unidad de I/O contra disco (se lee y
// escribe una página entera). Usamos el tamaño típico de bloque del SO;
// si quieren igualar a PostgreSQL, usen 8192.
#define TAM_PAGINA 4096

// Tamaño máximo (en bytes) que puede ocupar la porción de datos de UN
// registro en memoria (el tipo Registro es fijo en C, así que necesita un
// techo). Súbanlo si tienen una tabla con strings largos (`persona`, por
// ejemplo). Lo que realmente se escribe a disco por registro es más chico
// (ver datos.c): sizeof(int) + registro_tam_actual(), no este máximo.
#define TAM_REGISTRO_MAX 256

// Nombres de archivo estándar DENTRO de la carpeta de cada tabla, p.ej.
// "data/coordenadas/datos.dat". La carpeta la arma quien integra (main.c),
// una por tabla — ISAM es una estructura por relación, no un archivo
// mezclando varias tablas (así trabaja PostgreSQL también: 1 archivo = 1
// tabla, dividido en páginas).
#define ARCHIVO_DATOS     "datos.dat"
#define ARCHIVO_INDICE    "indice.dat"
#define ARCHIVO_OVERFLOW  "overflow.dat"

#endif
