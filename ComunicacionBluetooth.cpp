#include "ComunicacionBluetooth.h"

void sendBluetooth(int value){
  //mandar...
}

void sendHandshaking(){
  sendBluetooth(0);
  sendBluetooth(1023);
  sendBluetooth(0);
}
