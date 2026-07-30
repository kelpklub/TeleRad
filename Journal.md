# log 1  
Today was day 0 for this project.I decided on what i was gonna do and how to go about it.I will be creating a radio telescope.I got inspiration for this project from motorised telescopes i always wanted as a kid.
![image](https://cdn.hackclub.com/019deeb7-d8bd-75ee-bb4e-4075e5cbf7c8/image.png)
Ill have an antenna hooked up to a low noise amplifier which will then plug into a RTL-SDR to get the radio signals into my laptop.The antenna will be mounted on an alt-azimuth(i.e. a mount which can move in 2 axes) motorised mount .The mount will be powered by a battery pack and controlled using an arduino uno clone(since arduino open sources all its hardware.) The mount will move the antenna across the night sky like a scan. The readings will then be recorded and converted into a false color image using those readings.Hopefully at the end of it the images will look similar to this picture.
![image](https://cdn.hackclub.com/019deeb8-0ab9-7222-90f9-6abd0ac33358/image.png)  
# log 2  
## Github README  
wrote an overview, planned hardware section on the readme page of my github repo.

![image](https://cdn.hackclub.com/019deeba-a6ef-7190-844e-4310c2146fcb/image.png)

couldn't decide if i wanted to grab a tv satellite dish or make one myself using wire mesh .Decided on a horn antenna tuned to 1420.405 Mhz . i.e. the frequency of the hydrogen line using the following diagram and the guide  https://physicsopenlab.org/2020/07/20/horn-antenna-for-the-21cm-neutral-hydrogen-line/

![image](https://cdn.hackclub.com/019deeba-d16d-7222-b408-057d95b82f09/image.png)  
# log 3  
## Thoughts  
Previously i had decided on making my own horn antenna because i wasnt sure if the Nema 17 Could rotate the heavy ~5kg dish against gravity.Realized i was being stupid, how could i forget about neutral equilibrium (Learned about that three year back  in physics class).Instead of having the stepper motor rotate the the dish on its own, i could add a counterweight to have the altitude system in neutral equilibrium so the nema 17 could actually handle it.I also decided to use a low Gear ratio to increase torque.Found out double Helical gaer is best for my need as Multiple teeth engage at the same time reducing wear.Since my Torque problem is fixed i will be using a Parabolic tv dish.  
## Design  
Drew up a basic design in school.
![image](https://cdn.hackclub.com/019deebd-d748-7804-a3a8-fe99c48c5c06/image.png)
Realized it had a lot of flaws so i drew up a redisgn.
![image](https://cdn.hackclub.com/019deebd-f277-704f-a147-b90ecfc62946/image.png)  
The Load here is the satellite dish.Im not an artist so couldnt draw the satellite dish.
## CAD  
spent like 5 hours modelling the design in Autodesk Fusion.
![image](https://cdn.hackclub.com/019deebd-4924-7f26-a662-4ec827045187/image.png)
This was my second time making a CAD so the design is rough around the edges.The first time was yesterday where i learned the basics.Below is all the parts i modelled today except the bearing(Got it from GRABcad)
![image](https://cdn.hackclub.com/019deebd-6c3b-77b2-98b4-23c01a7eadc9/image.png)

# log 4  
## thoughts  
started with a clear plan for today -research nema 17 torque and what gear ill have to use.make gears for azimuth input and output do the same for altitude but higher gear ratio.finish CAD. I was able to make 3 gears leaving that of altitude output.AS for CAD im like 90% done (hope i havent forgotten anything in CAD)
## Gears   
started making gears in fusion spent like an hour an a half making gears by hand. (this time is not included)
then i thought "fusion is an EXPENSIVE  software why doesnt it have auto gear genaration like it does for threadding".googled it up and found out it does have it in the form of add ins(its so obvious now.)
used the GF gear generator to make hearingbone gears AKA double helical gears.after making 2 gears i forgot the module,direction  and pressure angle of the gear.Ithought the add in would save the info but it didn't.so now im saving all detail of the gears on my notes app.never making thar mistake again.
## Gear ratio  
For Azimuth i am going with a gear ratio of 1:3
the azi-input gear has 24 teeth and azi-output has 72 teeth.  
For altitude im going with 12 teeth for input and 72 teeth for output.  
Both input gear also have 2 threads on opposite side to insert a M3 screw to clasp on to the Nema 17 Shaft

![image](https://cdn.hackclub.com/019deebf-d771-7f4e-b156-3ab08fb63e13/image.png)
All gears have a module fo 1.5mm and height of 20mm  
## joining gears  
i spent more time than i would like to admil making the motion link between the two azimuth gears .looking back on it i now realize it was quite simple.
![image](https://cdn.hackclub.com/019deec0-0234-7553-b7b9-33b8abfc24eb/image.png)
## Almost done with CAD design  
ive finished most of the cad design,today i joined up the parts i made yesterday as well as modify almost all parts ,(namely baseplate ,altitude baseplate,altitude rod,)
heres how it looks 
![image](https://cdn.hackclub.com/019deec0-4c9b-7017-9ed3-1197904fc215/image.png)
## To do (CAD)  
add nema 17 to the altitude plate  
desing the gear for altitude output  
fix the gears in place  
Get better naming sense(like what is "Altitude input gear")  
# log 5  
## Thoughts  
A holiday after so long...planned on Finishing my CAD today and I finished it.
![image](https://cdn.hackclub.com/019deec3-3272-788f-9a73-8cde31f3ca34/image.png)
Was going through the submission guidlines and realised i still have to do my firmware. I have the Algorithm in mind so it will probably not take that long.  
## BOM Price fluctuations  
I was going through my BOM again and noticed quite a lot of prices had changed.I think iwill have to correct the prices before i submit for review and it might even change during the review process .   

  .  
 
Short journal entry today as i had extra classes (even though its holiday today (T-T) silver lining was that it was maths and quite fun)  
# log 6  
## Thoughts  
First thought i would use a arduino for this but now ive realized that i should be using a esp32 instead 
![image](https://cdn.hackclub.com/019deec5-70e0-7dd7-baef-3c9926e5f3ce/image.png)
Its got Builtin wifi which will allow it so communticate with my laptop. I will be able to pull up Planetary coords of objects that i wouuld like to observe on my laptop send it to the esp compare it to the mpu9250 data and point at that object with live feedback.  
## How the Pointing will happen  
i will send two coordinates in equatorial coordinate system to the esp32. it will make a rectangle with those points as opposite corners.it will then divide that rectangle into a variable no of small squares.the individual squares will become somthing akin to a pixel on a screen and the number of rows and columns will become its resolution. it will check it own pointing coordinates and then move the system to the upper-left corner and do a scan motion till the lower-right corner stopping at each pixel to record its radio intensity. the intensity will be stored into a 2-D array of same size as the resolution.This array will then be made into a heatmap of sorts.  
# log 7  
## Thoughts  
i asked in #macondo-help and got some feedback. the pins that aren't used need to have  the X symbol to signify that it isnt connected to anything. Added a MPU9250 to the wiring sheet.  
## Wiring  
Ive added wiring so that the drivers are in 16 microsteps/second ive made a custom esp32 symbol and 9250 symbol and wired it up respectively.
![image](https://cdn.hackclub.com/019e1049-29ff-733b-9d48-e197e67293fe/image.png)  
## What's Next  
for the submition im missing just the firmware. Ill start that today.hopefully finish it today as well.  
# log 8  
## Firmware  
started on my esp32 code it now has the basic server stuff.it hosts a webpage that has four text box  and a submit button .two text box combined take the coordinate of the bottom-left corner and another two take the coordinate of the upper right corner.
![image](https://cdn.hackclub.com/019e1739-3fdc-7d2d-ab2b-ae8de4d7605c/image.png)  
## Repository  
polished up the repo-added the individual f3d files of all the individual parts that need 3d printing.  
# log 9  
## thoughts  
started this session with a clear goal of colpleting the esp32 code.the code its now done to a point im satisfied with.  
im missing the code to get the current pos from mpu9250.  
## Algorithm 
the code accepts two coordinates from a webpage,  
while(currentpos!=coord2)  
{the system points to coord1,then moves coord2.y,then moves in x by a defined step variable,then it moves to coord1.y.}  
theres probs a lot of errors and bugs.will have to ask someone in #macondo-help  
## Webpage  
the webpage now has a new look. its an improvement from the old look but not by much.  
![image](https://cdn.hackclub.com/019e319b-2a9b-715c-8edd-e265041f14d9/image.png)  
## Next  
next i need to improve the CAD and ill also probably read some docs on how a cnc machine works since they have quite a lot of similarities.  
# log 10  
## Research  
So my idea of using a mpi9250 may not work , the sensor has sensor drift which will result in inacurate movement.SO i Looked on the web and found a solution.  
## Solution  
instead of relying on a sensor for position itll rely on the a step counter which will count how many steps the system has moved , basically when the system powers up tit will move in -x and -y till it hits a limiting switch.
![image](https://cdn.hackclub.com/019e5f9c-6155-7a1a-a09f-baa976de1a2e/image.png)when it hits the switch it stops in that axis. after it has  hit both switches it will mark those points as (0,0) then it will goto the points the webserver will recieve.and start the tracking.  
NOTE-the entire system will have to be facing North,which will have to be done manually.  
## log 11  
# CAD  
Added Limiting switrches to CAD (just the switches no wires)  
for Y AXIS  
![image](https://cdn.hackclub.com/019e796d-88e6-772c-89df-9974642a7568/homing%20switch%20y.png)  
FOR X AXIS  
![image](https://cdn.hackclub.com/019e796d-d428-7b35-ac8c-d15222df12b2/homing%20switch%20x.png)
## Wiring  
Did the wiring for switches also removes MPU9250.  
![image](https://cdn.hackclub.com/019e796e-e293-7856-8977-b71c959e5d39/image.png)  
## Calibration Idea  
At Setup both motots will move in -x and -y direction till the it hits its respective limiting switch. When it does it will Mark that position as origin of that axis.After both axes have been calibrated it will rely on step counting for its current position.  
## TODO  
- Firmware Update, need to adjuct esp sketch   
- COdE to get SDR Readings in sync with motor movement(idea-use esp32 server to send a signasl to my computer which starts a python script to start recording ).Convert reading to heatmap(idea- use 2D array).  
# log 12  
## missing code  
have not updated my esp32 code after deciding to ditch the mpu9250 and using step counting . ![image](https://cdn.hackclub.com/019faeb3-210c-7687-86ee-4549f4176f85/image.png).
