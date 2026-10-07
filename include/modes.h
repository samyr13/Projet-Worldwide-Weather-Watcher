#ifndef MODES_H
#define MODES_H

// Énumération des 4 modes de la station 3W
typedef enum {
    MODE_STANDARD,
    MODE_CONFIGURATION,
    MODE_ECONOMIQUE,
    MODE_MAINTENANCE
} SystemMode;

// Prototype de fonction pour initialiser ou changer de mode
void set_system_mode(SystemMode new_mode);
SystemMode get_current_mode(void);

#endif // MODES_H
