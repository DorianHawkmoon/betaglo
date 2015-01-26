#include "Arduino.h"
#include "ProcessCommands.h"

SoftwareSerial mySerial(12,13); // RX, TX

void setupBluetooth(){
  mySerial.begin(9600);
}

/*
 * Condicion imprescindible que la cadena termine con \n
 */
void sendBluetooth(String value){
  // Length (with one extra character for the null terminator and \n)
  int strLen = value.length() + 2; 

  // Prepare the character array (the buffer) 
  char charArray[strLen];

  // Copy it over 
  value.toCharArray(charArray, strLen);
  charArray[strLen-2]='\n';
  charArray[strLen-1]='\0';
  
  if (mySerial.available()) {
    mySerial.write(charArray);    
  }
}



//guardo una lista de variables que me permite saber el estado actual del guante
//es decir, por ejemplo estoy actualmente controlando el raton. Un toque largo del meñique cambiaria al estado de teclado
//por lo que los botones pasarian a hacer otras acciones

void processCommands(ClickButton buttons[],boolean flexorActived[], int flexor[]){
  //comprueba que botones y flexores estan activados, para seleccionar un estado de la enumeracion de estados y mandarlo
  //por bluetooth, donde el ordenador recibira la orden y la ejecutara
}
