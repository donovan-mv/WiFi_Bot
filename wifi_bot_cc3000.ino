#include <Adafruit_CC3000.h>
#include <Adafruit_CC3000_Server.h>
#include <ccspi.h>

//initialise the coonections; see example for details
Adafruit_CC3000 wifi = Adafruit_CC3000(10, 2, 7, SPI_CLOCK_DIVIDER);
//initialise a server object on port 80
Adafruit_CC3000_Server server(80);
//innitialise the other variables that are required
String data="";
boolean flag = 0;
byte curr_spd;
int curr_dir;

void setup() {
  Serial.begin(115200);
  Serial.println("Initialising...");
  //the begin() method starts SPI communication with the module
  //it returns a boolean value that can be used to chack if the module is connected properly
  if(!wifi.begin()){
    Serial.println("ERROR: Could not connect to CC3000");
    return;
  }
  Serial.println("CC3000 Initialised successfully");
  //next we need to connect to an access point 
  //this method also returns a boolean val that is used to check if it is connected to AP
  if(!wifi.connectToAP("ES_3700", "", WLAN_SEC_UNSEC)){
    Serial.println("ERROR:Could not connect to AP");
    return;
  }
  Serial.println("Connected to AP");
  //method that actually starts the server on port 80 and starts listening for clients
  server.begin();
  Serial.println("Waiting for clients...\n-----------------------------------------------------------------------------------------\n");
}

void loop() {
  String spd="";
  String dir="";
  //server.available() returns a client object when a client has coonected
  Adafruit_CC3000_ClientRef client = server.available();
  if(client){ //check if client has connected to server
    flag = 1; //set flag to indicate that a client was available
    while(client.available()){ //check if client has sent any request
      data += char(client.read()); //store the entire request in data
    }
  }
  if (flag == 1){ //print the request only if something had been sent
    Serial.println("DATA:\n");
    Serial.print(data);
    flag=0; //reset the flag to 0 for next loop()
  }
  //next part of the code only extracts the dir and spd
  int i = data.indexOf("dir=");
  int j = data.indexOf("spd=");
  //Serial.println(i);
  //Serial.println(j);
  if (i!=-1 && j!=-1){
    client.fastrprintln(F("HTTP/1.1 200 OK")); //send response to client
    dir = data.substring(i+4,j-1);
    spd = data.substring(j+4,data.indexOf("HTTP/1.1")-1);
    Serial.println("Speed="+spd);
    Serial.println("Direction="+dir);
    if (!dir.equals("-1")){
      curr_dir = dir.toInt();
      update();
    }
    else if (!spd.equals("-1")){
      curr_spd = spd.toInt();
      update();
    }
  }
  else{
    client.fastrprintln(F("HTTP/1.1 405 ERROR"));
  }
  data = ""; //clear the data for next loop()
}

void update(){ //control the motors according to data received
  if (curr_dir == 1){
    analogWrite(5, curr_spd);
    analogWrite(6, 0);
    analogWrite(9, curr_spd);
    analogWrite(3, 0);
  }
  else if (curr_dir == 2){
    analogWrite(5, byte(0.5*curr_spd));
    analogWrite(6, 0);
    analogWrite(9, curr_spd);
    analogWrite(3, 0);
  }
  else if (curr_dir == 3){
    analogWrite(5, curr_spd);
    analogWrite(6, 0);
    analogWrite(9, byte(0.5*curr_spd));
    analogWrite(3, 0);
  }
  else if (curr_dir == 4){
    analogWrite(5, 0);
    analogWrite(6, curr_spd);
    analogWrite(9, 0);
    analogWrite(3, curr_spd);
  }
}

