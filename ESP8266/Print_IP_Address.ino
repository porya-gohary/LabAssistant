#include "ESP8266WiFi.h"
#include <ArduinoJson.h>
#include "ESP8266HTTPClient.h"
#include <string.h>

// WiFi parameters to be configured
const char* ssid = "[PORYA]";
const char* password = "Wftltposu5";
String inputString = ""; 
int value1,value2;

void setup(void)
{
  Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid, password);

  // while wifi not connected yet, print '.'
  // then after it connected, get out of the loop
  delay(10000);
  Serial.println();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  //print a new line, then print WiFi connected and the IP address
  //Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println(WiFi.RSSI());
  // Print the IP address
  //Serial.println(WiFi.localIP());

//   for(int i=0;i<1000;i++){
//      delay(1000);
//      sendData(i,i*2);
//      
//    }

  



}
void loop() {

  if (Serial.available() && Serial.find("|S|")){
    
    int analog_in1=Serial.parseInt();
    int analog_in2=Serial.parseInt();
    int digital_in1=Serial.parseInt();
    int digital_in2=Serial.parseInt();
    int analog_out1=Serial.parseInt();
    int analog_out2=Serial.parseInt();
    int digital_out1=Serial.parseInt();
    int digital_out2=Serial.parseInt();
    
     // int t1,t2,t3,t4,t5,t6,t7,t8;
   //   sscanf (Serial.readString(),"%d %d %d %d %d %d %d %d",&t1,&t2,&t3,&t4,&t5,&t6,&t7,&t8);
     // Serial.println(digital_in1,DEC);
//    Serial.println(value1,DEC);
//    Serial.println(value2,DEC);
     sendData(analog_in1,analog_in2,digital_in1,digital_in2,analog_out1,analog_out2,digital_out1,digital_out2);
    
//    if (Serial.read() == '\n') {
//        
//    }
    
  }

  

  

   



}




void sendData(int analog_in1,int analog_in2,int digital_in1,int digital_in2,int analog_out1,int analog_out2,int digital_out1,int digital_out2) {
  StaticJsonBuffer<300> JSONbuffer;

  JsonObject& root = JSONbuffer.createObject();
  root["analog_in1"] = analog_in1;
  root["analog_in2"] = analog_in2;
  root["digital_in1"] = digital_in1;
  root["digital_in2"] = digital_in2;
  root["analog_out1"] = analog_out1;
  root["analog_out2"] = analog_out2;
  root["digital_out1"] = digital_out1;
  root["digital_out2"] = digital_out2;
 

  //String a="value1=2&value2=1";
 

  char JSONmessageBuffer[300];
  root.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  //Serial.println(JSONmessageBuffer);
 

  HTTPClient http;    //Declare object of class HTTPClient
  http.begin("http://185.236.36.233:2020/ServerConnect/hardwareData.php");      //Specify request destination
  http.addHeader("Content-Type", "form-data");
  
  int httpCode = http.POST(JSONmessageBuffer);
  //int httpCode = http.POST(a);
 
  String payload = http.getString();
  //Serial.println(httpCode);   //Print HTTP return code
 // Serial.println(payload);    //Print request response payload
  JsonObject& response = JSONbuffer.parseObject(payload);
  char main[4];
  char analog1[4];
  char analog2[4];
  char digital1[4];
  char digital2[4];
  char out2[30];

  //data=response["analog1"];
  Serial.print("|G| ");
  strcpy(main, response["main"]);
  Serial.print(main);
  Serial.print(" ");
  strcpy(analog1, response["analog1"]);
  Serial.print(analog1);
  Serial.print(" ");
  strcpy(analog2, response["analog2"]);
  Serial.print(analog2);
  Serial.print(" ");
  strcpy(digital1, response["digital1"]);
  Serial.print(digital1);
  Serial.print(" ");
  strcpy(digital2, response["digital2"]);
  Serial.println(digital2);
  

 // String out="|G| ";
//  out+=main;
//  out+=" ";
//  out+=analog1;
//  out+=" ";
//  out+=analog2;
//  out+=" ";
//  out+=digital1;
//  out+=" ";
//  out+=digital2;


//sprintf(out2,"|G| %d %c %c %c %c \n",main,analog1,analog2,digital1,digital2);
  
  
  
  //Serial.println(out);
 // String w="|G| "+main;
 //out=out2;
  //Serial.println(out2); 
//  Serial.print("|G| ");
//  Serial.print(main);
//  Serial.print(" ");
//  Serial.print(analog1);
//  Serial.print(" ");
//  Serial.print(analog2);
//  Serial.print(" ");
//  Serial.print(digital1);
//  Serial.print(" ");
//  Serial.println(digital2);
  
  http.end();

}


