#ifndef ACQUISITION_H
#define ACQUISITION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "cesi_types.h"

typedef enum {
    LIGHT_LEVEL_NA = 0,
    LIGHT_LEVEL_LOW,     // < LUMIN_LOW
    LIGHT_LEVEL_MEDIUM,  // entre LUMIN_LOW et LUMIN_HIGH
    LIGHT_LEVEL_HIGH     // > LUMIN_HIGH
} LightLevel;

/*
 * Une ligne de mesures horodatée (indicateur *_valid à 0 -> "NA").
 * Types ajustés à la précision réellement utilisée (1 décimale affichée) :
 * entiers en dixièmes plutôt que float, indicateurs sur 1 bit.
 */
typedef struct {
    RTC_DateTime time;

    int16_t  temperature_x10;   // Dixièmes de °C (214 = 21,4 °C)
    uint16_t humidity_x10;      // Dixièmes de % (482 = 48,2 %)
    uint16_t pressure_x10;      // Dixièmes de hPa (10136 = 1013,6 hPa)
    uint16_t light;             // 0-1023
    int32_t  latitude_e6;       // Millionièmes de degré (négatif = Sud)
    int32_t  longitude_e6;      // Millionièmes de degré (négatif = Ouest)
    uint8_t  satellites;
    uint8_t  light_level;       // LightLevel

    uint16_t time_valid    : 1;
    uint16_t temp_valid    : 1;
    uint16_t hum_valid     : 1;
    uint16_t pres_valid    : 1;
    uint16_t light_valid   : 1;
    uint16_t gps_skipped   : 1; // GPS non lu (mode économique : une mesure sur deux)
    uint16_t gps_responded : 1; // Trame GPS reçue pendant l'acquisition
    uint16_t gps_valid     : 1; // Position disponible (fix GPS)
} Measurement;

// Bilan de santé mis à jour à la fin de chaque acquisition (1 bit par indicateur)
typedef struct {
    uint8_t rtc_ok    : 1;  // Horloge RTC lue
    uint8_t gps_ok    : 1;  // GPS en erreur après 2 mesures consécutives sans trame
    uint8_t sensor_ok : 1;  // Aucun capteur en erreur (2 mesures consécutives en timeout)
    uint8_t data_ok   : 1;  // Données dans les seuils (MIN/MAX_TEMP_AIR, PRESSURE_MIN/MAX)
} AcquisitionHealth;

// Lance une acquisition (non bloquant) ; with_gps = false : GPS non lu
void Acquisition_Start(bool with_gps);

// À appeler dans la boucle principale. Retourne true quand l'acquisition est
// terminée : *m contient alors la mesure. Un capteur qui ne répond pas est
// réinterrogé jusqu'à TIMEOUT, puis sa donnée vaut "NA".
bool Acquisition_Poll(Measurement *m);

bool Acquisition_IsRunning(void);
AcquisitionHealth Acquisition_GetHealth(void);

// Affichage lisible sur le moniteur série
void Acquisition_Print(const Measurement *m);

// Ligne pour le fichier de log SD : AAAA-MM-JJ HH:MM:SS;TEMP;HUM;PRES;LUMIN;NIVEAU;LAT;LON
const char *Acquisition_HeaderLine(void);
void Acquisition_FormatLine(const Measurement *m, char *buf, size_t len);

#endif // ACQUISITION_H
