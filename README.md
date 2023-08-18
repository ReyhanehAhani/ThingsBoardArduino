# IoT Sensor Data Sender

This repository contains an Arduino sketch that reads data from various sensors (temperature, light, and moisture) and sends the collected data to a remote server using an ESP8266 module. The collected data is sent in JSON format via a TCP connection over WiFi. This project can be used as a starting point for building IoT applications that involve sensor data collection and remote communication.

## Prerequisites

- Arduino IDE installed.
- ESP8266 module connected to the Arduino board.

## Setup

1. Clone or download this repository to your local machine.

2. Open the Arduino IDE and ensure that you have the ESP8266 board package installed (follow [these instructions](https://arduino-esp8266.readthedocs.io/en/latest/)).

3. Connect the ESP8266 module to your Arduino board as follows:

   - **ESP8266 RX** to **Arduino TX (Pin 2)**
   - **ESP8266 TX** to **Arduino RX (Pin 3)**

   Make sure the power and ground pins are also properly connected.

4. Open the `IoT_Sensor_Data_Sender.ino` sketch from the repository in the Arduino IDE.

5. Modify the following parameters in the sketch to match your setup:

   - `token`: Replace with your server authentication token.
   - `server`: Replace with the IP address or hostname of the remote server.
   - `port`: Replace with the port number the server is listening on.

6. Upload the sketch to your Arduino board.

## Usage

1. Open the Arduino Serial Monitor at a baud rate of 9600 to view the debug output.

2. When the sketch starts, it will attempt to connect to a WiFi network using the provided credentials. If the connection is successful, sensor initialization will occur.

3. The sketch will continuously read sensor data from the LM335 temperature sensor, LDR light sensor, and moisture sensor.

4. Sensor data is collected and formatted into a JSON object.

5. The formatted JSON data is sent to the remote server using a TCP connection.

6. Data transmission success or failure is indicated in the Serial Monitor.

7. The data transmission is scheduled to occur every 15 minutes based on an interrupt timer.

## Notes

- Make sure to handle security and authentication properly if deploying this code in a production environment.
- Remember to adjust pins and connections as needed based on your specific hardware setup.
- This code is provided as a basic example. You can expand and customize it to suit your project's requirements.
