# ✅ Checklist — Station météo Worldwide Weather Watcher

> Ce qui est demandé dans le PDF *« Modes de fonctionnement »*, ce qui est fait, et ce qu'il reste.
>
> **✅ fait et testé** · **🧪 fait, à tester par vous** · **❌ pas encore fait**

---

## 📊 Où on en est

```
Capteurs et matériel   ██████████  fini (sauf carte SD)
LED d'état             ██████████  fini (sauf erreurs SD)
Mode standard          ███████░░░  reste : écriture sur la carte SD
Mode configuration     ██████████  fini
Mode maintenance       ████████░░  reste : retrait de la carte SD
Mode économique        ██████████  fini
Carte SD               ░░░░░░░░░░  à faire
Livrables              ░░░░░░░░░░  à faire
```

---

## 🔌 1. Le matériel

- ✅ **Température, humidité, pression** : capteur BME280, sur un port **I2C**
- ✅ **Horloge (RTC)** : sur un port **I2C**
- ✅ **GPS** : sur le port **A0**
- ✅ **Luminosité** : sur le port **A2**
- ✅ **LED** : sur le port **D2**
- ✅ **Boutons rouge et vert** : sur le port **D7**
- ❌ **Carte SD** : ne répond pas, mise de côté pour l'instant

---

## 🔁 2. Les 4 modes et les boutons

**Le bouton vert entre et sort de l'économique. Le bouton rouge entre et sort de la maintenance.**

```
🟢 STANDARD   ──vert 5 s──►  🔵 ÉCONOMIQUE ──vert 5 s──►  🟢 STANDARD

🟢 STANDARD   ──rouge 5 s─►  🟠 MAINTENANCE ──rouge 5 s─► 🟢 STANDARD
🔵 ÉCONOMIQUE ──rouge 5 s─►  🟠 MAINTENANCE ──rouge 5 s─► 🔵 ÉCONOMIQUE
                                             (retour au mode d'avant)

Démarrage avec le bouton rouge appuyé ──► 🟡 CONFIGURATION
                                          retour en standard après 30 min sans activité
```

- ✅ Démarrage normal → mode **standard**
- ✅ Démarrage avec le bouton rouge appuyé → mode **configuration**
- ✅ Vert 5 s en standard → mode **économique**
- 🧪 Vert 5 s en économique → retour en **standard**
- ✅ Rouge 5 s en standard → mode **maintenance**
- 🧪 Rouge 5 s en économique → mode **maintenance**
- ✅ Rouge 5 s en maintenance → retour au **mode d'avant** (standard ou économique)
- ✅ Configuration → retour en standard après **30 min sans activité** (le délai repart à chaque commande ou appui)
- 🧪 Ce retour automatique après 30 min n'a pas encore été testé en vrai (trop long à attendre)

> ⚠️ **À expliquer dans le rapport :** le PDF demande deux choses pour le même geste en mode économique (rouge 5 s) : aller en maintenance, et revenir en standard. Nous avons gardé le rouge pour la maintenance, comme dans les autres modes, et utilisé le **vert** pour revenir en standard. Chaque bouton a ainsi un seul rôle, facile à retenir.

---

## 💡 3. La LED

| La LED fait… | Ça veut dire… | |
|---|---|---|
| 🟢 vert fixe | mode standard | ✅ |
| 🟡 jaune fixe | mode configuration | ✅ |
| 🔵 bleu fixe | mode économique | ✅ |
| 🟠 orange fixe | mode maintenance | ✅ |
| 🔴🔵 rouge / bleu | l'horloge (RTC) ne répond pas | 🧪 débrancher la RTC pour tester |
| 🔴🟡 rouge / jaune | le GPS ne répond pas | ✅ |
| 🔴🟢 rouge / vert | un capteur ne répond pas | ✅ luminosité · 🧪 débrancher le BME280 pour tester |
| 🔴🟢 rouge / vert *(vert 2× plus long)* | une valeur mesurée est anormale | ✅ |
| 🔴⚪ rouge / blanc | la carte SD est pleine | ❌ en attente de la carte SD |
| 🔴⚪ rouge / blanc *(blanc 2× plus long)* | erreur de la carte SD | ❌ en attente de la carte SD |

---

## 🌡️ 4. Mode standard *(LED verte)*

- ✅ Mesure tous les capteurs toutes les **LOG_INTERVAL** minutes (10 min par défaut)
- ✅ Chaque mesure est **datée** grâce à l'horloge
- ✅ Un capteur qui ne répond pas affiche **« NA »** (indisponible)
- ✅ Après **2 mesures** sans réponse, le capteur est signalé en erreur sur la LED
- ✅ Luminosité classée **faible / moyenne / forte**
- ✅ Valeur hors des seuils → LED « valeur anormale »
- ❌ Enregistrer les mesures sur la **carte SD**, dans des fichiers `AAMMJJ_0.LOG`
- ❌ Quand un fichier atteint **FILE_MAX_SIZE**, le copier en `_1`, `_2`… et recommencer en `_0`

---

## ⚙️ 5. Mode configuration *(LED jaune)*

**Pour y entrer :** maintenir le bouton rouge appuyé, appuyer sur le bouton **reset** de la carte, puis relâcher.
Les commandes se tapent dans le **moniteur série**, puis Entrée.

- ✅ Les mesures sont **arrêtées** dans ce mode
- ✅ Les réglages sont **gardés en mémoire**, même après avoir débranché la carte
- ✅ Une valeur hors limites est **refusée** avec un message clair
- ✅ Toutes les commandes du PDF fonctionnent :

| Commande | Ce qu'elle fait | Exemple |
|---|---|---|
| `NOM=valeur` | change un réglage | `LOG_INTERVAL=5` |
| `RESET` | remet tous les réglages d'usine | |
| `VERSION` | affiche la version et le numéro de lot | |
| `CLOCK=` | règle l'heure | `CLOCK=14:30:00` |
| `DATE=` | règle la date (mois, jour, année) | `DATE=10,12,2026` |
| `DAY=` | règle le jour de la semaine | `DAY=MON` |
| `CONFIG` | affiche tous les réglages *(en plus du PDF)* | |
| `HELP` | affiche l'aide *(en plus du PDF)* | |
| `EXIT` | revient au mode standard *(en plus du PDF)* | |

<details>
<summary><b>📋 Tous les réglages possibles (cliquer pour afficher)</b></summary>

| Réglage | Rôle | Valeurs | Par défaut |
|---|---|---|---|
| `LOG_INTERVAL` | minutes entre 2 mesures | 1 à 1440 | 10 |
| `FILE_MAX_SIZE` | taille max d'un fichier (octets) | 512 à 32768 | 2048 |
| `TIMEOUT` | secondes d'attente d'un capteur | 1 à 300 | 30 |
| `LUMIN` | capteur de luminosité allumé | 0 ou 1 | 1 |
| `LUMIN_LOW` | en dessous = luminosité faible | 0 à 1023 | 255 |
| `LUMIN_HIGH` | au-dessus = luminosité forte | 0 à 1023 | 768 |
| `TEMP_AIR` | capteur de température allumé | 0 ou 1 | 1 |
| `MIN_TEMP_AIR` | température minimale (°C) | -40 à 85 | -10 |
| `MAX_TEMP_AIR` | température maximale (°C) | -40 à 85 | 60 |
| `HYGR` | capteur d'humidité allumé | 0 ou 1 | 1 |
| `HYGR_MINT` | humidité ignorée en dessous de (°C) | -40 à 85 | 0 |
| `HYGR_MAXT` | humidité ignorée au-dessus de (°C) | -40 à 85 | 50 |
| `PRESSURE` | capteur de pression allumé | 0 ou 1 | 1 |
| `PRESSURE_MIN` | pression minimale (hPa) | 300 à 1100 | 850 |
| `PRESSURE_MAX` | pression maximale (hPa) | 300 à 1100 | 1080 |

</details>

---

## 🔧 6. Mode maintenance *(LED orange)*

- ✅ Les mesures s'affichent **en direct** sur le moniteur série (toutes les 5 s)
- ✅ Plus rien n'est écrit sur la carte SD
- ❌ Pouvoir retirer la carte SD sans risque (à faire avec la carte SD)

---

## 🔋 7. Mode économique *(LED bleue)*

- ✅ Mesures **2 fois moins souvent** (LOG_INTERVAL × 2, soit 20 min par défaut)
- ✅ GPS lu **une mesure sur deux** seulement

---

## 💾 8. Carte SD — ❌ à faire

- ❌ Faire répondre la carte (vérifier la carte, les fils et l'alimentation)
- ❌ Écrire les fichiers de mesures
- ❌ Changer de fichier quand il est plein
- ❌ Détecter la carte pleine et les erreurs (LED rouge / blanc)

---

## 📄 9. Livrables — ❌ à faire

- ❌ Diagrammes (UML / SysML) et algorithmes
- ❌ Architecture du programme (fichiers, fonctions)
- ❌ Démonstration sur la carte
- ❌ Documentation technique et guide utilisateur

---

## 🧪 À tester par vous

1. **Mode configuration** : démarrer avec le rouge appuyé, taper `VERSION`, puis `LOG_INTERVAL=1`, puis `EXIT` → une mesure par minute.
2. **Mode économique** : vert 5 s → la position GPS ne s'affiche qu'une mesure sur deux.
3. **Panne de l'horloge** : débrancher la RTC → LED rouge / bleu.
4. **Panne du capteur** : débrancher le BME280 → « indisponible », puis LED rouge / vert.
5. **GPS** : le poser près d'une fenêtre → la position s'affiche après quelques minutes.

---

## 📝 Choix à expliquer dans le rapport

- **Boutons en mode économique** : le rouge mène en maintenance, le vert ramène en standard (voir section 2).
- **Attente des capteurs** : le GPS et la luminosité répondent tout de suite, donc on ne les attend que 3 s au lieu des 30 s du TIMEOUT. Sinon, chaque mesure serait bloquée 30 s quand un capteur est débranché.
- **Luminosité débranchée** : on charge l'entrée vers 0 V puis vers 3,3 V. Si les deux lectures sont très différentes, c'est que rien n'est branché.
- **Mémoire des réglages** : la carte n'a pas d'EEPROM, les réglages sont donc enregistrés dans sa mémoire flash.
- **Maintenance « en direct »** : une mesure toutes les 5 s, pour voir les valeurs bouger.
- **Commandes en plus** : `CONFIG`, `HELP` et `EXIT`, pour faciliter l'utilisation et les démonstrations.
