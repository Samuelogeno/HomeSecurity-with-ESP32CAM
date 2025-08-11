//FOR BLYNK BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
//FOR BLYNK
#define BLYNK_TEMPLATE_ID "TMPL2QgwTA_8E"
#define BLYNK_TEMPLATE_NAME "HomeSecurity"
#define BLYNK_AUTH_TOKEN "8USMOmB981VtAshS6SoA3C4LSdQpLerw"
///////////////////////////////////////////////////////////


#include "esp_camera.h"
#include <WiFi.h>

#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

//
// WARNING!!! Make sure that you have either selected ESP32 Wrover Module,
//            or another board which has PSRAM enabled
//
///////////////////////////////////////////////////////////

// Select camera model
//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER
#define BLYNK_PRINT Serial

#include "camera_pins.h"


const char* ssid2 = "OGENO'S";
const char* password2 = "JacktonOjwang1.";



//########################################################################################sam
//##############################################volatage pin
#define POTI_PIN 12
int voltage ;
int wifiStrength ;
  //#####

//##########################################################detection, faceId
int getId();
int faceId ;
int detection ;
int detectionStatus();
void startCameraServer();
// Set your Static IP address
IPAddress local_IP(192, 168, 0, 102);// tenda wire
//IPAddress local_IP(192, 168, 194, 111);// phone
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

//####################################################################################################

// Comment this out to disable prints and save space
BlynkTimer timer;

///////////////////////////////////
//FOR BLYNK


char auth[] = "8USMOmB981VtAshS6SoA3C4LSdQpLerw";

//Change the virtual pins according the blynk server
#define vpin0   V0 //detection
#define vpin1   V1 //take picture command
#define vpin2   V2 //user name
#define vpin3   V3 //link to website

#define vpin4 V4 //voltage pin virtual
#define vpin5 V5 //wifi signal pin
#define vpin6 V6 //mac address
#define vpin7 V7 //hard reset
#define vpin8 V8 //IP Address
//------------------------------------------------------------------------------
// This function is called every time the device is connected to the Blynk.Cloud
// Request the latest state from the server
BLYNK_CONNECTED() {
  Blynk.syncVirtual(vpin0);
  Blynk.syncVirtual(vpin1);
  Blynk.syncVirtual(vpin2);
  Blynk.syncVirtual(vpin3);
  Blynk.syncVirtual(vpin4);
  Blynk.syncVirtual(vpin5);
  Blynk.syncVirtual(vpin6);
  Blynk.syncVirtual(vpin7);
  Blynk.syncVirtual(vpin8);
  //Blynk.syncVirtual(button2_vpin);
  
}
// This function is called every time the Virtual Pin state change
//#############write to blynk ################

BLYNK_WRITE(vpin1) {
    int state = param.asInt();
    if(state==1){
      takePhoto();
    }
    
    //detection = 1;
  //Blynk.notify("sam");
  
  // int state;
  // state = param.asInt();
  //Serial.println(relay3_state);  
}
BLYNK_WRITE(vpin7) {
    int reset = param.asInt();
    if(reset>1000){
      Serial.println("Resetting............");
      Blynk.virtualWrite(vpin0, "Hardware reset!");
      Blynk.virtualWrite(vpin7, 0);
      ESP.restart();
      


    }
    //detection = 1;
  //Blynk.notify("sam");
  
  // int state;
  // state = param.asInt();
  //Serial.println(relay3_state);  
}
void takePhoto(){
  Serial.println("Taking picture and sending to web");
  ///generate a link
  //http;//ip//capture
  Blynk.virtualWrite(vpin2, "Http://192.168.0.103/capture");
}

void listen_detection(){
    if(detection==1){
        Blynk.virtualWrite(vpin0, "Someone around, huh!");
        //replace with camera detected
        //detected = 0;
        //name of detected person depends on mathed id
        if(faceId==0){
          Blynk.virtualWrite(vpin3, "Welcome Sam");
        }else if(faceId==1){
          Blynk.virtualWrite(vpin3, "Welcome Roy");
        }
        else {
          Blynk.virtualWrite(vpin3, "Unknown Person");
        }
        
    }else{
      Blynk.virtualWrite(vpin0, "No Person!");
      Blynk.virtualWrite(vpin3, ".....");
    } 
    //Serial.println(("written to vpin0,1"));
    //Serial.println();
    //delay(10);
    
}
void sendStatus(){
  //Serial.println("Sending voltage and wifi status: ");
  Blynk.virtualWrite(vpin4,voltage);
  Blynk.virtualWrite(vpin5,wifiStrength);
  Blynk.virtualWrite(vpin6,WiFi.macAddress());
  Blynk.virtualWrite(vpin8,WiFi.localIP().toString());
  //delay(10);
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();
 
   
  

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

#if defined(CAMERA_MODEL_ESP_EYE)
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
#endif


  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  //initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1);//flip it back
    s->set_brightness(s, 1);//up the blightness just a bit
    s->set_saturation(s, -2);//lower the saturation
  }
  //drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

#if defined(CAMERA_MODEL_M5STACK_WIDE)
  s->set_vflip(s, 1);
  s->set_hmirror(s, 1);
#endif
       ///////////////////////////////////Blynk start
  
  //Blynk.config(auth);
  // if(!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
  // Serial.println("STA Failed to configure");
  // }
 
  WiFi.begin(ssid2, password2);
  

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("MAC address: ");
  Serial.println(WiFi.macAddress());

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  char ssid[] = "OGENO'S";
  char pass[] = "JacktonOjwang1.";
  Blynk.begin(auth, ssid, pass);
  //Blynk.begin(auth);
 


}
//###################################################
unsigned long previousMillis = 0;
unsigned long interval = 30000;

void loop() {
// put your main code here, to run repeatedly:
    Blynk.run();
    timer.run();
    listen_detection();
    sendStatus();
    //Serial.println("loop running");
  
  unsigned long currentMillis = millis();
// if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
  Serial.print(millis());
  Serial.println("Reconnecting to WiFi...");
  WiFi.disconnect();
  WiFi.reconnect();
  previousMillis = currentMillis;
  } else{
    faceId = getId();
    detection = detectionStatus();
    // Serial.print("Matched Id by sam: ");
    // Serial.print(faceId);
    // Serial.print(" Detection by sam: ");
    // Serial.println(detection);
    //delay(1000);
    //###############################################################Wifi signal strength
    //Serial.print(" Wifi Signal: ");
     wifiStrength = WiFi.RSSI();
    //Serial.print(wifiStrength);
    //#################### voltage strenth
    //Serial.print(" Voltage: ");
    voltage = analogRead(POTI_PIN);
    //Serial.println(voltage);
    //Serial.println();

  }
 
}
