# Worldwide Weather Watcher

Station météorologique embarquée destinée à la surveillance des paramètres
environnementaux susceptibles d'influencer la formation de cyclones et
d'autres catastrophes naturelles.

Le projet est réalisé dans le cadre du projet système embarqué du CESI. La
station a vocation à être installée à bord de navires et à être utilisée
simplement par un membre de l'équipage.

## Objectifs

La station doit permettre de :

- mesurer les paramètres météorologiques et environnementaux ;
- horodater les mesures ;
- enregistrer les données sur une carte SD ;
- informer l'utilisateur de l'état du système grâce à une LED RGB ;
- être pilotée à l'aide de deux boutons poussoirs ;
- proposer plusieurs modes de fonctionnement adaptés à l'utilisation à bord.

## Matériel

Le prototype s'appuie sur une carte **STM32 Nucleo-L476RG**, équipée d'un
microcontrôleur ARM Cortex-M4, et sur les composants suivants :

| Composant | Interface | Fonction |
| --- | --- | --- |
| Lecteur de carte SD | SPI | Sauvegarde des mesures |
| Horloge RTC DS1307 | I2C | Date et heure des mesures |
| LED RGB Grove | 2 fils | Indication de l'état du système |
| Deux boutons poussoirs | Numérique | Interaction avec l'utilisateur |
| Capteur BME680 | I2C | Pression, température et hygrométrie |
| GPS Air530Z | UART | Position et données GPS |
| Capteur de luminosité Grove | Analogique | Mesure de la luminosité |

Des extensions pourront être intégrées ultérieurement :

- température de l'eau ;
- force du courant marin ;
- force du vent ;
- concentration de particules fines.

### Câblage principal

Le câblage détaillé est défini dans
[`include/board_pins.h`](include/board_pins.h). Les interfaces principales
sont les suivantes :

- **I2C1** : PB8 (SCL) et PB9 (SDA) pour la RTC et le BME680 ;
- **SPI1** : PA5 (SCK), PA6 (MISO) et PA7 (MOSI) pour le lecteur SD ;
- **UART4** : PA0 (TX) et PA1 (RX) pour le GPS ;
- **USART2** : PA2 (TX) et PA3 (RX) pour le moniteur série ;
- **luminosité** : PA4, entrée ADC ;
- **carte SD** : CS sur PB5 ;
- **LED RGB** : données sur PA8 et horloge sur PB10.

> Le shield SD doit être relié aux broches SPI de la carte Nucleo à l'aide de
> fils Dupont. Vérifier le câblage et l'alimentation avant toute mise sous
> tension.

## Modes de fonctionnement

L'architecture prévoit quatre modes :

- **Standard** : fonctionnement nominal et acquisition des mesures ;
- **Configuration** : réglage des paramètres de la station ;
- **Économique** : réduction de la consommation énergétique ;
- **Maintenance** : diagnostic et vérification du matériel.

Les fonctionnalités de chaque mode seront complétées au fur et à mesure de
l'avancement du projet.

## Architecture du dépôt

```text
.
├── include/              # En-têtes du projet et définition du câblage
├── lib/cesi_grove/       # Drivers des composants Grove
├── src/                  # Code applicatif et initialisation STM32
├── test/                 # Tests et documentation de test
├── .vscode/              # Recommandations pour l'environnement VS Code
└── platformio.ini        # Configuration de compilation PlatformIO
```

## Prérequis

- Visual Studio Code ;
- extension [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) ;
- carte STM32 Nucleo-L476RG ;
- composants et câblage décrits dans la section [Matériel](#matériel).

Le projet utilise :

- **PlatformIO** ;
- **STM32Cube HAL** ;
- la plateforme `ststm32` ;
- l'environnement `nucleo_l476rg`.

## Compiler le projet

Depuis la racine du dépôt :

```bash
pio run
```

Pour nettoyer les fichiers générés :

```bash
pio run --target clean
```

Pour téléverser le programme sur la carte, connecter la Nucleo puis exécuter :

```bash
pio run --target upload
```

Le moniteur série est configuré à **115200 bauds** :

```bash
pio device monitor
```

Au démarrage, le programme affiche l'état d'initialisation sur le port série
et traite les actions des boutons.

## État actuel

Le socle du firmware est en place :

- initialisation STM32Cube HAL ;
- initialisation des interfaces I2C, SPI et UART ;
- drivers Grove intégrés au projet ;
- gestion initiale des boutons et de la LED RGB ;
- déclaration des quatre modes de fonctionnement ;
- configuration PlatformIO pour la Nucleo-L476RG.

Les fonctions d'acquisition complète, d'enregistrement des mesures et de
gestion détaillée des modes restent à développer et à valider sur le
prototype.

## Livrables du projet

Le projet est organisé autour des livrables suivants :

1. **Analyse du système** : diagrammes UML/SysML, algorithmes et analyse
   fonctionnelle ;
2. **Architecture du programme** : structure des modules, fonctions et
   variables ;
3. **Maquette** : démonstration du programme et du montage sur la Nucleo ;
4. **Documentation** : documentation technique et guide utilisateur.

## Ressources

- [Documentation STM32 Nucleo-L476RG](https://stm32python.gitlab.io/fr/docs/Kit/nucleo_l476rg)
- [Dépôt d'architecture Worldwide Weather Watcher](https://github.com/ValentinMoraine/Worldwide-Weather-Watcher)
- [Documentation PlatformIO](https://docs.platformio.org/)
- [Ressources pédagogiques du projet CESI](https://scenari.cesi.fr/scenari/cas/IDB_105_CPI_A2_Informatique___S3E_26_27_Septembre_Etudiant_FR/8_-_Projet.html)

## Équipe

Projet réalisé par l'équipe du dépôt
[samyr13/Projet-Worldwide-Weather-Watcher](https://github.com/samyr13/Projet-Worldwide-Weather-Watcher).
