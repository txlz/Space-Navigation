#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
Adafruit_MPU6050 mpu;
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>  
#define OLED_RESET 0     
Adafruit_SSD1306 display(128, 64, &Wire, -1);
#define SDA_1 13
#define SCL_1 14
#define SDA_2 4
#define SCL_2 5
#define address 0x68
int last_dx, last_dy, dx, dy, angle;
const int centreX = 32 + 30;
const int centreY = 30;
const int radius = 20;

void setup() {
  Serial.begin(115200);
  Wire.begin(4,5);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display_item(0, 0, "Please face towards the base for 5 seconds", WHITE, 1);
  display_item(0, 15, "Click reset to recalibrate", WHITE, 1);
  display.display();
  delay(6000);  // Delay for 6 seconds
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();

  Wire.beginTransmission(address);  
  Wire.write(0x1C);                 
  Wire.write(0x10);                
  Wire.endTransmission();
  delay(300);  
  last_dx = centreX;
  last_dy = centreY;


  while (!Serial)
    delay(10); 

  Serial.println("Adafruit MPU6050 test!");

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
}

void loop() {

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);  

  Serial.println("");
  delay(500);
  int16_t x, y, z;  //triple axis data

  Wire.beginTransmission(address);
  Wire.write(0x3B);  
  Wire.endTransmission();

    Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read() << 8 | Wire.read();
    y = Wire.read() << 8 | Wire.read();
    z = Wire.read() << 8 | Wire.read();
  }
  Draw_Compass_Rose(); 
  float heading = atan2(z, y); 
  float readjust = 1.529;
  heading = heading + readjust;
  if(heading < 0)    heading += 2*PI;  
  if(heading > 2*PI) heading -= 2*PI;  
  angle = int(heading * 180/M_PI); 
  dx = (0.7*radius * cos((angle-90)*3.14/180)) + centreX; 
  dy = (0.7*radius * sin((angle-90)*3.14/180)) + centreY;  
  arrow(last_dx,last_dy, centreX, centreY, 2,2,BLACK);      
  arrow(dx,dy, centreX, centreY, 2, 2,WHITE);               
  last_dx = dx; 
  last_dy = dy;
  display.setCursor(100,45);
  display.fillRect(100,45,25,48,BLACK);
  display.print(angle);
  display.print(char(247)); 
  display.display();
  delay(10);
}

void display_item(int x, int y, String token, int txt_colour, int txt_size) {
  display.setCursor(x, y);
  display.setTextColor(txt_colour);
  display.setTextSize(txt_size);
  display.print(token);
  display.setTextSize(1);
}

void arrow(int x2, int y2, int x1, int y1, int alength, int awidth, int colour) {
  float distance;
  int dx, dy, x2o,y2o,x3,y3,x4,y4,k;
  distance = sqrt(pow((x1 - x2),2) + pow((y1 - y2), 2));
  dx = x2 + (x1 - x2) * alength / distance;
  dy = y2 + (y1 - y2) * alength / distance;
  k = awidth / alength;
  x2o = x2 - dx;
  y2o = dy - y2;
  x3 = y2o * k + dx;
  y3 = x2o * k + dy;
  x4 = dx - y2o * k;
  y4 = dy - x2o * k;
  display.drawLine(x1, y1, x2, y2, colour);
  display.drawLine(x1, y1, dx, dy, colour);
  display.drawLine(x3, y3, x4, y4, colour);
  display.drawLine(x3, y3, x2, y2, colour);
  display.drawLine(x2, y2, x4, y4, colour);
} 

void Draw_Compass_Rose() {
  int dxo, dyo, dxi, dyi;
  display.drawCircle(centreX,centreY,radius,WHITE);  // Draw compass circle

  display_item((centreX-2),(centreY-24),".",WHITE,1);
  display_item((centreX-2),(centreY+17),".",WHITE,1);
  display_item((centreX+19),(centreY-3),".",WHITE,1);
  display_item((centreX-23),(centreY-3),".",WHITE,1);
}
