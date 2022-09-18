#include <Arduino.h> //include librarary header
#include "NeuralNetwork.h" //include model header
#include "WiFi.h"
// #include <esp_http_client.h>
// #include <base64.h>
// #include<iostream>
// #include<fstream>

// using namespace std;

//Select Camera model 
#define CAMERA_MODEL_ESP_EYE


// WiFi credentials.
const char* ssid = "TP-Link_29F3";
const char* password = "93374262";

// const char* WIFI_SSID = "Elena"; //"REPLACE_WITH_YOUR_SSID";
// const char* WIFI_PASS = "1234567890"; //"REPLACE_WITH_YOUR_PASSWORD";

// #include <esp_camera.h> //include esp camera
#include "esp_camera.h"

#include "camera_pins.h"
#include "img_converters.h"

#define FRAME_SIZE FRAMESIZE_96X96 ////FRAMESIZE_QVGA // FRAMESIZE_240X240 //
#define WIDTH 96//320 //224
#define HEIGHT 96//240 //224

// uint8_t img_array [HEIGHT][WIDTH] = { 0 };
// float img_array [HEIGHT][WIDTH] = { 0 };

uint8_t img_array[HEIGHT * WIDTH] = { 0 };   // grayscale goes from 0 to 255. fits in 8bits


NeuralNetwork *nn;

bool setup_camera(framesize_t);
void frame_to_array(camera_fb_t * frame);
void print_image_shape(camera_fb_t * frame);
bool capture_image();

// void startCameraServer();

void setup(){
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  nn = new NeuralNetwork();
  
  Serial.println("tf model loaded!"); 

  Serial.println("camera setup"); 
  Serial.println(setup_camera(FRAME_SIZE) ? "OK" : "ERR INIT");
  
  // Connect to Wifi.
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

}

void loop(){
  // delay(10000);
  if (!capture_image()) {
        Serial.println("Failed capture");
        delay(2000);

      return;
  }
  if (capture_image){
        Serial.println("CAMERA OK");
              // // Fill input buffer
      for (uint32_t i = 0; i < HEIGHT*WIDTH; i++){
        // Assuming your model input expects signed 8bit integers
        // Serial.printf("IMG_Array: ", i); 
        // Serial.println(img_array[i]/255);
        // nn->getInputBuffer()[i] = (float_t) (myArray[i]);
        //Serial.print("img_array:"); 
        //Serial.print((float)img_array[i]/255); 
        // Serial.println();
        // nn->getInputBuffer()[i] = (int8_t)(img_array[i]);
        nn->getInputBuffer()[i] = (float)(img_array[i])/255;//(int8_t) (img_array[i] - 128)/255;
      }

      float result = nn->predict(); 

      Serial.print("Object_detected:"); 

      String object_detected; 
      if (result>0.5){
        Serial.print("Apple"); 
        object_detected = "Apple"; 
      }
      else{
        Serial.print("Lemon"); 
        object_detected = "Lemon"; 
        result = 1 - result; 
      }
      Serial.println(); 
      Serial.print("Pred:"); 
      Serial.print(result);
      Serial.println(); 

  }

  // Wait for serial input to be made available and parse it as a float 
  // float number1 = random(100) / 100.0;
  // float number2 = random(100) / 100.0;


  //nn->getInputBuffer()[0] = //number1;//img_array;
  // nn->getInputBuffer()[1] = number2;
   
    // for (uint32_t i = 0; i < HEIGHT*WIDTH; i++){
      // Assuming your model input expects signed 8bit integers
      // uint32_t input_ix = 0; 
      // Serial.print("Image_array"); 
      // Serial.print(img_array[i]);
      // nn->getInput()[i] = (int8_t) (img_array[i] - 128);

      // }

      // //Opening the file
      // ifstream inputfile("Apple.txt");
      // if (!inputfile.is_open()){
      //   Serial.print("Error opening file"); 
      // } 
      // //Defining the loop for getting input from the file
      // int row = 96;
      // int col = 96;
      // int myArray[row*col];

      // for (int r = 0; r < row; r++) //Outer loop for WIDTHs
      // {
      //     for (int c = 0; c < col; c++) //inner loop for columns
      //     {
      //       inputfile >> myArray[r*c];  //Take input from file and put into myArray
      //     }
      // }

      // for (int r = 0; r < row; r++)
      // {
      //     for (int c = 0; c < col; c++)
      //     {
      //         cout << myArray[r*c] << "\t";
      //     }
      //     cout<<endl;
      //     // Serial.print("MyArray[1000]");
      //     // Serial.print(myArray[10*10]); 
      //     }
     


      // // tflite model has input shape [batch_size, height, width, channels]
      // // which in turn is [1, HEIGHT, WIDTH, 3] three channels because RGB
      // // but tflite micro expects flattened 1D array so you can just do this
      // for (uint32_t pix = 0; input_ix < HEIGHT*WIDTH; pix++){
      //   // for (int h=0; h < HEIGHT; h++){
      //   //  Serial.println(h);
      //   //  for (int w=0; w < WIDTH; w++){
      //     // Convert from RGB55 to RGB888 and int8 range
      //     uint16_t color = img_array[pix];
      //     // int16_t color = img_array[h*w];
      //     // int16_t r = ((color & 0xF800) >> 11)*255/0x1F - 128;
      //     // int16_t g = ((color & 0x07E0) >> 5)*255/0x3F - 128;
      //     // int16_t b = ((color & 0x001F) >> 0)*255/0x1F - 128;
      //     // nn->getInput()[input_ix] = (int8_t) r; 
      //     // nn->getInput()[input_ix+1] = (int8_t) g; 
      //     // nn->getInput()[input_ix+2] = (int8_t) b; 
         
      //     // Serial.printf("Color R:"); 
      //     // Serial.println(r); 
      //     // Serial.printf("Color G:"); 
      //     // Serial.println(g); 
      //     // Serial.printf("Color B:"); 
      //     // Serial.println(b);         
      //     // input_ix += 3;
     
      //   }

    // Serial.print(input_ix);c

    
  // const char *expected = number2 > number1 ? "True" : "False";

  // const char *predicted = result > 0.5 ? "True" : "False";

  // Serial.printf("%.2f %.2f - result %.2f - Expected %s, Predicted %s\n", number1, number2, result, expected, predicted);

  // delay(3000);
}

bool setup_camera(framesize_t frameSize) {
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
    config.pixel_format =  PIXFORMAT_GRAYSCALE; 
    config.frame_size = frameSize;
    config.jpeg_quality = 12;
    config.fb_count = 1;

    bool ok = esp_camera_init(&config) == ESP_OK;

    //Changing ESP32-CAM Camera Settings Arduino Sketch 
    sensor_t *sensor = esp_camera_sensor_get();
    
    sensor->set_brightness(sensor,2);  //up the brightness just a bit  // -2 to 2
    sensor->set_contrast(sensor, -2);       // -2 to 2
    // sensor->set_saturation(sensor, 1); // lower the saturation //-2 to 2 
    sensor->set_vflip(sensor, 1);          // 0 = disable , 1 = enable 
    // sensor->set_special_effect(sensor, 2); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
    sensor->set_awb_gain(sensor, 1);       // 0 = disable , 1 = enable
    sensor->set_wb_mode(sensor, 4);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
    
    sensor->set_framesize(sensor, frameSize);

    return ok;
}


 bool capture_image() {

    camera_fb_t * frame = NULL;
    frame = esp_camera_fb_get();
    const char *data = (const char *)frame->buf;
    print_image_shape(frame);
    frame_to_array(frame);
    esp_camera_fb_return(frame);

    if (!frame)
        return false;

    return true;
 }


 void print_image_shape(camera_fb_t * frame){

     // print shape of image and total length (=heigth*width)
    //  Serial.print("Width: ");
    //  Serial.print(frame->width);
    //  Serial.print("\tHeigth: ");
    //  Serial.print(frame->height);
    //  Serial.print("\tLength: ");
    //  Serial.println(frame->len);

 }

 void frame_to_array(camera_fb_t * frame){

    int len = frame->len;
    char imgBuffer[frame->len];
    int counter = 0;
    
    //Reference: https://microcontrollerslab.com/esp32-cam-image-classification-machine-learning/#ESP32-CAM_Image_Classification_Arduino_Sketch
    // camera_fb_t * fb = NULL;
    // fb = esp_camera_fb_get();
    // camera_fb_t * frame = NULL;
    // fb = esp_camera_fb_get();
    

    // size_t size = frame->len;
    // Serial.print("data: "); 
    // Serial.printf(data);
    
    // String buffer = base64::encode((uint8_t *) frame->buf, frame->len);
    // String payload = "{\"inputs\": [{ \"data\": {\"image\": {\"base64\": \"" + buffer + "\"}}}]}";
    // buffer = "";
    // Serial.print("Payload:");
    // Serial.println(payload);

    // esp_camera_fb_return(fb);
    
     //Reference//////

    //  uint16_t img_array [HEIGHT][WIDTH] = { 0 };
    // uint8_t img_array [HEIGHT * WIDTH] = {0}; 

     int h_counter = 0;
     int w_counter = 0;

     // write values from buffer into 2D Array
     for (int h=0; h < HEIGHT; h++){
        //  Serial.println(h);
         for (int w=0; w < WIDTH; w++){
            //  Serial.println(w);
            int position = h*(len/HEIGHT)+w;

            // Serial.print(position);
            // Serial.print(",");
            img_array[h*w] = {frame->buf[position]};
            // Serial.print(img_array[h*w]);
            // Serial.println();
            //  delay(2);
         }
        // delay(100); 

     }
    //  Serial.println("Current frame:");
    // Serial.println("=====================");
    // Serial.println("data: "); 
    // Serial.println(data); 
     
 }

 