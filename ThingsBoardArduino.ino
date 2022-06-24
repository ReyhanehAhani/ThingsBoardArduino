#include <SoftwareSerial.h>

#define ArduinoRX 3  // Wire this to Tx Pin of ESP
#define ArduinoTX 2  // Wire this to Rx Pin of ESP
#define ESPBaud 9600

#define LDRPin A1
#define MoistureSignal A2
#define LM335Pin A0

const char* token = "-------------------";
const char* server = "192.168.1.102";
const int port = 8080;

// We'll use a software serial interface to connect to ESP
SoftwareSerial ESP(ArduinoRX, ArduinoTX);

bool ConnectToWIFI() {
  ESP.print("AT+CWLAP\r\n");
  delay(2000);

  Serial.println("Available access points (WIFI connections): ");

  while (ESP.available()) {
    Serial.write(ESP.read());
  }

  Serial.println("SSID: ");
  while (!Serial.available())
    ;
  String ssid = Serial.readString();

  Serial.println("Password: ");
  while (!Serial.available())
    ;
  String password = Serial.readString();

  ESP.print("AT+CWJAP=");
  ESP.print(ssid);
  ESP.print(",");
  ESP.print(password);
  ESP.print("\r\n");

  delay(2000);
  if (ESP.find("OK")) {
    Serial.println("Ready.");
    return true;
  } else {
    Serial.println("Could not connect, try again");
    return false;
  }
}

inline void InitESP() {
  ESP.begin(ESPBaud);
  // Check for module existance
  ESP.print("AT\r\n");
  delay(1000);

  if (ESP.find("OK")) {
    Serial.println("Ready.");
  } else {
    Serial.println("ESP not found");
    return;
  }
  ESP.print("AT+RST\r\n");
  delay(3000);

  if (ESP.find("GOT IP")) {
    Serial.println("Connected to WIFI");
  } else {
    Serial.println("Could not connect to WIFI");

    while (!ConnectToWIFI())
      ;
  }
  ESP.print("AT+CWMODE=3\r\n");
  delay(500);

  if (ESP.find("OK")) {
    Serial.println("Mode set.");
  }

  ESP.print("AT+CIPMUX=1\r\n");
  delay(500);
  if (ESP.find("OK")) {
    Serial.println("Connections set.");
  }

  Serial.println("ESP initalization comepleted successfully");
}

void InitTimer() {
  cli();  // Clear interrupts before config
  TCCR2A = (0 << COM2A1) | (0 << COM2A0);
  TCCR2B = (1 << CS22) | (1 << CS21) | (1 << CS20);
  TCNT2 = 28;
  TIMSK2 = (1 << TOIE2);
  sei();  // Set interrupts
}

volatile unsigned long ovf_count = 0, min_ovf = 0;

ISR(TIMER2_OVF_vect) {
  TCNT2 = 28;
  ovf_count++;

  if (ovf_count == 3662) {
    min_ovf++;
    ovf_count = 0;
  }
}

unsigned long light = 0, moisture = 0, temperature = 0;

void readSensors() {
  temperature = ((analogRead(LM335Pin) / 1024.0) * 5000) / 10;
  light = 1023 - analogRead(LDRPin);
  moisture = 1023 - analogRead(MoistureSignal);
}

void ESPFlush() {
  while (ESP.available() > 0) {
    char t = ESP.read();
  }
}

bool sendSensorValues() {
  char data[50];
  char request[300];
  char command[50];

  ESPFlush();

  sprintf(command, "AT+CIPSTART=0,\"TCP\",\"%s\",%d\r\n", server, port);
  ESP.print(command);
  delay(500);

  if (ESP.find("ERROR")) {
    Serial.print("Could not connect to server");
    return false;
  }

  delay(100);

  sprintf(data, "{\"temperature\": %d, \"light\": %d, \"moisture\": %d}", temperature, light, moisture);
  sprintf(request, "POST /api/v1/%s/telemetry HTTP/1.1\r\nContent-Type:application/json\r\nContent-Length: %d\r\n\r\n%s", token, strlen(data), data);
  sprintf(command, "AT+CIPSEND=0,%d\r\n", strlen(request));
  ESP.print(command);

  delay(500);

  if (ESP.find(">")) {
    Serial.println("Ready to send data");
  } else {
    Serial.println("Could not send data");
    return false;
  }

  ESP.print(request);

  if (ESP.find("200")) {
    Serial.println("Sent data");
  } else {
    Serial.println("Could not send data");
    return false;
  }
  
  ESP.print("AT+CIPCLOSE \r\n");
}

void setup() {
  Serial.begin(9600);
  readSensors();
  InitESP();
  InitTimer();

  sendSensorValues();
}

void loop() {
  if (min_ovf == 15) {
    min_ovf = 0;

    readSensors();
    sendSensorValues();
  }
}
