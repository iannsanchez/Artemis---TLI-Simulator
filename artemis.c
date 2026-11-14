#include "artemis.h"
#include "validaciones.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

static const char *nombreEstado(EstadoMision estado)
{
    switch (estado) {
        case ESTADO_PREPARACION: return "Preparacion LEO";
        case ESTADO_TLI_REALIZADA: return "TLI ejecutada";
        case ESTADO_ICPS_SEPARADA: return "ICPS desacoplada";
        case ESTADO_ANALISIS_COMPLETO: return "Analisis completo";
        default: return "Estado desconocido";
    }
}

static void actualizarEstado(Mision *mision)
{
    if (mision->estado >= ESTADO_ICPS_SEPARADA &&
        mision->desafioRespondido &&
        mision->analisisGravitatorioRealizado &&
        mision->analisisEnergeticoRealizado) {
        mision->estado = ESTADO_ANALISIS_COMPLETO;
    }
}

void registrarEvento(Mision *mision, const char *descripcion)
{
    if (mision == NULL || descripcion == NULL) {
        return;
    }

    if (mision->cantidadEventos >= MAX_EVENTOS) {
        printf("Aviso: se alcanzo el limite del historial de eventos.\n");
        return;
    }

    snprintf(mision->historial[mision->cantidadEventos].descripcion,
             MAX_DESC_EVENTO, "%s", descripcion);
    mision->cantidadEventos++;
}

void inicializarMision(Mision *mision)
{
    if (mision == NULL) {
        return;
    }

    memset(mision, 0, sizeof(*mision));
    mision->datos.masaKg = MASA_DEFAULT;
    mision->datos.velocidadTliMps = VELOCIDAD_DEFAULT;
    mision->datos.alturaLeoM = ALTURA_DEFAULT;
    mision->datos.distanciaTierraLunaM = DISTANCIA_DEFAULT;
    mision->estado = ESTADO_PREPARACION;
    registrarEvento(mision, "Simulador iniciado: Orion se encuentra en orbita LEO.");
}

void configurarMision(Mision *mision)
{
    double distanciaMinima;
    char error[160];

    if (mision->estado != ESTADO_PREPARACION) {
        printf("La configuracion queda bloqueada despues de ejecutar la TLI.\n");
        return;
    }

    printf("\n--- CONFIGURACION DE ORION ---\n");
    printf("Valores iniciales: masa %.1f kg, velocidad %.1f m/s, altura %.1f m, distancia %.1f km.\n",
           mision->datos.masaKg, mision->datos.velocidadTliMps,
           mision->datos.alturaLeoM, mision->datos.distanciaTierraLunaM / 1000.0);

    mision->datos.masaKg = leerReal("Masa de Orion (kg): ", 100.0, 1.0e7);
    mision->datos.velocidadTliMps = leerReal("Velocidad despues de TLI (m/s): ", 100.0, 50000.0);
    mision->datos.alturaLeoM = leerReal("Altura LEO (m): ", 0.0, 1000000.0);

    distanciaMinima = RADIO_TIERRA + mision->datos.alturaLeoM + 1000.0;
    mision->datos.distanciaTierraLunaM = leerReal(
        "Distancia Tierra-Luna (m): ", distanciaMinima, 1.0e9);

    if (!calcularResultadosFisicos(&mision->datos, &mision->resultados,
                                   error, sizeof(error))) {
        printf("No se pudo validar la configuracion: %s\n", error);
        printf("Se restauran los valores iniciales.\n");
        mision->datos.masaKg = MASA_DEFAULT;
        mision->datos.velocidadTliMps = VELOCIDAD_DEFAULT;
        mision->datos.alturaLeoM = ALTURA_DEFAULT;
        mision->datos.distanciaTierraLunaM = DISTANCIA_DEFAULT;
        return;
    }

    registrarEvento(mision, "Parametros de Orion configurados por el operador.");
    printf("Configuracion guardada correctamente.\n");
}

void ejecutarTLI(Mision *mision)
{
    char error[160];

    if (mision->estado != ESTADO_PREPARACION) {
        printf("La maniobra TLI ya fue ejecutada o la mision avanzo de etapa.\n");
        return;
    }

    if (!calcularResultadosFisicos(&mision->datos, &mision->resultados,
                                   error, sizeof(error))) {
        printf("No se puede ejecutar la TLI: %s\n", error);
        return;
    }

    mision->estado = ESTADO_TLI_REALIZADA;
    registrarEvento(mision, "ICPS ejecuto la maniobra TLI; Orion alcanzo la velocidad indicada.");

    printf("\nTLI EJECUTADA.\n");
    printf("Energia cinetica inicial: %.3e J\n", mision->resultados.energiaCineticaJ);
    printf("Incremento respecto de una orbita circular LEO: %.2f m/s\n",
           mision->resultados.deltaVTliMps);
    printf("La velocidad ingresada se interpreta como velocidad final despues del impulso, no como velocidad de escape terrestre exacta.\n");
}

void separarICPS(Mision *mision)
{
    if (mision->estado != ESTADO_TLI_REALIZADA) {
        printf("Primero debe ejecutarse la maniobra TLI.\n");
        return;
    }

    mision->estado = ESTADO_ICPS_SEPARADA;
    registrarEvento(mision, "ICPS se desacoplo; los motores principales quedaron apagados.");
    printf("ICPS desacoplada. Orion continua en movimiento por inercia y bajo accion gravitatoria.\n");
}

void realizarDesafio(Mision *mision)
{
    int respuesta;

    if (mision->estado < ESTADO_ICPS_SEPARADA) {
        printf("El desafio se habilita despues del desacoplamiento de ICPS.\n");
        return;
    }
    if (mision->desafioRespondido) {
        printf("El desafio ya fue respondido.\n");
        return;
    }

    printf("\n--- DESAFIO DE NAVEGACION: PRIMERA LEY DE NEWTON ---\n");
    printf("Los motores se apagaron. Que ocurre con el movimiento de Orion?\n");
    printf("1. La nave se detiene inmediatamente.\n");
    printf("2. Conserva su velocidad instantanea, mientras la gravedad modifica su velocidad vectorial.\n");
    printf("3. La nave deja de experimentar fuerzas gravitatorias.\n");
    respuesta = leerEntero("Respuesta: ", 1, 3);

    mision->desafioRespondido = 1;
    if (respuesta == 2) {
        mision->respuestasCorrectas++;
        registrarEvento(mision, "Desafio de inercia respondido correctamente.");
        printf("Correcto. La inercia mantiene el estado de movimiento; la fuerza gravitatoria produce aceleracion.\n");
    } else {
        registrarEvento(mision, "Desafio de inercia respondido incorrectamente.");
        printf("No es correcto. El apagado de motores elimina el empuje, no la velocidad ni la gravedad.\n");
        printf("La Primera Ley describe la inercia; la Segunda Ley explica el cambio de velocidad por la fuerza neta.\n");
    }
    actualizarEstado(mision);
}

void analizarGravedad(Mision *mision)
{
    double distanciaL1Tierra;
    double distanciaL1Luna;
    double mu;
    double n;
    double radioLeo;
    double fracciones[] = {0.25, 0.50, 0.75, 1.00};
    size_t i;

    if (mision->estado < ESTADO_ICPS_SEPARADA) {
        printf("El analisis de trayectoria se habilita despues del desacoplamiento de ICPS.\n");
        return;
    }

    if (!calcularPuntoL1(mision->datos.distanciaTierraLunaM,
                         &distanciaL1Tierra, &distanciaL1Luna, &mu, &n)) {
        printf("No se pudo calcular L1.\n");
        return;
    }

    radioLeo = RADIO_TIERRA + mision->datos.alturaLeoM;
    printf("\n--- ANALISIS GRAVITATORIO SOBRE EL EJE TIERRA-LUNA ---\n");
    printf("Este muestreo compara posiciones del eje; no representa una trayectoria orbital integrada.\n\n");
    printf("%-12s %-16s %-16s %-16s\n", "Punto", "Dist. Tierra km", "g Tierra m/s2", "g Luna m/s2");

    for (i = 0; i < sizeof(fracciones) / sizeof(fracciones[0]); i++) {
        double distanciaTierra = radioLeo + fracciones[i] * (distanciaL1Tierra - radioLeo);
        double distanciaLuna = mision->datos.distanciaTierraLunaM - distanciaTierra;
        double gTierra = calcularGravedad(GM_TIERRA, distanciaTierra);
        double gLuna = calcularGravedad(GM_LUNA, distanciaLuna);

        printf("%-12.0f %-16.3f %-16.6f %-16.6f\n",
               fracciones[i] * 100.0, distanciaTierra / 1000.0, gTierra, gLuna);
    }

    printf("\nEn L1, gTierra - gLuna = %.9f m/s2.\n",
           mision->resultados.gravedadTierraL1Mps2 -
           mision->resultados.gravedadLunaL1Mps2);
    printf("La aceleracion centripeta del marco rotante es %.9f m/s2.\n",
           n * n * (distanciaL1Tierra - mu * mision->datos.distanciaTierraLunaM));
    printf("En el modelo circular restringido, ambas cantidades coinciden en L1.\n");

    mision->analisisGravitatorioRealizado = 1;
    registrarEvento(mision, "Se analizaron las aceleraciones gravitatorias sobre el eje Tierra-Luna.");
    actualizarEstado(mision);
}

void analizarEnergia(Mision *mision)
{
    double radioLeo = RADIO_TIERRA + mision->datos.alturaLeoM;
    double energiaEscapeLeo = GM_TIERRA / (2.0 * radioLeo);
    double energiaEscapeLuna = GM_LUNA / RADIO_LUNA;
    double relacion = energiaEscapeLeo / energiaEscapeLuna;

    if (mision->estado < ESTADO_TLI_REALIZADA) {
        printf("Primero debe ejecutarse la maniobra TLI.\n");
        return;
    }

    if (!calcularResultadosFisicos(&mision->datos, &mision->resultados, NULL, 0)) {
        printf("No se pudieron calcular los resultados energeticos.\n");
        return;
    }

    printf("\n--- ANALISIS ENERGETICO ---\n");
    printf("Energia cinetica inicial:                 %.6e J\n",
           mision->resultados.energiaCineticaJ);
    printf("Velocidad circular LEO:                  %.3f m/s\n",
           mision->resultados.velocidadCircularMps);
    printf("Velocidad de escape terrestre en LEO:    %.3f m/s\n",
           mision->resultados.velocidadEscapeLeoMps);
    printf("Delta-v ideal desde orbita circular:     %.3f m/s\n",
           mision->resultados.deltaVTliMps);
    printf("Gravedad terrestre en LEO:               %.6f m/s2\n",
           mision->resultados.gravedadLeoMps2);

    printf("\nModelo energetico CR3BP (marco rotante):\n");
    printf("L1 desde el centro terrestre:            %.3f km\n",
           mision->resultados.puntoL1DesdeTierraKm);
    printf("L1 desde el centro lunar:                %.3f km\n",
           mision->resultados.puntoL1DesdeLunaKm);
    printf("Trabajo firmado para llegar a reposo en L1: %.3f J/kg\n",
           mision->resultados.trabajoFirmadoJkg);
    printf("Trabajo firmado total para Orion:          %.3e J\n",
           mision->resultados.trabajoFirmadoJkg * mision->datos.masaKg);
    printf("Trabajo adicional positivo minimo:         %.3f J/kg\n",
           mision->resultados.trabajoAdicionalMinimoJkg);
    printf("Velocidad en L1 sin trabajo externo:     %.3f m/s (marco rotante)\n",
           mision->resultados.velocidadEnL1SinTrabajoMps);

    if (mision->resultados.trabajoFirmadoJkg < 0.0) {
        printf("Interpretacion: con las hipotesis adoptadas no hace falta aportar energia;\n");
        printf("para llegar a L1 con velocidad rotante nula habria que retirar energia.\n");
    } else {
        printf("Interpretacion: se requiere aporte energetico positivo para alcanzar\n");
        printf("el estado final idealizado de reposo en el marco rotante.\n");
    }

    printf("\nComparacion de energia especifica de escape (aproximacion):\n");
    printf("Desde orbita circular LEO terrestre:    %.3f MJ/kg\n",
           energiaEscapeLeo / 1.0e6);
    printf("Desde reposo en superficie lunar:       %.3f MJ/kg\n",
           energiaEscapeLuna / 1.0e6);
    printf("Relacion de esas referencias:           %.2f veces\n", relacion);
    printf("Esto compara pozos energeticos ideales; NO equivale directamente a una\n");
    printf("relacion de masas de combustible Tierra-Luna/Luna-Tierra.\n");
    printf("La masa real de propelente requiere Isp, masas inicial/final, etapas y perfil de maniobras.\n");

    mision->analisisEnergeticoRealizado = 1;
    registrarEvento(mision, "Se calcularon energias, L1 y comparacion de pozos gravitatorios.");
    actualizarEstado(mision);
}

void mostrarHistorial(const Mision *mision)
{
    int i;

    printf("\n--- HISTORIAL DE LA MISION ---\n");
    if (mision->cantidadEventos == 0) {
        printf("No hay eventos registrados.\n");
        return;
    }

    for (i = 0; i < mision->cantidadEventos; i++) {
        printf("%02d. %s\n", i + 1, mision->historial[i].descripcion);
    }
}

void generarInforme(Mision *mision)
{
    int tareasCompletas = mision->desafioRespondido +
                          mision->analisisGravitatorioRealizado +
                          mision->analisisEnergeticoRealizado;

    actualizarEstado(mision);
    printf("\n===============================================\n");
    printf("          INFORME DE MISION ARTEMIS TLI\n");
    printf("===============================================\n");
    printf("Estado: %s\n", nombreEstado(mision->estado));
    printf("Masa Orion: %.2f kg\n", mision->datos.masaKg);
    printf("Velocidad TLI: %.2f m/s\n", mision->datos.velocidadTliMps);
    printf("Altura LEO: %.2f km\n", mision->datos.alturaLeoM / 1000.0);
    printf("Distancia Tierra-Luna: %.2f km\n",
           mision->datos.distanciaTierraLunaM / 1000.0);
    printf("Energia cinetica inicial: %.6e J\n",
           mision->resultados.energiaCineticaJ);
    printf("Desafio de inercia: %s\n",
           mision->desafioRespondido ?
           (mision->respuestasCorrectas ? "correcto" : "respondido, con error") : "pendiente");
    printf("Analisis gravitatorio: %s\n",
           mision->analisisGravitatorioRealizado ? "realizado" : "pendiente");
    printf("Analisis energetico: %s\n",
           mision->analisisEnergeticoRealizado ? "realizado" : "pendiente");
    printf("Actividades interactivas completas: %d/3\n", tareasCompletas);
    printf("Eventos registrados: %d\n", mision->cantidadEventos);

    if (tareasCompletas == 3 && mision->estado >= ESTADO_ICPS_SEPARADA) {
        printf("Resultado: SIMULACION COMPLETADA.\n");
    } else {
        printf("Resultado: SIMULACION EN PROGRESO.\n");
    }
    printf("Nota del modelo: no se integra una trayectoria real de retorno libre;\n");
    printf("se analizan eventos, el eje Tierra-Luna y el equilibrio L1 con CR3BP.\n");
    printf("===============================================\n");
    mostrarHistorial(mision);
}

