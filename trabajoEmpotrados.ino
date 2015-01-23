#include "Enumerations.h"
#include "ProcessCommands.h"
#include "ComunicacionBluetooth.h"

const int DEDOS = 4;

//state of the hand
int flexs[DEDOS];
int pressures[DEDOS];

//previous state of the hand
int previousFlexs[DEDOS];
int previousPressures[DEDOS];

//processed state of hand
ClickButton buttons[DEDOS];
boolean flexorActived[DEDOS]; //the value of them is stored in flexs (no need to process them)


//values of the flex sensor
const int minimumSensorFlex = 10;
const int maximumSensorFlex = 40;
//values which will use with the flex sensor
const int minimumFlex = 0;
const int maximumFlex = 100;
//dead zone flexion
const int deadFlexion=20;

//values of the pressure sensor
const int minimumSensorPressure = 10;
const int maximumSensorPressure = 40;
//values which will use with the pressure sensor
const int minimumPressure = 0;
const int maximumPressure = 100;
//dead zone pressure
const int deadPressure = 10;
//strong pressure
const int strongPressure = 50;

//time of pressing sensor
unsigned long timePressure[DEDOS];
unsigned long timer=0;
unsigned long timeResponse=200;

const int FLEXOR = 1;
const int PRESSURE = 2;

//pins of sensors
int flexSensorPin[DEDOS];
int pressureSensorPin[DEDOS];


/**
 * Read a concrete sensor given the type of sensor and the number
 */
int valueOfSensor(int typeSensor, int sensor){
  int valueSensor = 0;
  int valueFinal = 0;

  if(typeSensor == FLEXOR){
    valueSensor = analogRead(flexSensorPin[sensor]);
    Serial.println("Flexor: " + valueSensor);
    valueFinal = map(valueSensor, minimumSensorFlex, maximumSensorFlex, minimumFlex, maximumFlex);
    Serial.println("Flexor process: " + valueFinal);

  }
  else if(typeSensor == PRESSURE){
    valueSensor = analogRead(pressureSensorPin[sensor]);
    Serial.println("Pressure: " + valueSensor);
    valueFinal = map(valueSensor, minimumSensorPressure, maximumSensorPressure, minimumPressure, maximumPressure);
    Serial.println("Pressure process: " + valueFinal);

  }

  Serial.println();
  return valueFinal;
}

/**
 * Read the whole state of the hand (flexion and contact)
 */
void readState(){
  //read the values of the flex's sensors
  for (int i=0; i<DEDOS; i++){
    flexs[i] = valueOfSensor(FLEXOR, i);
  }

  //read the values of the pressure's sensors
  for (int i=0; i<DEDOS; i++){
    pressures[i] = valueOfSensor(PRESSURE, i);
  }
}

/**
 * Dado el delay de la ultima vez que se proceso y el boton que se quiere mirar
 * se comprueba si ha habido pulsacion, pulsacion fuerte/larga o nada
 * devuelve un int que corresponde a la enumeracion
 */
ClickButton processButton(int delay, int button){
  int compareStates= ((previousPressures[button]-deadPressure) > 0) ? 1:0;
  compareStates= ((pressures[button]-deadPressure) > 0) ? compareStates+1:compareStates;
  boolean touch=false;
  boolean strong=false;

  //comprobamos fuerza, si la supera, es strong y touch
  if(pressures[button]>=(strongPressure+deadPressure)){
    strong=true;
    touch=true;

    //boton pulsado antes y despulsado, cuenta como toque
  }
  else  if( compareStates == 2 ){
    touch=true;

    //boton se mantiene pulsado
  }
  else if(compareStates == 3){
    //sumamos el tiempo transcurrido
    timePressure[button]+=delay;
    //si ha superado el tiempo, cuenta como toque
    if(timePressure[button]>timeResponse){
      touch=true;
    }
  }

  //procesamos resultado
  if(touch){
    //seteamos el actual valor como no pulsado para que no se vuelva a tener en cuenta el siguiente frame
    pressures[button]=0;
    //igualmente reseteamos su tiempo transcurrido
    timePressure[button]=0;

    //proceso fuerza y accion
    if(strong){
      return long_click;
    }
    else{
      return normal_click;
    }
  }
  return no_click;
}

/**
 * Proceso el estado al completo de la mano y actuo en consecuencia
 */
int processState(int delay){
  //ante el procesado, escribir por pantalla que accion ha realizado, despues
  //refinamos haciendo los envios por bluetooth

  //comprobamos...
  for (int i=0; i<DEDOS; i++){
    //comprobamos botones
    buttons[i]=processButton(delay, i);
    switch(buttons[i]){
    case normal_click:
      Serial.println("Click normal de "+i);
      break;
    case long_click:
      Serial.println("Click fuerte de "+i);
      break;
    case no_click:
      break;
    }

    //comprobamos flexores
    if((flexs[i]-deadFlexion) > 0){
      flexorActived[i]=true;  
    }
    else{
      flexorActived[i]=false;
    }
  }
}

/**
 * Mueve el estado actual a un estado previo para poder leer el siguiente estado
 */
void nextState(){
  for (int i=0; i<DEDOS; i++){
    previousFlexs[i] = flexs[i];
    previousPressures[i] = pressures[i];
  }
}








void setup(){
  Serial.begin(9600);

  //TODO setear los pines
  pressures[0]=8;
    pressures[0]=10;
      pressures[0]=12;
        pressures[0]=14;
         

  for (int i=0; i<DEDOS; i++){
    timePressure[i]=0;
    flexorActived[i]=false;
    flexs[i] = 0;
    pressures[i] = 0;
  }
  //copio los valores al previous
  readState();

  timer=millis();
}

void loop(){
  readState();
  processState(millis()-timer);
  //processCommands();
  timer=millis();
  nextState();
}

