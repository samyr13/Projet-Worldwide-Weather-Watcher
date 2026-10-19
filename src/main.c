#include <stdio.h>
#include <string.h>
#include "stm32l4xx_hal.h"
#include "setup.h"
#include "cesi_types.h"

// Drivers matériels Grove
#include "grove_rgb.h"
#include "grove_light.h"
#include "grove_rtc_ds1307.h"
#include "grove_bme680.h"
#include "grove_gps_air530z.h"
#include "board_pins.h"

// Modules applicatifs
#include "modes.h"
#include "buttons.h"
#include "system_status.h"
#include "config.h"
#include "acquisition.h"
#include "console.h"
#include "datalog.h"
#include "heap.h"

// Mode maintenance : données consultables "en direct" sur le port série
#define MAINTENANCE_PERIOD_MS 10000

extern I2C_HandleTypeDef hi2c1;

SystemMode mode_actuel = MODE_STANDARD;

/**
 * @brief Temps entre 2 mesures selon le mode (0 = acquisition désactivée)
 */
static uint32_t acquisition_period_ms(SystemMode mode)
{
    uint32_t log_interval_ms = (uint32_t)g_config.log_interval_min * 60UL * 1000UL;

    switch (mode) {
        case MODE_STANDARD:      return log_interval_ms;
        case MODE_ECONOMIQUE:    return log_interval_ms * 2; // LOG_INTERVAL x 2
        case MODE_MAINTENANCE:   return MAINTENANCE_PERIOD_MS;
        case MODE_CONFIGURATION: return 0;                   // Acquisition désactivée
    }
    return 0;
}

/**
 * @brief Message affiché à l'entrée dans un mode
 */
static void announce_mode(SystemMode mode)
{
    switch (mode) {
        case MODE_STANDARD:
            printf("[STANDARD] Une mesure toutes les %u min (LOG_INTERVAL)\r\n\r\n",
                   g_config.log_interval_min);
            break;
        case MODE_ECONOMIQUE:
            printf("[ECONOMIQUE] Une mesure toutes les %u min (LOG_INTERVAL x 2), GPS 1 mesure sur 2\r\n\r\n",
                   g_config.log_interval_min * 2);
            break;
        case MODE_MAINTENANCE:
            printf("[MAINTENANCE] Mesures en direct toutes les %u s, aucune ecriture sur la carte SD :\r\n",
                   MAINTENANCE_PERIOD_MS / 1000);
            printf("              la carte SD peut etre retiree sans risque\r\n\r\n");
            break;
        case MODE_CONFIGURATION:
            printf("[CONFIGURATION] Acquisition desactivee. Retour en mode standard apres 30 min sans activite.\r\n");
            Console_PrintHelp();
            printf("\r\n");
            break;
    }
}

/**
 * @brief Date et heure de la compilation (__DATE__ = "Oct  9 2026", __TIME__ = "14:32:05")
 */
static void build_datetime(RTC_DateTime *dt)
{
    static const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
    const char *date = __DATE__;
    const char *time = __TIME__;
    char month[4] = { date[0], date[1], date[2], '\0' };

    dt->month   = (uint8_t)((strstr(months, month) - months) / 3 + 1);
    dt->day     = (uint8_t)((date[4] == ' ' ? 0 : date[4] - '0') * 10 + (date[5] - '0'));
    dt->year    = (uint16_t)((date[7] - '0') * 1000 + (date[8] - '0') * 100 + (date[9] - '0') * 10 + (date[10] - '0'));
    dt->hours   = (uint8_t)((time[0] - '0') * 10 + (time[1] - '0'));
    dt->minutes = (uint8_t)((time[3] - '0') * 10 + (time[4] - '0'));
    dt->seconds = (uint8_t)((time[6] - '0') * 10 + (time[7] - '0'));

    // Jour de la semaine (Sakamoto) : 1 = lundi ... 7 = dimanche
    static const uint8_t t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
    uint16_t y = dt->year - (dt->month < 3);
    uint8_t dow = (uint8_t)((y + y / 4 - y / 100 + y / 400 + t[dt->month - 1] + dt->day) % 7);
    dt->dayOfWeek = (dow == 0) ? 7 : dow;
}

/**
 * @brief Vérifie la RTC et la démarre à l'heure de compilation si elle est arrêtée
 */
static void rtc_check(void)
{
    bool running = false;

    if (!RTC_PRESENT) {
        printf("[RTC] Horloge non installee (RTC_PRESENT = 0) : mesures sans heure\r\n");
        return;
    }

    if (!GroveRTC_IsRunning(&running)) {
        printf("[RTC] ERREUR : horloge injoignable sur I2C\r\n");
        return;
    }

    if (!running) {
        RTC_DateTime dt;
        build_datetime(&dt);
        GroveRTC_SetDateTime(&dt);
        printf("[RTC] Horloge arretee -> reglee a l'heure de compilation\r\n");
    }

    RTC_DateTime now;
    if (GroveRTC_GetDateTime(&now)) {
        printf("[RTC] %04u-%02u-%02u %02u:%02u:%02u\r\n",
               now.year, now.month, now.day, now.hours, now.minutes, now.seconds);
    }
}

int main(void) {
    // 1. Initialisations matérielles (HAL_Init est appelé dans Global_Init)
    Global_Init();
    Console_Init();
    GroveRGB_Init();
    GroveLight_Init();

    printf("\r\n==============================================\r\n");
    printf("   STATION METEO 3W - INITIALISATION SYSTEME  \r\n");
    printf("==============================================\r\n");

    if (Config_Load()) {
        printf("[CONFIG] Parametres charges depuis la memoire (EEPROM)\r\n");
    } else {
        printf("[CONFIG] Aucun parametre enregistre : valeurs par defaut\r\n");
    }

    check_boot_mode();

    // Diagnostic : liste des composants présents sur le bus I2C
    // (attendu : 0x68 = RTC DS1307, 0x76 = BME280)
    printf("[I2C] Scan du bus :");
    for (uint8_t addr = 0x08; addr < 0x78; addr++) {
        if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr << 1), 1, 10) == HAL_OK) {
            printf(" 0x%02X", addr);
        }
    }
    printf("\r\n");

    rtc_check();

    GroveGPS_Init();
    printf("[GPS] Ecoute sur UART4 (port A0, 9600 bauds)\r\n");

    if (GroveBME680_Init()) {
        printf("[CAPTEUR] %s detecte en 0x%02X, calibration chargee\r\n",
               GroveBME680_GetChipName(), GroveBME680_GetAddress());
    } else {
        printf("[CAPTEUR] ERREUR init : %s (chip ID lu : 0x%02X)\r\n",
               GroveBME680_StatusToString(GroveBME680_GetLastStatus()), GroveBME680_GetFoundChipId());
    }

    DataLog_Init(); // Carte SD : montage et affichage de son contenu

    printf("[MEM] Tas : %u octets utilises, %u octets disponibles avant la pile\r\n",
           (unsigned)Heap_Used(), (unsigned)Heap_Available());

    printf("\r\n");

    SystemStatus status;
    SystemStatus_Init(&status);

    SystemHealth health = {
        .rtc_ok    = 1,
        .gps_ok    = 1,
        .sensor_ok = 1,
        .data_ok   = 1,
        .sd_space_ok = 1,
        .sd_ok       = DataLog_IsOk()
    };

    Measurement measure;
    uint32_t last_acquisition_tick = 0;
    bool measure_now = false;         // Mesure immédiate à l'entrée dans un mode
    uint8_t eco_measure_count = 0;    // Mode économique : GPS lu une mesure sur deux
    SystemMode last_mode = mode_actuel;
    announce_mode(mode_actuel);
    measure_now = true;

    // 2. Boucle principale (Non-bloquante)
    while (1) {
        uint32_t current_tick = HAL_GetTick();

        // A. Boutons, commandes série et trames GPS reçues en arrière-plan
        process_button_presses();
        Console_Process();
        GroveGPS_Process();

        // B. Changement de mode : annonce et première mesure immédiate
        if (mode_actuel != last_mode) {
            last_mode = mode_actuel;
            eco_measure_count = 0;
            measure_now = true;
            if (mode_actuel == MODE_MAINTENANCE)
                DataLog_Release(); // Carte SD libérée : retrait possible sans risque
            announce_mode(mode_actuel);
        }

        // C. Lancement d'une acquisition selon le mode (aucune en configuration)
        uint32_t period_ms = acquisition_period_ms(mode_actuel);
        if (period_ms > 0 && !Acquisition_IsRunning() &&
            (measure_now || current_tick - last_acquisition_tick >= period_ms)) {
            measure_now = false;
            last_acquisition_tick = current_tick;

            bool with_gps = true;
            if (mode_actuel == MODE_ECONOMIQUE) {
                with_gps = (eco_measure_count % 2) == 0;
                eco_measure_count++;
            }
            Acquisition_Start(with_gps);
        }

        // D. Fin d'acquisition : affichage de la mesure et mise à jour de l'état
        if (Acquisition_Poll(&measure)) {
            AcquisitionHealth acq = Acquisition_GetHealth();
            health.rtc_ok    = acq.rtc_ok;
            health.gps_ok    = acq.gps_ok;
            health.sensor_ok = acq.sensor_ok;
            health.data_ok   = acq.data_ok;

            Acquisition_Print(&measure);
            if (!acq.rtc_ok)    printf("  /!\\ Horloge RTC injoignable\r\n\r\n");
            if (!acq.gps_ok)    printf("  /!\\ GPS : aucune trame recue (cablage port A0 ?)\r\n\r\n");
            if (!acq.sensor_ok) printf("  /!\\ Capteur en erreur : 2 mesures de suite sans reponse\r\n\r\n");
            if (!acq.data_ok)   printf("  /!\\ Valeur hors des seuils : verifier le capteur\r\n\r\n");

            // Standard et économique : enregistrement sur la carte SD (pas en maintenance)
            if (mode_actuel == MODE_STANDARD || mode_actuel == MODE_ECONOMIQUE) {
                DataLog_Write(&measure);
                health.sd_ok       = DataLog_IsOk();
                health.sd_space_ok = DataLog_HasSpace();
                if (!health.sd_space_ok) printf("  /!\\ Carte SD pleine\r\n\r\n");
                else if (!health.sd_ok)  printf("  /!\\ Carte SD : erreur d'acces ou d'ecriture\r\n\r\n");

                printf("  Prochaine mesure dans %lu min\r\n\r\n",
                       (unsigned long)(acquisition_period_ms(mode_actuel) / 60000UL));
            }
        }

        // E. Mise à jour de la LED RGB
        SystemStatus_Update(&status, mode_actuel, health);
    }
}
