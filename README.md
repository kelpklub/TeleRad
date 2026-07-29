# TeleRad-A radio telescope  
NOTE: This project is currently in its design phase. No physical prototype has been made yet.  
A radio telescope mounted on an two axis alt-azimuth motorized mount for observing radio phenomenon.  
# Overview 
The Radio telescope will be made up of two systems.  
- Esp32 powered dual axis mount  
 (An Esp32 will control two stepper motors which will motorize two axes of the mount. The mount will move in a snake scan pattern across the sky.)
- Radio reciever  
  (An parabolic satallite ~75cm in diameter dish will will feed signal into a Low Noise Amplifier which goes through a Software Defined Radio dongle into a computer where it will be processed and converten into a 2d image map)  
# Planned components
- Mount  
  1.Esp32
  2.Stepper motor 2x(NEMA17)  
  3.Stepper motor driver board 2x(DRV8825)  
  4.Limiting Switch 2x  
  5.BreadBoard and wires for easy wiring 
- Radio reciever  
  1.A Antenna  
  2.A Low noise amplifier  
  3.A Software defined radio dongle (RTL-SDR)  
  4.A computer  
  5.SMA connectors.
# Bill of Materials
Heres everything you need for this project. [Bill Of Materials](BOM.csv)  
# CAD  
The Basic Cad design Assembly Of the mount can be found at [CAD Radio Telescope](<Radio Telescope.step>)
<img width="1920" height="1080" alt="CAD Assembly" src="https://github.com/user-attachments/assets/61eeb32e-ff1f-426d-9cc0-bc1dd4e0e6fb" />
The Mount utilizes Gears to increase torque.
<img width="1920" height="1080" alt="Screenshot (7)" src="https://github.com/user-attachments/assets/1b288a7f-7db5-4c8e-81af-5256dfdb4b58" />  
# Gear Ratio  
The gear Ratios for both axes are as follows :-  
For The Altitude Rotation - Input gear teeth = 16T ; Output gear teeth = 96T  
For The Azimuth  Rotation - Input gear teeth = 24T ; Output gear teeth = 72T  
# Basic Wiring  
There are 7 diffrent component to be wired up. 1-ESP32, 2-DRV8825, 2-NEMA17 ,2-Homing Switches.  
The wiring is as follows:-  
 <img width="1447" height="886" alt="TeleRad_CircuitDiagram" src="https://github.com/user-attachments/assets/fdcb4878-7840-4647-bb8b-0cf63fa270bb" />  
**MICROSTEPPING** - The M2 pin of both driver boards are connected to 3.3v of esp32 to set microstepping to 16 microsteps/second following the documentation of the drv8825 board.  
<img width="687" height="256" alt="microsteps-drv8825-snip" src="https://github.com/user-attachments/assets/cfb69f11-d256-4005-8756-5e025fe479bc" />  
**DRV8825 BOARD PROTECTION** - A 100µF Capacitor is wired across VMOT and GND to stabalize voltage and protect the board from damage.  
**POWER SUPPLY** - An external powersupply of 12V is required for the two DRV8825 connected to VMOT of both driver.  
# Coordinate Calibration  
At Setup both motots will move in -x and -y direction till the it hits its respective limiting switch. When it does it will Mark that position as origin of that axis.After both axes have been calibrated it will rely on step counting for its current position.





