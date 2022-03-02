# Arduino Spot Welder Control

#### *!--This project is still a work in progress--!*

### Introduction

This is my version of a DIY spot welder controller. It is intended for use with a modified microwave oven transformer, but may suit other applications too. I have included the PCB Gerber files, BOM (build of materials) for electrical components and Arduino code. 

The main goal of this project is to make a feature rich, low cost, controller for a spot welder. That can reproduce repeatable consistent welds as per the user requirements. 

**PLEASE NOTE:** *This is a mains powered project and requires modifying a microwave oven transformer for high current operation, attempt at your own risk.*

### Features

To date the current features of this controller are;

- Single Pulse Welding
- Double Pulse Welding
- Load/Save Settings
- Temperature Controlled Cooling Fan
- OLED Display
- Welding Triggered Alert Tone

### PCB Gerber Files

Gerber files for PCB manufacture can be found in the Gerber folder.

- Pick the newest revision/version
- Visit https://jlcpcb.com/
- Sign In or Register
- Click 'Order Now'
- Click 'Add Gerber File'
- Select the Gerber .zip file
- Select build options. (I recommend leave defaults, but change qty, colour and select 'Specify a location' under 'Remove Order Number' option. Also make sure both 'SMT Assembly' and 'Stencil' options are off as they are not needed for this.)
- Once your happy with your selection, place order and pay.
- Wait for PCB's to arrive.

### BOM (Build of Materials)

Build of materials can be found the BOM folder. Most of the PCB components can be found in a .csv file, the rest you will need to source yourself from the .txt file. I would recommend places like Amazon, eBay and AliExpress.

- Find the .csv that matches the same revision as the Gerber file.
- Next visit https://lcsc.com/
- Sign In or Register
- Click on the 'BOM Tool' option.
- Now upload the .csv file.
- Set column with C##### numbers to LCSC Part Number.
- Click Next
- Adjust quantities to suit your needs. (Note some parts are optional and other have minimum order quantities)
- Once your happy with your selection, place order and pay.
- Wait for parts to arrive.

Now all you need to do is source the rest of the parts in the .txt file and your ready to start building.
