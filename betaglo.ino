#include "Enumerations.h"
#include "ProcessCommands.h"
#include <SoftwareSerial.h>




const int DEDOS = 4;
const int FLEXORS = 5;

//state of the hand
int flexs[DEDOS];
int pressures[DEDOS];
// variable que guarda si el dedo presion paso del maximo (toque fuerte)
boolean maxPressures[DEDOS];

//previous state of the hand
int previousFlexs[DEDOS];
int previousPressures[DEDOS];

//processed state of hand
ClickButton buttons[DEDOS];
boolean flexorActived[DEDOS]; //the value of them is stored in flexs (no need to process them)


//values of the flex sensor
const int minimumSensorFlex = 0;
const int maximumSensorFlex = 45;
//values which will use with the flex sensor
const int minimumFlex = 0;
const int maximumFlex = 100;
//dead zone flexion
const int deadFlexion=20;
int initialPosition[DEDOS];

//values of the pressure sensor
const int minimumSensorPressure = 10;
const int maximumSensorPressure = 40;
//values which will use with the pressure sensor
const int minimumPressure = 0;
const int maximumPressure = 100;
//dead zone pressure
const int deadPressure = 70;
//strong pressure
const int strongPressure = 240;

//time of pressing sensor
unsigned long timePressure[DEDOS];
unsigned long timeWait[DEDOS];
unsigned long timer=0;
unsigned long timeResponse=1000;
unsigned long timeWaitResponse=1000;

const int FLEXOR = 1;
const int PRESSURE = 2;

//pins of sensors
int flexSensorPin[DEDOS];
int pressureSensorPin[FLEXORS];


/**
 * Read a concrete sensor given the type of sensor and the number
 */
int valueOfSensor(int typeSensor, int sensor){
  int valueSensor = 0;
  int valueFinal = 0;

  //segun si estoy mirando flexor o presion
  if(typeSensor == FLEXOR){
    valueSensor = analogRead(flexSensorPin[sensor]);
    valueFinal = map(valueSensor, minimumSensorFlex, maximumSensorFlex, minimumFlex, maximumFlex);

  }
  else if(typeSensor == PRESSURE){
    int sensorButton=sensor+1;
    valueFinal = analogRead(pressureSensorPin[sensorButton]);
  }

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
    pressures[i]=valueOfSensor(PRESSURE, i);
  }
}

/**
 * Dado el delay de la ultima vez que se proceso y el boton que se quiere mirar
 * se comprueba si ha habido pulsacion, pulsacion fuerte/larga o nada
 * devuelve un int que corresponde a la enumeracion
 */
ClickButton processButton(int delay, int button){
  /* Estados: 0 -> estados anterior y actual no pulsados
   * 1 -> anterior estado pulsado
   * 2 -> estado actual pulsado
   * 3 -> estado anterior y actual pulsado
   */
  int compareStates= ((previousPressures[button]-deadPressure) > 0) ? 1:0;
  compareStates= ((pressures[button]-deadPressure) > 0) ? compareStates+2:compareStates;
  boolean touch=false;
  boolean strong=false;
   

  //si no ha pasado suficiente tiempo de la anterior pulsacion y no coincide con que haya pulsado y despulsado
  //sumamos el tiempo de espera entre toque y toque y salimos
  if(timeWait[button]<0 && compareStates != 1){
    //sumamos el tiempo transcurrido
    timeWait[button]+=delay;
    return no_click;
  }
  
  //si hay suficiente fuerza...
  if(pressures[button]>=(strongPressure+deadPressure)){
    strong=true;
    maxPressures[button]=true;
  } 

  //boton pulsado antes y despulsado, cuenta como toque
  if( compareStates == 1 ){
    touch=true;
  }

  //boton se mantiene pulsado
  else if(compareStates == 3 ){
    //sumamos el tiempo transcurrido
    timePressure[button]+=delay;
    //si ha superado el tiempo, cuenta como toque
    if(timePressure[button]>timeResponse){
      touch=true;
    }
  }

  else if(compareStates == 2){
    //acaba de pulsar, no hacemos nada
  }




  //procesamos resultado
  if(touch){
    //seteamos el actual valor como no pulsado para que no se vuelva a tener en cuenta el siguiente frame
    pressures[button]=0;
    //igualmente reseteamos su tiempo transcurrido y le ponemos un tiempo negativo para que no haga varias pulsaciones
    //del mismo toque
    timePressure[button]=0;
    //establecemos el tiempo de espera antes de considerar otro toque del mismo boton
    timeWait[button]=(timeWaitResponse*(-1));
    //miramos si hubo toque fuerte
    strong=maxPressures[button];
    //proceso fuerza y accion
    if(strong){
      //reseteamos la presion fuerte para el siguiente toque
      maxPressures[button]=false;
      return long_click;
    }else{
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
      //Serial.print("Click normal de ");
      //Serial.println(i);
      break;
    case long_click:
      //Serial.print("Click fuerte de ");
      //Serial.println(i);
      break;
    case no_click:
      break;
    }

    //comprobamos flexores
    if((flexs[i]<=(initialPosition[i]-deadFlexion))
        || (flexs[i]>=(initialPosition[i]+deadFlexion))  ){
      flexorActived[i]=true; 
      //Serial.print("Flexor ");
      //Serial.print(i);
      //Serial.print(" activado: ");
      //Serial.println(flexs[i]);
    }else{
      flexorActived[i]=false;
      //Serial.print("Flexor ");
      //Serial.print(i);
      //Serial.print(" desactivado: ");
      //Serial.println(flexs[i]);
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
  setupBluetooth();

  //TODO setear los pines
  pressureSensorPin[0]=0;
  pressureSensorPin[1]=1;
  pressureSensorPin[2]=2;
  pressureSensorPin[3]=3;
  pressureSensorPin[4]=4;

  //pins of sensors
  flexSensorPin[0]=6;
  flexSensorPin[1]=7;
  flexSensorPin[2]=8;
  flexSensorPin[3]=9;
  flexSensorPin[4]=10;

  for (int i=0; i<DEDOS; i++){
    timePressure[i]=0;
    timeWait[i]=0;
    flexorActived[i]=false;
    flexs[i] = 0;
    maxPressures[i]=false;
    pressures[i] = 0;
    initialPosition[i]=map(analogRead(flexSensorPin[i]), 
                            minimumSensorFlex, maximumSensorFlex, 
                            minimumFlex, maximumFlex);
  }

  //copio los valores al previous
  readState();

  timer=millis();
}

void loop(){
  readState();
  processState(millis()-timer);
  processCommands(buttons, flexorActived, flexs);
  timer=millis();
  nextState();
}






