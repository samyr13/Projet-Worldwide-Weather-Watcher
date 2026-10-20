#!/usr/bin/env bash
#
# Station météo 3W : compile le programme, le téléverse sur la carte
# NUCLEO-L476RG, puis ouvre le moniteur série.
#
# Utilisation sous Windows (Git Bash) :
#   - double-cliquer sur televerser.sh (le mode est demandé) ;
#   - ou, dans un terminal Git Bash : ./televerser.sh [release|debug]
#
# Chaque compilation incrémente le numéro de build (include/build_number.h),
# affiché par la commande VERSION du mode configuration.
#
# Quitter le moniteur série : Ctrl+C

# Toujours travailler dans le dossier du projet, d'où que le script soit lancé
cd "$(dirname "$0")" || exit 1

# Garde la fenêtre ouverte en cas d'erreur (utile après un double-clic)
fin_erreur() {
    echo
    echo "ERREUR : $1"
    read -r -p "Appuyer sur Entree pour fermer..."
    exit 1
}

# 1. Recherche de PlatformIO : commande pio, sinon installation de l'extension VS Code
if command -v pio >/dev/null 2>&1; then
    PIO="pio"
elif [ -x "$HOME/.platformio/penv/Scripts/pio.exe" ]; then
    PIO="$HOME/.platformio/penv/Scripts/pio.exe"
else
    fin_erreur "PlatformIO introuvable. Installer l'extension PlatformIO IDE dans VS Code."
fi

# 2. Mode de compilation : release (mise en service) ou debug (débogage)
MODE="$1"
if [ -z "$MODE" ]; then
    read -r -p "Mode de compilation : [r]elease ou [d]ebug ? (r par defaut) " choix
    case "$choix" in
        d|D|debug) MODE="debug" ;;
        *)         MODE="release" ;;
    esac
fi
case "$MODE" in
    release) ENV="nucleo_l476rg" ;;
    debug)   ENV="nucleo_l476rg_debug" ;;
    *)       fin_erreur "mode inconnu : $MODE (release ou debug)" ;;
esac

# 3. Numéro de build : lu dans include/build_number.h puis incrémenté
FICHIER_BUILD="include/build_number.h"
BUILD=0
if [ -f "$FICHIER_BUILD" ]; then
    BUILD=$(sed -n 's/^#define BUILD_NUMBER \([0-9]*\).*/\1/p' "$FICHIER_BUILD")
    BUILD=${BUILD:-0}
fi
BUILD_PRECEDENT=$BUILD
BUILD=$((BUILD + 1))
ecrire_build() {
    echo "// Généré par televerser.sh : ne pas modifier à la main" > "$FICHIER_BUILD"
    echo "#define BUILD_NUMBER $1" >> "$FICHIER_BUILD"
}

echo "=============================================="
echo "   STATION METEO 3W - COMPILATION ET ENVOI"
echo "   mode $MODE - build $BUILD"
echo "=============================================="

# 4. La carte doit être branchée en USB (port série du ST-Link)
echo
echo "[1/4] Recherche de la carte..."
if ! "$PIO" device list | grep -q "VID:PID=0483:374B"; then
    fin_erreur "carte NUCLEO non detectee. Brancher le cable USB et reessayer."
fi
echo "Carte detectee."

# 5. Compilation (le numéro de build n'est gardé que si elle réussit)
echo
echo "[2/4] Compilation..."
ecrire_build "$BUILD"
if ! "$PIO" run -e "$ENV"; then
    ecrire_build "$BUILD_PRECEDENT"
    fin_erreur "la compilation a echoue (voir les messages ci-dessus)."
fi

# 6. Téléversement par le ST-Link
echo
echo "[3/4] Televersement sur la carte..."
"$PIO" run -e "$ENV" --target upload || fin_erreur "le televersement a echoue. Fermer les logiciels qui utilisent le ST-Link (STM32CubeProgrammer, session de debug) et reessayer."

# 7. Moniteur série (115200 bauds, réglé dans platformio.ini)
echo
echo "[4/4] Ouverture du moniteur serie (Ctrl+C pour quitter)..."
echo
"$PIO" device monitor -e "$ENV" || fin_erreur "moniteur serie inaccessible. Fermer le moniteur deja ouvert (VS Code, PuTTY...) et reessayer."
