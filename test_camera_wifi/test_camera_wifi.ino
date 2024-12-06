#include "esp_camera.h"
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "UCSD-GUEST";
const char* password = "";

// Select the camera model
#define CAMERA_MODEL_T_CAMERA_S3

// Pin definition for T_CAMERA_S3
#if defined(CAMERA_MODEL_T_CAMERA_S3)
  #define PWDN_GPIO_NUM  -1
  #define RESET_GPIO_NUM 39
  #define XCLK_GPIO_NUM  38
  #define SIOD_GPIO_NUM  5
  #define SIOC_GPIO_NUM  4

  #define Y2_GPIO_NUM 14
  #define Y3_GPIO_NUM 47
  #define Y4_GPIO_NUM 48
  #define Y5_GPIO_NUM 21
  #define Y6_GPIO_NUM 13
  #define Y7_GPIO_NUM 11
  #define Y8_GPIO_NUM 10
  #define Y9_GPIO_NUM 9

  #define VSYNC_GPIO_NUM 8
  #define HREF_GPIO_NUM  18
  #define PCLK_GPIO_NUM  12
#endif



  
void setup() {
  Serial.begin(115200);
Serial.println("defined");
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());
   if (WiFi.getMode() == WIFI_STA) {
        int band = WiFi.RSSI();
        Serial.println("Connected to a Wi-Fi network");
        Serial.print("Wi-Fi band: ");
        Serial.println(band);
    }

// Camera configuration
camera_config_t config;
  // Camera configuration
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;  // XCLK frequency in Hz
  config.frame_size = FRAMESIZE_VGA; // Set the frame size to VGA (640x480)
  config.pixel_format = PIXFORMAT_JPEG; // Set the pixel format to JPEG


  Serial.printf("Free heap before camera init: %d bytes\n", esp_get_free_heap_size());

  // Initialize the camera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed");
    return;
  }
  else{Serial.println("Camera_successful");}

  // Print the IP address of the ESP32 camera once connected
  Serial.print("Camera Stream Ready! Use http://");
  Serial.print(WiFi.localIP());
  Serial.println(":80");
}

void loop() {
  
  // Your code to handle streaming, capturing, etc.
}