# Esqueleto ISAM (archivo .dat en disco)

División en 3 módulos independientes, cada uno con su `.h` (contrato) y su
`.c` (implementación, con TODOs marcados). Nadie necesita esperar a que
otro termine para empezar: solo necesitan los `.h`.

## Genérico para cualquier tabla
El motor (`datos.c`, `indice.c`, `overflow.c`) **nunca conoce las columnas
reales** de una tabla. Solo conoce `Registro`: una `clave` (int) + un
bloque de bytes crudos.

Cada tabla real (Bus, Coordenada, Viaje...) vive en `tablas/<nombre>.c` con:
- un struct con las columnas de verdad,
- `<nombre>_a_registro()` — empaqueta esa fila a un `Registro` genérico,
- `registro_a_<nombre>()` — la desempaqueta de vuelta.

Para agregar una tabla nueva: copian `tablas/bus.h`/`tablas/bus.c` (o
`coordenada.*`), cambian el struct y los dos `memcpy`, y en `main.c`
llaman `registro_configurar_tam(sizeof(SuTabla))`. No hay que tocar
`datos.c`, `indice.c` ni `overflow.c`.

**Pensando en la integración con PostgreSQL:** estas mismas funciones de
empaquetado son las que después van a adaptar para convertir entre
`Registro` y una fila obtenida con libpq (`PQgetvalue`) o para armar el
`INSERT`/`UPDATE` correspondiente.

## Un archivo (datos+índice+overflow) por TABLA, no uno mezclando todas
ISAM está definida sobre una relación con una clave: no tiene sentido
mezclar `bus` y `coordenadas` en el mismo `datos.dat` (las claves de
tablas distintas no son comparables entre sí, y se rompería el orden
físico). Por eso `datos_crear`/`datos_abrir`, `indice_guardar`/`cargar` y
`overflow_crear`/`abrir` reciben la **ruta del archivo** como parámetro —
quien integra decide la carpeta, típicamente una por tabla:

```
data/
  bus/
    datos.dat  indice.dat  overflow.dat
  coordenadas/
    datos.dat  indice.dat  overflow.dat
```

`main.c` ya arma esas rutas con `mkdir` + `snprintf` antes de usarlas. Así
trabaja PostgreSQL también: 1 archivo = 1 tabla, dividido en páginas.

## Páginas de verdad (no un número fijo de registros)
`TAM_PAGINA` (en `config.h`) es un tamaño **en bytes** (4096 por defecto;
8192 si quieren igualar a PostgreSQL), no una cantidad de registros. El
factor de bloqueo real se calcula en tiempo de ejecución con
`datos_factor_bloque()`:

```
datos_factor_bloque() = TAM_PAGINA / (sizeof(int) + registro_tam_actual())
```

Por eso una tabla con filas chicas (`bus`) cabe más registros por página
que una con filas grandes (`persona`, con email/nombre). Como esto ya no
es fijo en tiempo de compilación, `Bloque.regs` es un arreglo dinámico:
se reserva con `datos_reservar_bloque(&bloque)` antes de leer/escribir un
bloque, y se libera con `datos_liberar_bloque(&bloque)` cuando ya no se
usa (`main.c` ya sigue ese patrón en `isam_buscar`/`insertar`/`eliminar`/
`buscar_rango`).

Ojo con Persona A al escribir a disco: no usar `sizeof(Registro)` completo
(eso escribiría siempre `TAM_REGISTRO_MAX` bytes por registro, aunque la
tabla sea chica). Hay que escribir solo `sizeof(int) + registro_tam_actual()`
bytes por registro — el comentario en `datos.c` lo detalla.

## Búsquedas por rango
`isam_buscar_rango(clave_min, clave_max, out, capacidad)` en `main.c` ya
está implementada (no es un stub): ubica el bloque de arranque con el
índice y escanea bloques consecutivos, cortando apenas un bloque empieza
más allá de `clave_max` — esa es la ventaja de tener los datos físicamente
ordenados. Lo único pendiente ahí es `overflow_buscar_rango` (Persona C),
porque la cadena de overflow no queda ordenada y hay que recorrerla entera.

Para `coordenadas` la clave por defecto es `id_coordenada`. Si el rango
que necesitan es por `orden` dentro de una ruta específica, lean el
comentario en `tablas/coordenada.h`.

## Archivos comunes (ya definidos, no tocar la estructura sin avisar al equipo)
- `registro.h`/`registro.c` — struct genérico `Registro` (clave + bytes).
- `tablas/coordenada.h`/`tablas/coordenada.c` — empaquetado para `coordenadas`.
- `tablas/bus.h`/`tablas/bus.c` — segundo ejemplo, para ver que conviven varias tablas.
- `config.h` — `TAM_PAGINA`, `TAM_REGISTRO_MAX` y nombres de archivo estándar.

## Persona A — módulo DATOS (`datos.h` / `datos.c`)
Crea y mantiene el archivo de datos de una tabla: la carga estática inicial
en páginas ordenadas por clave (el "área home" de la ISAM).
- `datos_crear`, `datos_abrir/cerrar`
- `datos_leer_bloque`, `datos_escribir_bloque`
- `datos_num_bloques`, `datos_buscar_en_bloque`
- `datos_factor_bloque`, `datos_reservar_bloque`, `datos_liberar_bloque` (ya resueltas)

No depende de nadie más. Puede probar su módulo con un `main` propio
temporal antes de integrarlo.

## Persona B — módulo INDICE (`indice.h` / `indice.c`)
Construye el índice disperso (una entrada por página, con la clave menor
de cada una) y lo persiste en el archivo de índice de la tabla.
- `indice_construir`, `indice_guardar/cargar`
- `indice_buscar_bloque`, `indice_num_entradas`

Depende de `datos_leer_bloque` y `datos_num_bloques` (declaradas en
`datos.h`, no de la implementación). Mientras Persona A termina, puede
avanzar con un `datos.c` de prueba que devuelva bloques hardcodeados.

## Persona C — módulo OVERFLOW (`overflow.h` / `overflow.c`)
Maneja el archivo de overflow de la tabla: la zona de desborde para
registros que no caben en su página después de inserciones (encadenamiento
simple).
- `overflow_crear`, `overflow_abrir/cerrar`
- `overflow_insertar`, `overflow_buscar`, `overflow_eliminar`, `overflow_buscar_rango`

Totalmente independiente de datos e índice: solo trabaja con `Registro`.

## Integración (`main.c`)
Ya tiene armadas `isam_buscar`, `isam_insertar`, `isam_eliminar` e
`isam_buscar_rango` usando las 3 interfaces, más el armado de rutas por
tabla. Cuando los tres módulos compilen, se juntan, se llenan los TODOs de
integración (inserción/eliminación dentro de una página) y se prueba con
datos reales de `coordenadas` (u otra tabla).

## Compilar
```
make
./isam
```

## Orden sugerido de trabajo
1. Persona A termina `datos.c` (o al menos deja un stub que compile).
2. Persona B y Persona C pueden trabajar en paralelo desde el día 1,
   usando solo los `.h`.
3. Se integra en `main.c` cuando los 3 `.c` compilan sin errores.
