#include "artemis.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define MAX_BISECCION 200
#define TOL_BISECCION 1.0e-13

static double funcionL1(double x, double mu)
{
    double distanciaTierra = x + mu;
    double distanciaLuna = x - 1.0 + mu;

    return x
         - (1.0 - mu) * distanciaTierra / pow(fabs(distanciaTierra), 3.0)
         - mu * distanciaLuna / pow(fabs(distanciaLuna), 3.0);
}

double calcularEnergiaCinetica(double masaKg, double velocidadMps)
{
    return 0.5 * masaKg * velocidadMps * velocidadMps;
}

double calcularGravedad(double GM, double distanciaM)
{
    return GM / (distanciaM * distanciaM);
}

double calcularVelocidadCircular(double GM, double radioOrbitaM)
{
    return sqrt(GM / radioOrbitaM);
}

double calcularVelocidadEscape(double GM, double radioM)
{
    return sqrt(2.0 * GM / radioM);
}

bool calcularPuntoL1(double distanciaM, double *distanciaDesdeTierraM,
                     double *distanciaDesdeLunaM, double *muSistema,
                     double *velocidadAngular)
{
    double mu;
    double inferior;
    double superior;
    double medio;
    double fInferior;
    double fMedio;
    double xL1;
    double n;
    int i;

    if (distanciaM <= 0.0 || distanciaDesdeTierraM == NULL ||
        distanciaDesdeLunaM == NULL || muSistema == NULL ||
        velocidadAngular == NULL) {
        return false;
    }

    mu = GM_LUNA / (GM_TIERRA + GM_LUNA);
    inferior = -mu + 1.0e-10;
    superior = 1.0 - mu - 1.0e-10;
    fInferior = funcionL1(inferior, mu);

    if (fInferior * funcionL1(superior, mu) >= 0.0) {
        return false;
    }

    for (i = 0; i < MAX_BISECCION; i++) {
        medio = 0.5 * (inferior + superior);
        fMedio = funcionL1(medio, mu);

        if (fabs(fMedio) < TOL_BISECCION ||
            fabs(superior - inferior) < TOL_BISECCION) {
            break;
        }

        if (fInferior * fMedio < 0.0) {
            superior = medio;
        } else {
            inferior = medio;
            fInferior = fMedio;
        }
    }

    xL1 = 0.5 * (inferior + superior);
    n = sqrt((GM_TIERRA + GM_LUNA) / pow(distanciaM, 3.0));

    *distanciaDesdeTierraM = (xL1 + mu) * distanciaM;
    *distanciaDesdeLunaM = (1.0 - mu - xL1) * distanciaM;
    *muSistema = mu;
    *velocidadAngular = n;

    return true;
}

bool calcularResultadosFisicos(const DatosMision *datos,
                               ResultadosFisicos *resultados,
                               char *mensajeError,
                               size_t tamanioMensaje)
{
    double radioLeo;
    double distanciaL1Tierra;
    double distanciaL1Luna;
    double mu;
    double n;
    double xInicial;
    double xL1Bari;
    double velocidadRotante;
    double potencialInicial;
    double potencialL1;
    double energiaInicialRotante;
    double deltaEnergia;
    double energiaFinalDisponible;

    if (datos == NULL || resultados == NULL) {
        if (mensajeError != NULL && tamanioMensaje > 0) {
            snprintf(mensajeError, tamanioMensaje, "Datos o resultado no disponibles.");
        }
        return false;
    }

    radioLeo = RADIO_TIERRA + datos->alturaLeoM;
    if (datos->masaKg <= 0.0 || datos->velocidadTliMps <= 0.0 ||
        datos->alturaLeoM < 0.0 || datos->distanciaTierraLunaM <= radioLeo) {
        if (mensajeError != NULL && tamanioMensaje > 0) {
            snprintf(mensajeError, tamanioMensaje,
                     "Los parametros no cumplen las restricciones fisicas del modelo.");
        }
        return false;
    }

    memset(resultados, 0, sizeof(*resultados));
    resultados->energiaCineticaJ = calcularEnergiaCinetica(
        datos->masaKg, datos->velocidadTliMps);
    resultados->velocidadCircularMps = calcularVelocidadCircular(GM_TIERRA, radioLeo);
    resultados->velocidadEscapeLeoMps = calcularVelocidadEscape(GM_TIERRA, radioLeo);
    resultados->deltaVTliMps = datos->velocidadTliMps - resultados->velocidadCircularMps;
    resultados->gravedadLeoMps2 = calcularGravedad(GM_TIERRA, radioLeo);

    if (!calcularPuntoL1(datos->distanciaTierraLunaM,
                         &distanciaL1Tierra, &distanciaL1Luna,
                         &mu, &n)) {
        if (mensajeError != NULL && tamanioMensaje > 0) {
            snprintf(mensajeError, tamanioMensaje, "No se pudo encontrar el punto L1 por biseccion.");
        }
        return false;
    }

    resultados->velocidadAngularRadS = n;
    resultados->puntoL1DesdeTierraKm = distanciaL1Tierra / 1000.0;
    resultados->puntoL1DesdeLunaKm = distanciaL1Luna / 1000.0;
    resultados->gravedadTierraL1Mps2 = calcularGravedad(GM_TIERRA, distanciaL1Tierra);
    resultados->gravedadLunaL1Mps2 = calcularGravedad(GM_LUNA, distanciaL1Luna);

    /* El eje x tiene origen en el baricentro y la Tierra en -mu*D.
       Se supone que Orion inicia del lado lunar y con velocidad prograde
       tangencial a la linea Tierra-Luna. */
    xInicial = -mu * datos->distanciaTierraLunaM + radioLeo;
    xL1Bari = distanciaL1Tierra - mu * datos->distanciaTierraLunaM;
    resultados->aceleracionCentrifugaL1Mps2 = n * n * xL1Bari;

    /* Transformacion a marco rotante: v_rot = v_inercial - n x r_rel. */
    velocidadRotante = datos->velocidadTliMps - n * radioLeo;

    potencialInicial = -GM_TIERRA / radioLeo
                     - GM_LUNA / (datos->distanciaTierraLunaM - radioLeo)
                     - 0.5 * n * n * xInicial * xInicial;
    potencialL1 = -GM_TIERRA / distanciaL1Tierra
                - GM_LUNA / distanciaL1Luna
                - 0.5 * n * n * xL1Bari * xL1Bari;

    energiaInicialRotante = 0.5 * velocidadRotante * velocidadRotante + potencialInicial;
    deltaEnergia = potencialL1 - energiaInicialRotante;

    resultados->trabajoFirmadoJkg = deltaEnergia;
    resultados->trabajoAdicionalMinimoJkg = deltaEnergia > 0.0 ? deltaEnergia : 0.0;
    energiaFinalDisponible = energiaInicialRotante - potencialL1;
    resultados->velocidadEnL1SinTrabajoMps = energiaFinalDisponible > 0.0
        ? sqrt(2.0 * energiaFinalDisponible) : 0.0;

    if (mensajeError != NULL && tamanioMensaje > 0) {
        mensajeError[0] = '\0';
    }
    return true;
}
