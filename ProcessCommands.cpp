#include "Arduino.h"
#include "ProcessCommands.h"
#include "Enumerations.h"

SoftwareSerial mySerial(13,12); // RX, TX
States stateHand;
unsigned long timerMouse;
StatesKeyboard stateKeyboard;
unsigned long timerKeys;
int finger;
int timesFinger;
unsigned long previousTimerKeys;
int limitTimeFinger=500;
char keyboardLettersMayus[][7]={
  { 
    'S','D','U','G','H','Ñ'      }
  ,
  {
    'O','I','T','B','Q','J','K'      }
  ,
  {
    'A','N','C','P','Y','Z','W'       }
  ,
  {
    'E','R','L','M','V','F','X'       }

};

char keyboardLettersMinus[][7]={
  { 
    's','d','u','g','h','ñ'      }
  ,
  {
    'o','i','t','b','q','j','k'      }
  ,
  {
    'a','n','c','p','y','z','w'       }
  ,
  {
    'e','r','l','m','v','f','x'       }
};


//por cada dedo(4), el numero de caracteres que tiene para saber que modulo hacer,
//ordenados segun la enumeracion
int numberChars[][4]={
  {    
    6,7,7,7            }
  ,
  {    
    6,7,7,7            }
};

void setupCommand(){
  mySerial.begin(9600);
  stateHand=mouse;
  timerMouse=0;
  finger=-1;
  timesFinger=-1;
  timerKeys=-1;
  stateKeyboard=minusculas;
}

/*
 * Condicion imprescindible que la cadena termine con \n
 */
void sendBluetooth(String value){
  Serial.println(value);
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

void sendKey(){
  String key="tecla:";
  char charToAppend;
  if(timerKeys!=-1 && timesFinger!=-1){
    
    switch(stateKeyboard){
      case minusculas:
        charToAppend=keyboardLettersMinus[finger][timesFinger];
        break;
      case mayusculas:
        charToAppend=keyboardLettersMayus[finger][timesFinger];
        break;
      case simbolos:
        charToAppend='j';
        break;
      case numeros:
        charToAppend='k';
        break;
    }
    key=key+charToAppend;
  }
  timerKeys=-1;
  timesFinger=-1;
  sendBluetooth(key);
}

void processButtonMouse(int value){
  switch(value){
    //click normal del indice
    case 64:
      sendBluetooth("click");
      break;
  
      //click fuerte del indice
    case 128:
      break;
  
      //click fuerte corazon
    case 32:
      //control c
      break;
  
      //click fuerte anular
    case 8:
      //control v
      break;
      //anular normal
    case 4:
      //control z
      break;
  
  
      //click normal corazon
    case 16:
      sendBluetooth("clickderecho");
      break;
  
  
  
  
      //click fuerte meñique
    case 2:
      Serial.println("changed state");
      stateHand=keyboard;
      stateKeyboard=minusculas;
      break;
  }
}

void processButtonKeyboard(int value){
  //lo primero mirar si queda por escribir alguna letra por espera del tiempo
  if(timerKeys!=-1 && (millis()-timerKeys)>limitTimeFinger){
    sendKey();
  }

  int numberFinger=-1;
  switch(value){
    //click fuerte meñique
    case 2:
      Serial.println("changed state");
      stateHand=mouse;
      break;
      //anular fuerte
    case 8:
      stateKeyboard=numeros;
      break;
      //dedo corazon fuerte
    case 32:
      if(stateKeyboard==minusculas){
        stateKeyboard=mayusculas; 
      }
      else{
        stateKeyboard=minusculas;
      }
      break;
      //click fuerte indice
    case 128:
      stateKeyboard=simbolos;
        break;
  
  
      //indice
    case 64:
      numberFinger=0;
      break;
      //corazon
    case 16:
      numberFinger=1;      
      break;
      //anular
    case 4:
      numberFinger=2;
      break;
      //meñique
    case 1:
      numberFinger=3;
      break;
  }

  if(numberFinger!=-1){
    if(finger!=numberFinger){
      sendKey();
      finger=numberFinger;
    }
    timesFinger=(timesFinger+1)%(numberChars[0][finger]);
    timerKeys=millis();

  }
}

//guardo una lista de variables que me permite saber el estado actual del guante
//es decir, por ejemplo estoy actualmente controlando el raton. Un toque largo del meñique cambiaria al estado de teclado
//por lo que los botones pasarian a hacer otras acciones
void processCommands(ClickButton buttons[],boolean flexorActived[], int flexor[], int DEDOS){
  int processButton=0;
  int result=0;
  //variable con 8 bits, cada dos bits es un dedo
  for(int i=0; i<DEDOS; i++){
    processButton=(int)buttons[i]; //enumeracion convertida a int
    int desplazamiento=2*i;
    int resultDesplazamiento=(processButton<<desplazamiento);
    result= result | resultDesplazamiento;
  }


  //preferencia a los botones para procesar el estado
  if(stateHand==mouse){
    processButtonMouse(result);
  } else if(stateHand==keyboard){
    processButtonKeyboard(result);
  }



  if(stateHand==mouse){
    //procesamos los flexores cuando estamos en modo raton
    if(stateHand==mouse){
      for(int i=0; i<DEDOS; i++){
        int delay=millis()-timerMouse;
        if(flexorActived[i]==true && delay>50){
          int value=flexor[i];
          value = map(value, 0, 100, 1, 20);
          String command="raton:";
          switch(i){
            case 0:
              command=command+value;
              command=command+":0";
              sendBluetooth(command);
              break;
            case 1:
              value=value*(-1);
              command=command+value;
              command=command+":0";
              sendBluetooth(command);
              break;
            case 2:
              value=value*(-1);
              command=command+"0:";
              command=command+value;
              sendBluetooth(command);
              break;
            case 3:
              command=command+"0:";
              command=command+value;
              sendBluetooth(command);
              break;
          }
          timerMouse=millis();
        }
      }
    }
  }
}

