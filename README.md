# TeleRad-A radio telescope  
### ![Radio Astronomy](https://img.shields.io/badge/Radio-Astronomy-black) ![ESP32](https://img.shields.io/badge/ESP32-Compatible-red) ![RTL-SDR](https://img.shields.io/badge/RTL--SDR-Supported-blueviolet)  ![Status](https://img.shields.io/badge/status-In%20Development-orange)  
NOTE: This project is currently in its design phase. No physical prototype has been made yet.  
A radio telescope mounted on an two axis alt-azimuth motorized mount for observing radio phenomenon.  
# About TeleRad  
I've always loved stargazing as a kid back in My rural Village . My love for the cosmos hasn't decreased , in fact it has increased manyfold. Alas ! Now i live in the In the Urban Area with light bleeding out every star . Even in my village I cant stargaze anymore.  
But is a few thousand lightbulbs going to stop me from observing the Night sky.  
NO , IT WILL NOT.  
TL;DR- I decided Viewing it in a diffrent ElectroMagnetic Spectrum would prove to be better as theres not as much radio pollution than light pollution. 
# What I Learned  
- Mechanical Design
- Embedded System
- Planning And Designing
- Communication protocol
- Optimizing Costs and Trade-offs
- Will Probs learn More during the building phase.

# Overview 
The Radio telescope will be made up of two systems.  
- Esp32 powered dual axis mount  
 (An Esp32 will control two stepper motors which will motorize two axes of the mount. The mount will move in a snake scan pattern across the sky.)
- Radio reciever  
  (An parabolic satallite ~75cm in diameter dish will will feed signal into a Low Noise Amplifier which goes through a Software Defined Radio dongle into a computer where it will be processed and converten into a 2d image map)  
## Bill of Materials
Heres everything needed for this project. [Bill Of Materials](BOM.csv)  
| Name | Quantity | Per Unit Cost (INR) | Total Cost (INR) | Total Cost (USD) | Link |
|---|---:|---:|---:|---:|---|
| Nema 17 Stepper Motor | 2 | 1109 | 2218 | 23.35 | [Link](https://robu.in/product/42hs60-1684-nema17-7-2kg-cm-stepper-motor-round-type/) |
| Nema 17 Mount Straight | 2 | 201 | 402 | 4.23 | [Link](https://robu.in/product/easymech-bracket-for-nema-17-stepper-motor-2/) |
| DRV8825 Stepper motor driver | 2 | 96 | 192 | 2.02 | [Link](https://robu.in/product/drv8825-stepper-motor-driver-aluminum-heat-sink-good-quality/) |
| 6008 Ball Bearing | 2 | 179 | 358 | 3.77 | [Link](https://www.amazon.in/gp/product/B08LPR978X/ref=ox_sc_act_title_7?smid=A1ATPDR7BUKQN6&th=1) |
| m3 x4mm brass inserts | 4 | 4.4 | 17.6 | 0.2 | [Link](https://onlyscrews.in/products/m3-x-4mm-3d-printing-brass-threaded-inserts-dia-3mm-length-4mm?variant=51684576002361) |
| m3 x6 mm screws | 14 | 1.2 | 16.8 | 0.18 | [Link](https://onlyscrews.in/products/hex-allen-csk-m3-x-6-screw-pack-of-20?variant=48591246098745) |
| M4 Hex Nut | 37 | - | - | - | Self Funded |
| M4 Bolt 35mm | 14 | - | - | - | Self Funded |
| M4 Bolt 25mm | 4 | - | - | - | Self Funded |
| M4 Bolt 40mm | 2 | - | - | - | Self Funded |
| Limit switches | 2 | 15 | 30 | 0.4 | [Link](https://robu.in/product/limit-switch-kv-05/) |
| 3m USB A to USB A | 1 | 399 | 399 | 4.2 | [Link](https://www.amazon.in/Storite-3-0-Extension-Transfer-Keyboard/dp/B00OFM6PFS/ref=srd_d_vsims_d_sccl_2_2/521-1981002-0848903?pd_rd_w=YYs8G&content-id=amzn1.sym.7ccbe032-5929-4c88-ab39-4923842061df&pf_rd_p=7ccbe032-5929-4c88-ab39-4923842061df&pf_rd_r=45MVWH7TCV3EFSNK0TGF&pd_rd_wg=OPtsm&pd_rd_r=cdff056c-0ccf-4b41-b96e-eeab9b3911db&pd_rd_i=B00OFM6PFS&th=1) |
| 22 Gague jumper wire | 2 | 149 | 298 | 3.14 | [Link](https://www.amazon.in/gp/product/B0B4WPX449/ref=ox_sc_act_title_2?smid=AJ6SIZC8YQDZX&psc=1) |
| Coaxial Cable | 1 | 294 | 294 | 3.09 | [Link](https://www.amazon.in/gp/product/B08254RHT3/ref=ox_sc_act_title_6?smid=A3G972E7L8N6MV&psc=1) |
| SMA Edge connector for DIY feed | 1 | 99 | 99 | 1.1 | [Link](https://www.amazon.in/Panel-Straight-Connector-Coaxial-Adapter/dp/B0965PSMSW/ref=sr_1_11?crid=K4K372VZLKRT&dib=eyJ2IjoiMSJ9.zapbte-I4E0YmV7nWSn-LJgEgSjFG5twdB3JbQwsVH7hftwGAPRKyDA1YTot9Qw04bnoE_RsyctxmZNRQSZGSU5s9xtkkIc0tb3tcAf4QGvryM43F81JnFxy-V1f7H0JWBhItqwSrjBgIQMKfvelENy4T6Z2r5asSDQE4wT3uUFUXpxOG_0Zv1gI_YULik8_quc1X11hPXyiRBITKOlJShkw-eFYqagFuSoo3_GqsyA.0p_AYy4ywc9mgCqi_41zGsQvj6B4ewY84YGO1kW_qcw&dib_tag=se&keywords=sma%2Bmale%2Bconnector&qid=1788887029&sprefix=Sma%2B%2Caps%2C330&sr=8-11&th=1) |
| RTL SDR V3 | 1 | 7678 | 7678 | 80.82 | [Link](https://www.amazon.in/gp/product/B0BMKB3L47/ref=ox_sc_act_image_1?smid=A1YZ9MMK17G6RH&th=1) |
| Low Noise Amplifier | 1 | 205 | 205 | 2.16 | [Link](https://robu.in/product/0-1-2000mhz-rf-wideband-amplifier-gain-30db-low-noise-amplifier-lna-board-module/) |
| ESP 32 | 1 | 549 | 549 | 5.78 | [Link](https://www.amazon.in/gp/product/B0D6GDV77P/ref=ox_sc_act_title_4?smid=AJ6SIZC8YQDZX&psc=1) |
| DC-DC 12V-3.3V | 1 | 49 | 49 | 0.52 | [Link](https://robu.in/product/dc-dc-voltage-converter-multi-output-power-supply-module-12v-3-3v5v12v/) |
| 12V 5A powersupply | 1 | 449 | 449 | 4.73 | [Link](https://robu.in/product/orange-ac-100-240v-to-dc-12v-5a-60w-power-adapter/) |
| Perf Board | 2 | 118 | 236 | 2.48 | [Link](https://www.amazon.in/gp/product/B09ZB4W17X/ref=ox_sc_act_title_1?smid=A31PUIOPHJ56Y3&psc=1) |
| 100 uF Capacitors | 5 | 2.36 | 11.8 | 0.2 | [Link](https://robu.in/product/100-uf-35v-through-hole-electrolytic-capacitor-dip-pack-of-10/) |
| 100nF Capacitor | 10 | 1.25 | 12.5 | 0.2 | [Link](https://robu.in/product/100nf-50v-disc-capacitor/) |
| 5x20 Cartridge Fuse 4A | 5 | 36 | 144 | 1.51 | [Link](https://robu.in/product/0239004-mxp-littelfuse-0239004-mxp-fuse-cartridge-time-delay-4-a-125-v-5mm-x-20mm-0-2-x-0-79-slo-blo-239/) |
| 5x20 Cartridge Fuse Holder | 1 | 30 | 30 | 0.31 | [Link](https://robu.in/product/0239004-mxp-littelfuse-0239004-mxp-fuse-cartridge-time-delay-4-a-125-v-5mm-x-20mm-0-2-x-0-79-slo-blo-239/) |
| allen key set | 1 | 200 | 200 | 2.09 | [Link](https://onlyscrews.in/products/allen-key-mm-set-m1-5-to-m10-chrome-vanadium-steel?variant=49975496671545) |
| 3D Prints | 1 | 2867.40 | 2867.40 | 30 | #printing-legion |
| **Total** | | | | **151.11** | |
## CAD  
The Basic Cad design Assembly Of the mount can be found at [TeleRad Assembly](CAD/Components%20STEP/Assembly/Assembly.step)
<img width="1280" height="1024" alt="TeleRad" src="https://github.com/user-attachments/assets/d340e0a2-323a-4b93-8e6f-24d2d4935322" />

The Mount utilizes Gears to increase torque.
<img width="1920" height="1080" alt="Screenshot (7)" src="https://github.com/user-attachments/assets/1b288a7f-7db5-4c8e-81af-5256dfdb4b58" />  
## Gears  
### Gear Ratio
The gear Ratios for both axes are as follows :-  
For The Altitude Rotation - Input gear teeth = 16T ; Output gear teeth = 128T  
For The Azimuth  Rotation - Input gear teeth = 24T ; Output gear teeth = 72T  
### 3D printing  
The STL files to be present are int [3D Print STL](CAD/3D%20Print%20STL/)
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
- Check Mount Status
  ```bash
  telerad mount status
- Check curremt Position
  ```bash
  telerad mount position
- Home Mount
  ```bash
  telerad mount home  
- Move the mount
  ```bash
  telerad mount move 90 45  
- Move Mount relavtive to its current position
  ```bash
  telerad mount rel 10 -5  
- Scan
  ```bash
  telerad scan start --from 20,30 --to 100,60 --step 1 
NOTE: for Scan the coordinates are opposites corners of a rectangle. the bottom-left and top-right.
- Park the mount
  ```bash
  telerad mount park
- Stop the mount
  ```bash
  telerad mount stop
# AI Declaration  
AI helped with creation of the Firmware Of this project. 




