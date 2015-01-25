#include "Arduino.h"
#include "ProcessCommands.h"

SoftwareSerial mySerial(12,13); // RX, TX

void setupBluetooth(){
  mySerial.begin(9600);
}

void sendBluetooth(char* value){
  value="Hola a todo el mundo!";
  String command;
  // Read device output if available.
  if (mySerial.available()) {
    while(mySerial.available()) { // While there is more to be read, keep reading.
      command += (char)mySerial.read();
    }
    Serial.println(command);
    command = ""; // No repeats


    mySerial.write(value);    
  }
}



//guardo una lista de variables que me permite saber el estado actual del guante
//es decir, por ejemplo estoy actualmente controlando el raton. Un toque largo del meñique cambiaria al estado de teclado
//por lo que los botones pasarian a hacer otras acciones

void processCommands(ClickButton buttons[],boolean flexorActived[], int flexor[]){
  //comprueba que botones y flexores estan activados, para seleccionar un estado de la enumeracion de estados y mandarlo
  //por bluetooth, donde el ordenador recibira la orden y la ejecutara
}
