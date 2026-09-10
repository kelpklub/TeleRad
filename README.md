# TeleRad-A radio telescope  
### ![Radio Astronomy](https://img.shields.io/badge/Radio-Astronomy-black) ![ESP32](https://img.shields.io/badge/ESP32-Compatible-red) ![RTL-SDR](https://img.shields.io/badge/RTL--SDR-Supported-blueviolet)  ![Status](https://img.shields.io/badge/status-In%20Development-orange)  
NOTE: This project is currently in its design phase. No physical prototype has been made yet.  
A radio telescope mounted on an two axis alt-azimuth motorized mount for observing radio phenomenon.  
# Overview 
The Radio telescope will be made up of two systems.  
- Esp32 powered dual axis mount  
 (An Esp32 will control two stepper motors which will motorize two axes of the mount. The mount will move in a snake scan pattern across the sky.)
- Radio reciever  
  (An parabolic satallite ~75cm in diameter dish will will feed signal into a Low Noise Amplifier which goes through a Software Defined Radio dongle into a computer where it will be processed and converten into a 2d image map)  
## Bill of Materials
Heres everything needed for this project. [Bill Of Materials](BOM.csv)  
## CAD  
The Basic Cad design Assembly Of the mount can be found at [CAD Radio Telescope](CAD/Assembly.step)
<img width="1280" height="1024" alt="TeleRad" src="https://github.com/user-attachments/assets/d340e0a2-323a-4b93-8e6f-24d2d4935322" />

The Mount utilizes Gears to increase torque.
<img width="1920" height="1080" alt="Screenshot (7)" src="https://github.com/user-attachments/assets/1b288a7f-7db5-4c8e-81af-5256dfdb4b58" />  
## Gear Ratio  
The gear Ratios for both axes are as follows :-  
For The Altitude Rotation - Input gear teeth = 16T ; Output gear teeth = 128T  
For The Azimuth  Rotation - Input gear teeth = 24T ; Output gear teeth = 72T  
## Basic Wiring  
The wiring is as follows:-  
<img width="1700" height="2200" alt="TeleRad-Schematic_diagram" src="https://github.com/user-attachments/assets/4eadf490-a899-4225-a823-460a13008b6d" />

**MICROSTEPPING** - The M2 pin of both driver boards are connected to 3.3v of esp32 to set microstepping to 1/16 microstepping following the documentation of the drv8825 board.  
<img width="687" height="256" alt="microsteps-drv8825-snip" src="https://github.com/user-attachments/assets/cfb69f11-d256-4005-8756-5e025fe479bc" />  
**DRV8825 BOARD PROTECTION** - A 100µF Capacitor And 100nF Capacitor are wired across VMOT and GND to stabalize VMOT and Supress high frequency Noise.  
**POWER SUPPLY** - An external powersupply of 12V is required for the two DRV8825 connected to VMOT of both driver.  
## Homing  
At Setup both motots will move in -x and -y direction till the it hits its respective limiting switch. When it does it will Mark that position as origin of that axis.After both axes have been calibrated it will rely on step counting for its current position.  
# Software  
TeleRad consists of two software components  
## ESP32 Firmware- Acts as a Motion Controller.  
It handles 
- Stepper Motor Control.
- Homing.
- Position Tracking.
- Serial Command Processing.
## Python firmware 
- ESP32 Communication
- Mount Control
- SDR control
- Radio Measurments
- Scanning
# Setup  
## Hardware 
- Assemble the Mount.
- uhhh (More Detailed Instructions will be given after i build the mount.)
## Software  
### ESP32  
1.Install Arduino IDE 
2.Install ESP32 Board Support  
3.Install AccelStepper Library for Arduino IDE  
4.Open Firmware/ESP-32/ESP-32.ino in Arduino IDE  
5.Connect ESP32 with a USB data cable.  
6.Compile the .ino and uploAd it to the ESP.  
7.Test The serial connection by sending "PING" using Serial Moniter.  
### Python 
Clone the Repository and install the Python dependencies.  
cd Firmware/python  
pip install -e .  
Verify Installation wit "telerad --help"  
# Usage  
## Python cli  
Check Mount Status - "telerad mount status"
Check curremt Position - "telerad mount position"  
Home Mount - "telerad mount home"  
Move the mount - "telerad mount 90 45"  
Move Mount relavtive to its current position - "telerad mount rel 10 -5"  
Scan - "telerad scan start --from 20,30 --to 100,60 --step 1"  
NOTE: for Scan the coordinates are opposites corners of a rectangle. the bottom-left and top-right.



