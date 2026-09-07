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
# Planned components
|Name                        |Quantity|Per Unit Cost (INR)|Total Cost (INR)|Total Cost (USD)|Link                                                                                                     |Distributor|FIELD8|
|----------------------------|--------|-------------------|----------------|----------------|---------------------------------------------------------------------------------------------------------|-----------|------|
|Nema 17 Stepper Motor       |2       |1109               |2218            |23.35           |https://robu.in/product/42hs60-1684-nema17-7-2kg-cm-stepper-motor-round-type/                            |Amazon     |      |
|DRV8825 Stepper motor driver|2       |96                 |192             |2.02            |https://robu.in/product/drv8825-stepper-motor-driver-aluminum-heat-sink-good-quality/                    |Amazon     |      |
| Nema 17 Mount Straight     |2       |201                |402             |4.23            |https://robu.in/product/easymech-bracket-for-nema-17-stepper-motor-2/                                    |Amazon     |      |
|DC-DC 12V-3.3V              |1       |49                 |49              |0.52            |https://robu.in/product/dc-dc-voltage-converter-multi-output-power-supply-module-12v-3-3v5v12v/          |Amazon     |      |
|12V 5A powersupply          |1       |449                |449             |4.73            |https://robu.in/product/orange-ac-100-240v-to-dc-12v-5a-60w-power-adapter/                               |Amazon     |      |
|RTL SDR V3                  |1       |7678               |7678            |80.82           |https://www.amazon.in/gp/product/B0BMKB3L47/ref=ox_sc_act_image_1?smid=A1YZ9MMK17G6RH&th=1               |Amazon     |      |
|Low Noise Amplifier         |1       |205                |205             |2.16            |https://robu.in/product/0-1-2000mhz-rf-wideband-amplifier-gain-30db-low-noise-amplifier-lna-board-module/|           |      |
|22 Gague jumper wire        |2       |149                |298             |3.14            |https://www.amazon.in/gp/product/B0B4WPX449/ref=ox_sc_act_title_2?smid=AJ6SIZC8YQDZX&psc=1               |Amazon     |      |
|Perf Board                  |2       |118                |236             |2.48            |https://www.amazon.in/gp/product/B09ZB4W17X/ref=ox_sc_act_title_1?smid=A31PUIOPHJ56Y3&psc=1              |Amazon     |      |
|ESP 32                      |1       |549                |549             |5.78            |https://www.amazon.in/gp/product/B0D6GDV77P/ref=ox_sc_act_title_4?smid=AJ6SIZC8YQDZX&psc=1               |Amazon     |      |
|Coaxial Cable               |1       |294                |294             |3.09            |https://www.amazon.in/gp/product/B08254RHT3/ref=ox_sc_act_title_6?smid=A3G972E7L8N6MV&psc=1              |Amazon     |      |
|6008 Ball Bearing           |2       |179                |358             |3.77            |https://www.amazon.in/gp/product/B08LPR978X/ref=ox_sc_act_title_7?smid=A1ATPDR7BUKQN6&psc=1              |           |      |


							

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
