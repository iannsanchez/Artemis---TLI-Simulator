#ifndef ARTEMIS_H
#define ARTEMIS_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_EVENTOS 30
#define MAX_DESC_EVENTO 140

/* Constantes físicas: SI (m, kg, s). Valores GM de JPL DE440. */
#define GM_TIERRA       3.98600435507e14
#define GM_LUNA         4.902800118e12
#define RADIO_TIERRA    6.3710e6
#define RADIO_LUNA      1.7374e6
#define G0              9.80665

#define MASA_DEFAULT       26000.0
#define VELOCIDAD_DEFAULT  10900.0
#define ALTURA_DEFAULT     192000.0
#define DISTANCIA_DEFAULT  384000000.0

typedef enum {
    ESTADO_PREPARACION = 0,
    ESTADO_TLI_REALIZADA,
    ESTADO_ICPS_SEPARADA,
    ESTADO_ANALISIS_COMPLETO
} EstadoMision;

typedef struct {
    double masaKg;
    double velocidadTliMps;
    double alturaLeoM;
    double distanciaTierraLunaM;
} DatosMision;

typedef struct {
    char descripcion[MAX_DESC_EVENTO];
} Evento;

typedef struct {
    double energiaCineticaJ;
    double velocidadCircularMps;
    double velocidadEscapeLeoMps;
    double deltaVTliMps;
    double gravedadLeoMps2;
    double velocidadAngularRadS;
    double puntoL1DesdeTierraKm;
    double puntoL1DesdeLunaKm;
    double gravedadTierraL1Mps2;
    double gravedadLunaL1Mps2;
    double aceleracionCentrifugaL1Mps2;
    double trabajoFirmadoJkg;
    double trabajoAdicionalMinimoJkg;
    double velocidadEnL1SinTrabajoMps;
} ResultadosFisicos;

typedef struct {
    DatosMision datos;
    ResultadosFisicos resultados;
    Evento historial[MAX_EVENTOS];
    int cantidadEventos;
    int respuestasCorrectas;
    int desafioRespondido;
    int analisisGravitatorioRealizado;
    int analisisEnergeticoRealizado;
    EstadoMision estado;
} Mision;

/* Ciclo de vida y lógica de la misión. */
void inicializarMision(Mision *mision);
void configurarMision(Mision *mision);
void ejecutarTLI(Mision *mision);
void separarICPS(Mision *mision);
void realizarDesafio(Mision *mision);
void analizarGravedad(Mision *mision);
void analizarEnergia(Mision *mision);
void mostrarHistorial(const Mision *mision);
void generarInforme(Mision *mision);

/* Cálculos físicos. Todas las funciones trabajan en unidades SI. */
bool calcularResultadosFisicos(const DatosMision *datos,
                               ResultadosFisicos *resultados,
                               char *mensajeError,
                               size_t tamanioMensaje);
bool calcularPuntoL1(double distanciaM, double *distanciaDesdeTierraM,
                     double *distanciaDesdeLunaM, double *muSistema,
                     double *velocidadAngular);
double calcularEnergiaCinetica(double masaKg, double velocidadMps);
double calcularGravedad(double GM, double distanciaM);
double calcularVelocidadCircular(double GM, double radioOrbitaM);
double calcularVelocidadEscape(double GM, double radioM);
void registrarEvento(Mision *mision, const char *descripcion);

#endif
