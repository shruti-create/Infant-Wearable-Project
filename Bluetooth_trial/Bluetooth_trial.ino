#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "esp_camera.h"

// Define camera pins for the T-Camera S3
#define CAMERA_MODEL_T_CAMERA_S3

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

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID        "12345678-1234-5678-1234-56789abcdef0"
#define CHARACTERISTIC_UUID "12345678-1234-5678-1234-56789abcdef1"

BLECharacteristic *cameraCharacteristic;

const size_t CHUNK_SIZE = 512;  // BLE MTU size for each chunk

// Callback for handling BLE characteristic read events
class CameraDataCallbacks : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {
    Serial.println("Client requested camera data...");

    // Capture a photo
    //camera_fb_t *fb = esp_camera_fb_get();
    // if (!fb) {
    //   Serial.println("Camera capture failed");
    //   return;
    // }

    camera_fb_t * fb = NULL;
    Serial.printf("Free heap: %d bytes\n", esp_get_free_heap_size());

    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb); // dispose the buffered image
    fb = NULL; // reset to capture errors
    fb = esp_camera_fb_get(); // get fresh image
    if(!fb) {
      Serial.println("Camera capture failed");
      delay(1000);
      ESP.restart();
    } 

  Serial.print("Here!");
    Serial.printf("Captured image size: %d bytes\n", fb->len);

    // Send the image in chunks
    size_t offset = 0;
    while (offset < fb->len) {
      size_t chunkSize = (fb->len - offset) > CHUNK_SIZE ? CHUNK_SIZE : (fb->len - offset);
      pCharacteristic->setValue(fb->buf + offset, chunkSize);
      pCharacteristic->notify();  // Send the chunk
      offset += chunkSize;
      delay(10);  // Small delay to ensure proper transmission
    }

    Serial.println("Camera data sent!");

    // Return frame buffer to the driver
    esp_camera_fb_return(fb);
  }
};

void setup() {
  Serial.begin(115200);

  // Initialize the camera
  camera_config_t config;
  
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
   
  config.xclk_freq_hz = 20000000;  // Typical frequency for OV2640
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Try different frame sizes
  config.frame_size = FRAMESIZE_VGA;  // VGA resolution
  config.jpeg_quality = 10;  // 0-63 lower number means higher quality
  config.fb_count = 2;  // Use two frame buffers
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    while (1) {
      delay(1000);
    }
  }

  Serial.println("Camera initialized.");

  // Initialize BLE
  BLEDevice::init("ESP32-Camera");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a readable characteristic for sending camera data
  cameraCharacteristic = pService->createCharacteristic(
                           CHARACTERISTIC_UUID,
                           BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY| BLECharacteristic::PROPERTY_WRITE
                         );

  // Set the callback function for when the characteristic is read
  cameraCharacteristic->setCallbacks(new CameraDataCallbacks());

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

  Serial.println("BLE Camera Server Initialized. Waiting for client...");
}

void loop() {
  // Nothing to do in the loop
}
