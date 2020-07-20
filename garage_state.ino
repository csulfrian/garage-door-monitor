/*
  SparkFun ESP8266 Thing based garage door position sensor,
  using a Blynk app to determine the state of two switches
  on a garage door track. Tell remotely whether your car hole
  door is open or closed or somewhere inbetween! And if it's
  somehow other than it should be, there's a provision in the
  code to actuate a relay to tell the door opener to open/close.

  Blynk uses virtual pins to interact with the app. They can be 
  set to anything, just make sure that matches when you set up the app.

  I used the following app "buttons":
  Green LED to represent "door closed" state
  Red LED to represent "door open" state
  Button to actuate open/close relay
  
  Wiring Diagram

  Arduino --------------- Inputs/Outputs
  Pin 0 ----------------- The "garage open" limit switch (pulls low)
  Pin 4 ----------------- The "garage closed" limit switch (pulls low)
  Pin 12 ---------------- The pin to actuate the relay to close or open the door
  

  Written by: Chris Sulfrian July 2020
  http://github.com/csulfrian
*/

// include the two Blynk libraries used
#include <Blynk.h>
#include <BlynkSimpleEsp8266.h>

// You should get an Auth Token in the Blynk App when you create a new project
char auth[] = "your_project_specific_blynk_token";

// Your WiFi credentials
// Set password to "" for open networks.
char ssid[] = "your_wifi_ssid";
char pass[] = "your_wifi_password";

// define our GPIO and virtual pins used
#define ESP8266_LED 5
#define up_switch 0
#define down_switch 4
#define relay_pin 12
#define up_virtual V1
#define down_virtual V2
#define relay_virtual V0

// set the initial state of the two position switches
bool up_state = false;
bool down_state = false;

// we'll be using two "LEDs" in Blynk, this is how to instantiate them
WidgetLED led1(up_virtual);
WidgetLED led2(down_virtual);

// instantiate the Blynk timer so as not to spam their server
BlynkTimer timer;

// initialize all the communications we'll be using: serial and WiFi
void init_coms()
{
  // serial
  Serial.begin(115200);

  // initialize our WiFi from the Blynk library
  Blynk.begin(auth, ssid, pass);
  // You can also specify server: (Blynk sample code)
  // Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  // Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
}

// set up our hardware and pin modes
void init_hardware()
{
  pinMode(ESP8266_LED, OUTPUT);
}

// check our "up" position switch and push the status to the Blynk app
void up()
{
  boolean isPressed = (digitalRead(up_switch) == LOW);

  if (isPressed != up_state)
  {
    if (isPressed)
    {
      led1.on();
    }
    else
    {
      led1.off();
    }
    up_state = isPressed;
  }
}

// check our "down" position switch and push the status to the Blynk app
void down()
{
  boolean isPressed = (digitalRead(down_switch) == LOW);

  if (isPressed != down_state)
  {
    if (isPressed)
    {
      led2.on();
    }
    else
    {
      led2.off();
    }
    down_state = isPressed;
  }
}

// call the switch check functions created previously
void switches_state()
{
  up();
  down();
}

// writing a state to the pin we'll actuate the relay with
BLYNK_WRITE(relay_virtual)
{
  int button = param.asInt(); // read button
  if (button == 1) {
    digitalWrite(ESP8266_LED, HIGH); // write LED high on board
    digitalWrite(relay_pin, HIGH); // actuate relay
  }
  else {
    digitalWrite(ESP8266_LED, LOW);
    digitalWrite(relay_pin, LOW);
  }
}

// the Arduino setup function; initialize coms and hardware
// and instantiate our timer with the period at which it will run
void setup()
{
  init_coms();
  init_hardware();

  timer.setInterval(1000L, switches_state); // Blynk timer will run every sec
}

/* Party time! All that work has led up to us running the Blynk and timer
  functions. Keep this area as clean as possible so as not to interfere with
  the Blynk server communication. See their documentation for all the details.*/
void loop()
{
  Blynk.run();
  timer.run();
}
