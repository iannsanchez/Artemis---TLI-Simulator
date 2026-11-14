# ARTEMIS - TLI Simulator

Trabajo Práctico Integrador Final — Algoritmos y Estructuras de Datos (Lenguaje C)

## 1. Integrantes

- Enzo Ian Sanchez Borelli
- Ivo Bruic
- Lucas Lautaro Gomez Contreras
- Comisión: D
- Universidad: UTN — Facultad Regional Resistencia

## 2. Escenario

El programa representa, mediante una simulación interactiva por eventos, la etapa posterior a la Inyección Translunar (TLI) de Orion. Se utilizan los datos iniciales proporcionados por la consigna:

| Parámetro | Valor inicial |
|---|---:|
| Masa de Orion | 26.000 kg |
| Velocidad posterior a TLI | 10.900 m/s |
| Altura de órbita LEO | 192.000 m |
| Distancia media Tierra-Luna | 384.000.000 m |

El operador configura la nave, ejecuta la TLI, desacopla ICPS, responde un desafío sobre inercia y consulta análisis gravitatorios y energéticos. El programa conserva un historial y genera un informe final.

## 3. Alcance y modelo físico

El programa usa unidades del Sistema Internacional (SI) en los cálculos: metros, kilogramos, segundos y joules. Los parámetros gravitatorios GM de la Tierra y la Luna se toman de JPL DE440 (modelo de efemérides desarrollado por el Jet Propulsion Laboratory (JPL) de la NASA): `GM_Tierra = 3,98600435507 × 10^14 m³/s²` y `GM_Luna = 4,902800118 × 10^12 m³/s²`. Se usa el radio medio terrestre de 6.371 km y el radio medio lunar de 1.737,4 km.

### 3.1. Energía cinética y órbita LEO

Ki = (1/2)mv^2

La velocidad circular y la velocidad de escape terrestres en el radio de LEO se calculan mediante:

v_c = √(G mT/r)

v_e = √(2G mT/ r)

mT = Masa de la Tierra
r = Distancia desde el cento de la Tierra hasta Orión

El programa informa `Δv = v_TLI - v_c` como incremento ideal de velocidad, suponiendo que Orion acelera en la misma dirección en la que ya se está moviendo alrededor de la Tierra. No debe interpretarse como una reconstrucción exacta del encendido real.

### 3.2. Primera y segunda ley de Newton

Apagar los motores elimina el empuje, pero no elimina la velocidad de Orion ni la gravedad. La Primera Ley explica la persistencia del estado de movimiento cuando la fuerza neta es nula; la fuerza gravitatoria produce aceleración y cambia el vector velocidad, de acuerdo con la Segunda Ley (`F = m a`). Por eso el programa no afirma que la nave se detenga al apagar los motores.

### 3.3. Punto L1 y problema restringido de tres cuerpos

El punto L1 se calcula con el modelo circular restringido de tres cuerpos (CR3BP), en un marco que rota con la velocidad angular media del sistema Tierra-Luna:

μ = (G mL) / (G mT + G mL)

La raíz de la ecuación colineal entre la Tierra y la Luna se obtiene con el método de bisección. En coordenadas normalizadas baricéntricas:

f(x) = x - (((1 - μ)(x + μ)) / ∣x + μ∣^3) - ((μ (x - 1 + μ)) / ∣x - 1 + μ∣^3) = 0

Con los valores por defecto, el programa debe obtener aproximadamente 326.041 km desde el centro terrestre y 57.959 km desde el centro lunar. L1 no es el punto donde las dos fuerzas gravitatorias se anulan: en el marco rotante, la diferencia entre las atracciones gravitatorias se equilibra con el término centrífugo.

### 3.4. Trabajo específico: hipótesis y lectura del resultado

Para el cálculo energético se define el potencial efectivo por unidad de masa:

Φef = - (G mT) / rT - (G mL) / rL - (1/2) n^2 x^2

Y la energía específica en el marco rotante:

Erot = (1/2) v^2rot + Φef

Se adopta una condición inicial idealizada: Orion está del lado de la Tierra orientado hacia la Luna, a `R_T + h`, con velocidad tangencial prograda de 10.900 m/s. Para la transformación al marco rotante se usa `v_rot = v_TLI - n·r`. El estado final de referencia es Orion en L1 con velocidad nula en el marco rotante. El trabajo externo específico firmado es `W_ext/m = E_final - E_inicial`.

Con los datos por defecto, ese trabajo firmado resulta negativo (aprox. −141.837 J/kg). Por lo tanto, bajo estas hipótesis no se necesita aportar trabajo positivo adicional: para llegar a L1 con velocidad rotante nula habría que retirar energía. El programa muestra por separado el trabajo positivo adicional mínimo (cero si el trabajo firmado es negativo).

Este cálculo es un diagnóstico energético idealizado, no una predicción de la trayectoria real ni una prueba de que Orion vaya a alcanzar L1. La energía del marco rotante se conserva en el CR3BP ideal cuando no actúan fuerzas externas no modeladas.

### 3.5. Combustible: qué puede y qué no puede concluirse

El programa compara como referencias la energía específica necesaria para escapar desde una órbita circular LEO terrestre y desde reposo en la superficie lunar. Esa comparación muestra que el pozo gravitatorio terrestre es más profundo. Pero, **la relación de energías no es una relación directa de masas de combustible**. Para estimar propelente se necesitan, como mínimo, el impulso específico (`Isp`), las masas inicial/final (mi, mf), las etapas y el perfil completo de maniobras. La ecuación ideal del cohete es:

Δv = Isp g0 ln(mi / mf)

Además, el regreso Luna-Tierra puede aprovechar una trayectoria de retorno libre y la atmósfera terrestre para la captura/desaceleración. No es correcto afirmar que todo viaje Tierra-Luna requiere siempre más combustible que cualquier regreso sin especificar las órbitas y maniobras comparadas.

## 4. Limitaciones conocidas

La consigna proporciona un módulo de velocidad, pero no proporciona el vector de velocidad inicial (dirección/ángulo), la época, la orientación del sistema, los parámetros de una órbita lunar de llegada ni un estado inicial cartesiano completo. Por eso esos datos no determinan una trayectoria única de retorno libre.

En consecuencia, esta versión implementa una simulación interactiva por fases/eventos y un muestreo gravitatorio sobre el eje Tierra-Luna; **no integra numéricamente una trayectoria real de retorno libre**.

## 5. Estructura del proyecto

```
    ARTEMIS-TLI
    ├── main.c             # Menú y flujo general
    ├── artemis.h          # Constantes, tipos de datos y prototipos públicos
    ├── artemis.c          # Estados, eventos, interacción e informes
    ├── fisica.c           # Cálculos físicos y método de bisección para L1
    ├── validaciones.h     # Prototipos de ingreso seguro
    ├── validaciones.c     # Validación de enteros y reales
    ├── tests_fisica.c     # Pruebas de cálculos principales
    ├── Makefile           # Compilación y pruebas
    ├── compilar.bat       # Compilación rápida en Windows
    ├── probar.bat         # Pruebas físicas en Windows
    ├── .gitignore         # Archivos que no se versionan
    ├── README.md          # Documentación
```

## 6. Compilación y ejecución

Desde la carpeta del proyecto pueden ejecutar `compilar.bat` desde CMD o PowerShell.

Para las pruebas físicas se puede ejecutar `probar.bat` o compilar `fisica.c` junto con `tests_fisica.c`.

## 7. Uso del programa

1. `Configurar parametros`: permite ingresar masa, velocidad TLI, altura LEO y distancia Tierra-Luna.
2. `Ejecutar maniobra TLI`: calcula energía cinética y el delta-v ideal respecto de una órbita circular.
3. `Desacoplar ICPS`: registra el apagado de los motores.
4. `Desafio de navegacion`: plantea una pregunta sobre inercia y gravedad.
5. `Analizar influencia gravitatoria`: compara las atracciones de Tierra y Luna en puntos del eje hasta L1.
6. `Analizar energia y punto L1`: calcula L1, energías y referencias de escape.
7. `Consultar historial`: muestra los eventos registrados.
8. `Generar informe`: muestra parámetros, resultados, actividades y eventos.

Las opciones se bloquean cuando se intenta ejecutar una etapa fuera de orden. Los valores numéricos se validan por tipo, finitud y rango. Se admite punto o coma como separador decimal.

## 8. Estructuras y modularización

- `DatosMision`: agrupa masa, velocidad TLI, altura y distancia Tierra-Luna.
- `ResultadosFisicos`: almacena los resultados calculados para reutilizarlos en distintas partes del programa.
- `Evento`: almacena la descripción de un evento. Se utiliza un arreglo de eventos dentro de `Mision`.
- `Mision`: agrupa datos, resultados, estado, respuestas y el historial.
- Los punteros permiten que las funciones modifiquen la estructura de misión original o reciban datos de lectura (`const`) sin copiar estructuras completas.

## 9. Diagrama del flujo de misión

```
    A[Inicio] --> B[Preparación en LEO]
    B --> C[Configurar parámetros]
    C --> D[Ejecutar TLI]
    D --> E[Desacoplar ICPS y apagar motores]
    E --> F[Desafío: inercia]
    E --> G[Análisis gravitatorio]
    E --> H[Análisis energético y L1]
    F --> I[Informe e historial]
    G --> I
    H --> I
    I --> J{¿Actividades completas?}
    J -->|No| K[Continuar simulación]
    K --> F
    J -->|Sí| L[Simulación completada]
    L --> M[Salir]
```

## 10. Pruebas realizadas

Se ejecutó la compilación con `-Wall -Wextra -Wpedantic` y el objetivo `make test`. Las pruebas automatizadas verifican:

- Energía cinética para 26.000 kg y 10.900 m/s: 1,54453 × 10¹² J.
- Cálculo de L1 por bisección: distancia Tierra-L1 cercana a 326.041 km.
- Consistencia de las distancias: Tierra-L1 + L1-Luna = distancia Tierra-Luna.
- Resultados físicos con los parámetros por defecto.
- Rechazo de una masa negativa.

También se hizo una prueba de recorrido por consola desde TLI hasta informe final.

## 11. Distribución del trabajo 

| Integrante | Tareas |

| Ian | Preparación inicial del repositorio y configuración de compilación (.gitignore, Makefile); desarrollo de los cálculos físicos (fisica.c); elaboración de pruebas físicas (tests_fisica.c). |

| Ivo | Definición de estructuras, enumeraciones y declaraciones (artemis.h); desarrollo de la lógica principal de la misión (artemis.c); elaboración de la documentación del proyecto (README.md). |

| Lucas |   Desarrollo de las validaciones de entrada (validaciones.h, validaciones.c); implementación del menú principal (main.c); preparación de los scripts de compilación y pruebas para Windows (compilar.bat, probar.bat). |

## 12. Uso de Inteligencia Artificial

**Herramienta utilizada:** ChatGPT (OpenAI).

La asistencia se utilizó para estructurar el proyecto, revisar el planteo físico y generar una primera implementación de funciones. 

### Consultas utilizadas y respuestas

Las siguientes descripciones resumen las consultas de esta conversación; no se presentan como transcripciones literales.

**Consulta 1 — Fundamento físico**

- **Solicitud:** Cómo encarar inercia, energía, gravedad y punto de equilibrio Tierra-Luna, para el programa.
- **Respuesta recibida:** se distinguió la Primera Ley (inercia) de la Segunda Ley (aceleración por fuerza neta); se propuso usar el CR3BP para L1 y explicitar la transformación al marco rotante.
- **Aplicación:** funciones de energía, gravedad y L1.

**Consulta 2 — Verificación de parámetros y modelos**

- **Solicitud de asistencia:** contrastar parámetros GM y revisar las ecuaciones usadas para L1 y combustible con fuentes técnicas.
- **Respuesta recibida:** se consultaron datos de JPL DE440, la formulación CR3BP de JPL y la ecuación ideal del cohete de NASA Glenn.
- **Aplicación:** constantes físicas en `artemis.h`, cálculo de L1 por bisección y advertencia de que no se puede calcular una masa real de propelente sin Isp y masas inicial/final.

**Consulta 3 — Asistencia en compilación y configuración del entorno**

- **Solicitud de asistencia:** cómo compilar el programa en Windows utilizando GCC y GNU Make, configurar las opciones de compilación y ejecutar las pruebas automatizadas.
- **Respuesta recibida:** se explicó cómo utilizar -Wall, -Wextra y -Wpedantic para detectar advertencias durante la compilación, cómo establecer estas opciones mediante CFLAGS en el Makefile y cómo ejecutar make, make test y los scripts .bat.
- **Aplicación:** configuración del proceso de compilación, ejecución de pruebas y verificación del programa en el entorno Windows.

### 13. Validación y adaptación

Se compiló el proyecto con GCC, ejecutamos `make test` y recorrimos por consola la secuencia TLI -> desacoplamiento -> desafío -> análisis -> informe. 

## 14. Fuentes de consulta

- JPL, *Astrodynamic Parameters* (parámetros GM de Tierra y Luna, DE440): https://ssd.jpl.nasa.gov/astro_par.html
- JPL, *Planetary Physical Parameters* (radios medios): https://ssd.jpl.nasa.gov/planets/phys_par.html
- JPL, *Physical Parameters of Satellites* (radio medio lunar): https://ssd.jpl.nasa.gov/sats/phys_par/sep.html
- JPL, *Three-Body Periodic Orbits / Circular Restricted Three-Body Problem*: https://ssd.jpl.nasa.gov/tools/periodic_orbits.html
- NASA Glenn, *Ideal Rocket Equation*: https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/ideal-rocket-equation/
- NASA Glenn, *Specific Impulse*: https://www1.grc.nasa.gov/beginners-guide-to-aeronautics/specific-impulse/
