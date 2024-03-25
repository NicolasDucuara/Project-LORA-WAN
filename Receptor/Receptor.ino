#include <SPI.h>
#include <LoRa.h>
#define SS 8
#define RST 4
#define DI0 7
#define BAND 433E6

String datos="", temp = "", presion = "", altura = "", latitud="", longitud="",fecha="",hora="";
int cont=0,i=0,f1=0,f2=0;
int pos[7]={0};

void imprimirData();

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);

  LoRa.setPins(SS, RST, DI0);

  Serial.println("Callback Receptor LoRa");

  if (!LoRa.begin(BAND)) {
    Serial.println("Inicio de LoRa fallido!");
    while (1)
      ;
  }

  // register the receive callback
  LoRa.onReceive(onReceive);
  // put the radio into receive mode
  LoRa.receive();
}

void loop() {
  // do nothing
}

void onReceive(int paqueteSize) {
  // received a packet
  Serial.print("Paquete recibido: '");

  // read packet
  for (int i = 0; i < paqueteSize; i++) {
    char q=(char)LoRa.read();
    if (q=='#'){
      f1=1;
    }else if (q=='*'){
      f2=1;
    }
    datos=datos+q;
    Serial.print(q);
  }

  Serial.print("' with RSSI ");
  Serial.println(LoRa.packetRssi());

  int lc=datos.length();

  for (int i=0;i<lc;i++){
    char c=datos.charAt(i);
    if ((f1==1)&&(f2==1)){
      if (c=='@'){
        pos[cont]=i;
        cont++;
      }
    }
  }
  if ((f1==1) && (f2==1)){
   imprimirData();    
  }
  else {
    Serial.println("Dato Incompleto");
  }
  
  // print RSSI of packet

  f1=0;f2=0;
  datos="";
  cont=0;
  for (i=0;i<8;i++){
    pos[i]=0;
  }
}


void imprimirData(){
  
  temp = datos.substring(1, pos[0]);
  presion = datos.substring(pos[0] + 1, pos[1]);
  altura = datos.substring(pos[1]+ 1, pos[2]);
  latitud = datos.substring(pos[2] + 1, pos[3]);
  longitud = datos.substring(pos[3] + 1, pos[4]);
  fecha=datos.substring(pos[4] + 1, pos[5]);
  hora=datos.substring(pos[5] + 1,pos[6]);

  Serial.println();
  Serial.print(F("Temperatura = "));
  Serial.print(temp);
  Serial.println(" C");

  Serial.print(F("Presion = "));
  Serial.print(presion);
  Serial.println(" Pa");

  Serial.print(F("Altura aprox = "));
  Serial.print(altura);
  Serial.println(" m");

  Serial.print(F("Ubicacion = "));
  Serial.print(latitud); 
  Serial.print(F(","));
  Serial.println(longitud); 

  Serial.print(F("Fecha = "));
  Serial.println(fecha);

  Serial.print(F("Hora = "));
  Serial.println(hora);
  Serial.println();
}