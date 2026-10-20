#ifndef VERSION_H
#define VERSION_H

// Affichés par la commande VERSION (traçabilité de la production)
#define FW_VERSION    "1.0.0"
#define BATCH_NUMBER  "LOT-2026-001"

// Numéro de build : généré par televerser.sh à chaque compilation (0 si compilé sans le script)
#if defined(__has_include)
#if __has_include("build_number.h")
#include "build_number.h"
#endif
#endif
#ifndef BUILD_NUMBER
#define BUILD_NUMBER 0
#endif

// Mode de compilation choisi dans televerser.sh (environnement PlatformIO)
#ifdef __PLATFORMIO_BUILD_DEBUG__
#define BUILD_TYPE "debug"
#else
#define BUILD_TYPE "release"
#endif

#endif // VERSION_H
