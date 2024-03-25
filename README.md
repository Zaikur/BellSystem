# Bell System Project

## Navigation
- [Overview](#overview)
- [Tech Used](#tech-used)
- [Key Components](#key-components)
- [Optimizations](#optimizations)
- [Setup this project](#setup-this-project)
- [Lessons Learned](#lessons-learned)
- [Known Bugs/Needs Improvement](#known-bugs-needs-improvement)
- [Final Notes](#final-notes)

## Overview
This project was to develop a bell scheduling system using an ESP8266 microcontroller, designed to automate bell ringing schedules for institutions such as schools or churches. With WiFi connectivity, the system is easily adjustable via a web interface, making bell schedules manageable in real-time.

I took on this project because I wanted to gain a better understanding of the fundamentals of writing code where limitations such as processing power and ram are strictly limited.
This system works by turning the ESP8266 board into a tiny server that can be accessed on your local network via IP address or a local URL. The URL is customizable so you're able to have multiple systems on one network without overlap.

I went back and forth on how in depth the schedule should be. I finally settled on a 1-week customizable schedule that repeats indefinitely. I feel this is a good balance between the inflexibility of a 1-day schedule, and the required overhead of a monthly schedule. Each day of the week is customizable with its own set schedule.

## Tech Used
**Microcontroller:** 
- ESP8266 ESP-12E 
- 80 MHz processor
- 80 KBytes RAM
- 4M Flash memory
- 802.11 b/g/n WiFi Module

**Programming Languages:** C++ (for ESP8266 firmware), JavaScript (for web interface)

**Web Technologies:** HTML, CSS (Bootstrap for styling), jQuery

**Libraries:** 
- ArduinoJson 
- ESP8266WebServer
- ESP8266WiFi
- WiFiManager
- LittleFS
- base64
- EzTime

**Development Tools:**
 - Visual Studio Code
 - PlatformIO

**Features:**
Built around the ESP8266, this project features WiFi setup and reconfiguration, a web server for schedule management, NTP time synchronization for accuracy, and security measures for safe operation.


## Key Components
 - **WiFi Manager:** Handles initial WiFi setup without needing to reprogram. If WiFi fails to connect for 2 minutes, it automatically reverts to an AP.
 - **Web Server:** Manages the web interface for scheduling and settings adjustments. Includes setting a custom URL so multiple devices can be on the same network.
 - **Time Management:** Syncs with NTP servers to maintain accurate timing. Posix string is cached so reboot time is faster, and to reduce the chance of time inconsistancies due to latency.
 - **Schedule Management:** Enables custom bell schedule setup via a web interface. A 1-week custom ring schedule repeats indefinitely.
 - **Security Features:** Includes password protection and token-based authentication. Both are hashed using an SHA-256 algorithm.
 - **Development Process:**
The project prioritized a user-friendly interface and reliable scheduling. Efforts were made to ensure accurate time synchronization, efficient memory usage, and responsive web server operations.

## Optimizations
**Significant optimizations that were introduced during development:**
*(i.e. Things I messed up and had to fix later)*
1. JSON Storage: Efficiently stores and retrieves schedules, reducing EEPROM wear. (Yeah it's emulated, but it's still faster)
2. File System Usage: Leveraging LittleFS for webpage storage significantly reduced server processing load by streaming files directly to the client.
3. Dynamic Time Zone Handling: Using ezTime with Posix format for self-contained daylight saving adjustments, eliminating reliance on external timezone servers.
4. EEPROM Management: Organized EEPROM use facilitates future modifications.
5. Security: Removed all hardcoded credentials. (Except the out of the box password, which you are strongly encouraged to change via system messages)


## Materials For This Project
1. ESP8266 NodeMCU ESP-12E board (I used the one from HiLetgo)
2. ESP8266 Breakout Board (To make wiring easier)
3. 5 volt DC Power Supply (You could just use a USB cord to power it, but your relay will need power also)
4. 24 volt DC Power Supply (To power the bell, change this to what your bell requires)
5. 5V One Channel  Relay (Make sure your board can trigger it with 3 volts)
6. 5Amp - 10Amp Circuit Breaker (Just in case)

**Here's my extremely rough diagram (Just pretend the light is a bell)**

![Wiring Diagram.](/images/wiring-diagram.jpg "Wiring Diagram")

**The assembled prototype**
![Assembled Bell System](/images/assembled.jpg "Assembled Bell System")


## Setup This Project
### Setting Up PlatformIO in Visual Studio Code
PlatformIO is a powerful platform that simplifies IoT development, supporting a wide range of boards and frameworks. Here's how to set it up in Visual Studio Code (VSCode) to work on projects like the ESP8266 Bell System.

**Installation:**

1.	Install VSCode: If you haven't already, download and install Visual Studio Code from the official website.

2.	Install PlatformIO Extension: [How to install PlatformIO for VSCode](https://platformio.org/install/ide?install=vscode)

3.	Restart VSCode: Once the installation is complete, restart VSCode to ensure the PlatformIO extension is properly initialized.

**Creating a New Project:**

1.	Open PlatformIO Home: Click on the PlatformIO icon on the sidebar or open it from the View > Open View… > PlatformIO.
2.	Create Project: In PlatformIO Home, click on "New Project". Enter a name for your project, select the board you are using (e.g., ESP8266 ESP-12E), and choose the framework (Arduino).
3.	Project Configuration: PlatformIO will create a new project with a default configuration. You can edit platformio.ini file to customize the environment.
4.	Writing Code: Your project is now set up! Start coding by editing the src/main.cpp file.

**Using this project:**	

1. Download this repository in your environment
2. Open the PlatformIO tab, and select import project
3. Navigate to the source folder of your downloaded repository
	

**Building and Uploading:**

1.	Building: 
    - Click on the checkmark icon in the bottom toolbar to compile your code.
2.	Uploading: 
    - Connect your ESP8266 to your computer. 
    - Click on the right arrow icon in the bottom toolbar to upload your code to the board.
 

**Building the ESP8266 Bell System**
This project was developed using various tools and libraries to ensure functionality and ease of use. You will need to make sure these libraries are installed in your environment using PlatformIO's library manager.
Tools and Libraries:
- 	ArduinoJson: For handling JSON data for schedules and settings.
- 	ESP8266WebServer: To serve the web interface allowing users to interact with the bell system.
- 	ezTime: For accurate timekeeping and handling different time zones.
- 	LittleFS: For storing web interface files and efficiently serving them to clients.
- 	WiFiManager: To initially configure and manage WiFi settings without hardcoding credentials.

## Lessons Learned
This project was a comprehensive exploration into ESP8266 capabilities, emphasizing accurate timekeeping, memory management, security, and user experience.

Token and Password Hashing: Ensures security and uniqueness for user authentication.

System Status Messages: Provides immediate feedback on system operations and errors.

Serial.println() is the unsung hero of board development and debugging

## Known Bugs/Needs Improvement
**Timezone Selection Page:** A user-friendly interface for selecting timezones via Posix strings could enhance customizability.

**Password and Token Security:** Continuous improvements in hashing and token generation methodologies are necessary for maintaining robust security measures. I would like to implement some sort of request limiting logic to prevent attacks at some point.

**Loading a week long schedule into memory:**
Need to change the logic to load a day schedule at a time, or just load the next ring time after the current one has been triggered.


## Final Notes

This project has been both frustrating at times, and very rewarding at others. If you get anything out of this project, see some easy improvements that could be made, or find other uses for it pleast let me know!
