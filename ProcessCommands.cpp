#include "Arduino.h"
#include "ProcessCommands.h"

//guardo una lista de variables que me permite saber el estado actual del guante
//es decir, por ejemplo estoy actualmente controlando el raton. Un toque largo del meñique cambiaria al estado de teclado
//por lo que los botones pasarian a hacer otras acciones

void processCommands(ClickButton buttons[],boolean flexorActived[], int pressures[]){
  //comprueba que botones y flexores estan activados, para seleccionar un estado de la enumeracion de estados y mandarlo
  //por bluetooth, donde el ordenador recibira la orden y la ejecutara
}
