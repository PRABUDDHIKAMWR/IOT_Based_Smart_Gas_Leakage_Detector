# IOT_Based_Smart_Gas_Leakage_Detector

## Description

This project is a comprehensive solution designed to measure the concentration of LPG gas particles in the air and provide real-time updates to the user. The system is equipped with a sensor that continuously monitors the air for LPG gas particles and displays the detected level in parts per million (ppm) on a built-in screen.

One of the key features of this system is its ability to alert the user when the concentration of LPG gas exceeds a predefined threshold. This is achieved through three different types of alarm indicators: an LED indicator, an alarm buzzer, and a remote notification sent to the user's smartphone via an app. These multiple modes of alert ensure that the user is promptly and effectively notified about any potential gas leaks, thereby enhancing safety.

In addition to local monitoring and alerts, this system also supports remote monitoring. This allows users to keep track of LPG gas levels in their environment even when they are not physically present, providing an extra layer of convenience and safety.

This Gas Leakage Detector is an essential tool for anyone looking to enhance the safety of their environment by effectively monitoring and managing the risk of gas leaks.
 

## Table of Contents

- Installation
- Usage
- Code Examples
- Contribution Guidelines
- License
- Contact Information

## Installation

To ensure optimal performance and safety, please follow these installation guidelines for the Gas Leakage Detector:

1. **Location**: Position the device near the gas source. Ideally, it should be placed within a distance of 50 cm from the gas source and approximately at the same height as the gas cooker.

2. **Power Source**: Ensure there is a power switch within a 1.5m radius of the device. This will allow for easy access to power the device by plugging it into a wall socket.

Please note that proper installation is crucial for the effective operation of the Gas Leakage Detector. Always refer to these guidelines when setting up your device.

## Usage
![WhatsApp Image 2023-07-19 at 17 46 58](https://github.com/PRABUDDHIKAMWR/IOT_Based_Smart_Gas_Leakage_Detector/assets/117188107/2ea9e62b-b38f-46c4-b355-64414cae6772)

![WhatsApp Image 2023-07-19 at 17 46 58](https://github.com/PRABUDDHIKAMWR/IOT_Based_Smart_Gas_Leakage_Detector/assets/117188107/ad9d3425-30bc-43cf-896f-6339a2fc5b69)

Follow these steps to operate the Gas Leakage Detector:

1. **Power Connection**: After mounting the device, connect it to a power source (socket) and turn on the power ON/OFF switch on the device.

2. **WiFi Connection**: Using a device such as a smartphone or laptop, connect the Gas Leakage Detector to WiFi using the access point. If you cannot connect to WiFi within 1 minute, the device will automatically operate without a WiFi connection.

3. **Remote Monitoring and Alerting**: For remote monitoring and alerting, it is necessary to connect the device to WiFi. If you cannot connect when prompted, press the second button on the device to initiate WiFi connection.

4. **Gas Source Usage**: When using the gas source, if the device indicates a gas leakage, press the first button on the device to avoid continuous alerting. This will activate 'Cooking Mode'. Once you turn off the gas source and the LPG concentration in the air decreases, the device will automatically return to normal operation.

5. **Safety Measures**: If you experience a gas leakage, take appropriate safety measures to prevent any accidents.

Remember, proper usage is crucial for ensuring effective operation and safety with the Gas Leakage Detector.


## Code Examples

Here are some key functions in our Gas Leakage Detector code:

1. **measure_ppm()**: This function measures the concentration of LPG gas in the air.

```c++
void measure_ppm() {
  float LPGAvg = 0;
  for (int i = 0; i < 30; i++) {
    delay(100);
    LPGAvg += analogRead(mqpin);
  }
  LPGppm = LPGAvg;
}


void printState(String state) {
  Serial.print("LPG ppm: ");
  Serial.print(LPGppm);
  Serial.println(" ppm");
  display.clearDisplay();
  print_line(state, 26, 45, 2);
  print_line("LPG concentration :", 0, 0, 1);
  display.setCursor(26, 20);
  print_line(String(LPGppm) + " ppm", 26, 20, 1);
  delay(3000);
}

void connectWiFi() {
  display.clearDisplay();
  print_line("Connect to WiFi...", 1, 1, 0);
  WiFiManager wm;
  wm.resetSettings();
  wm.setConfigPortalTimeout(120); // Set connection timeout to 2 minutes
  bool res = wm.autoConnect("AutoConnectAP", "password"); // Password-protected AP

...
}
```

## Contribution Guidelines

I welcome and appreciate all contributions. Here's how you can help:

- **Reporting Issues**: If you find a bug or a mistake in the code, please report it by opening a new issue in the GitHub repository.

- **Suggesting Enhancements**: If you have an idea for a new feature or an improvement in the existing functionality, feel free to open an issue describing your idea.


## License

This project is licensed under the GNU General Public License (GPL). This means that you are free to use, modify, and distribute the code, but any changes or improvements must also be released under the GPL.

For the full license text, see the [LICENSE] file in this repository.


## Contact Information

For any questions or concerns about the project, feel free to reach out:

- Create an issue in [GitHub Issues].

Please understand that this is an open source project and while I strive to be responsive, there may be some delays in getting back to you.

