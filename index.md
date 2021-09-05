## Fish Tank Solution Mixer

A mini-project purpose built for a vat of calcium carbonate solution. The device fits onto the lid of the vat, where a mixing head dips into the solution. A time interval can be set in between mixing cycles.

### The Problem

An alkaline solution of calcium carbonate is used for the operation of a tropical fish tank. However, the solution becomes saturated with extra particulates settling at the bottom. 



### Design Constraints

The effectiveness of mixing is the most important aspect of this problem. The mixer should be able to effectively mix the settled particles from the bottom of the vat, in a way that nearly homogenously distributes it within the rest of the solution. A secondary requirements of the device is the displacement of the mixing head and the material used for it. Too much displacement of liquid in the vat will reduce the efficiency of the Fish Tank system. As for the mixing head's material, it must not degrade or react within the solution. A tertiary consideration was the power consumption of the device. Ideally, the device should barely use any power between untervals, and should use power efficienty when mixing.

<image>
<table><tr>
  <td> <div style="text-align: center"><img src="images/calcium.jpg" alt="Calcium" height=35% width=35%/></div> </td>
  <td> <div style="text-align: center"><img src="images/carbonate.jpg" alt="Carbonate"  height=35% width=35%/><div/> </td>
</tr><table>
</image>
  
### Initial Approaches

The device will have 3 main aspects: Controller, Mixer, and Power. The first revision of the mixing head was meant to simply induce a vortex within the vat, but the particles at the bottom would not be lifted up efficiently.

<img src="images/toplevel1.JPG" alt="Top Level 1" class="inline" height=50% width=50%/>

A fully analog system using a 555 timer was considered, but would lmit flexibility and accuracy of adjusting the time interval. In addition, it would limit future functionality if the device were to be integrated into a larger Fish Tank management system.

The production of the mixing head and enclosure was to be done with an FDM 3D printer, with Polylactic Acid (PLA) as the material. However, this would not be fit for the fish tank's environment. PLA is brittle and degrades when exposed to water for long periods. In addition, calcium deposit build up may be difficult to remove and clean. 

To power the device, having only a set of 4 AA Batteries to power it would have been sufficient, but may not have been enough due to the required amount of power to mix the solution.

### Current Revision
  
<img src="images/toplevel2.PNG" alt="Top Level 2" class="inline" height=50% width=50%/>

Progressing from the previous approaches, the controller was upgraded into an Arduino Nano, underclocked at 8Mhz to lower power usage. This will allow for flexibility in the devices function if additional features like bluetooth or IoT functionality is added. This controller can also vary the movement of the motor to better mix the solution.
  
The device's enclosure and mixer will be FDM 3D Printed using PET-G, a common material used in water bottles and food containers. It was desireable for its inactivity with the solution, higher strength, and its tendency to deform instead of fracture at failure. It can also be submerged for long period of time without degradation. The mixing head was moved closer to the bottom, and was modeled to resemble paint mixer heads. The shaft was also extended from the enclosure. In the middle, the shaft was split into two components to allow for ease of transport. 
  
To adjust the time interval, a rotary encoder and an OLED display were added. Rotating the encoder would adjust the interval by the minute, which would then be displayed. A transistor would be used to switch the display on or off in order to save power between mix cycles.
  
To power the device, a combination of battery power and USB power. If a plug is accessible, USB power can be used. Otherwise, battery power can be used but at a reduced lifetime. Note, both cannot be used at the same time because there is no switching functionality. 
  
