#include "Arduino.h"
#include "Enumerations.h"

//lista de comandos a realizar segun las activaciones realizadas
//tengo que pasarle el array de botones, el array de flexores y el array de los valores de los flexores
void processCommands(ClickButton buttons[],boolean flexorActived[], int flexor[]);
