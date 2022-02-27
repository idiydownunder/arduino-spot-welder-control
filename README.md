# Arduino Spot Welder Control

#### This is still a work in progress

This is my version of a DIY spot welder. I have included the PCB Gerber files, BOM (build of materials) for eletrical componants and Arduino code.

### Gerber (PCB Files)

Gerber files for PCB manufactor can be found in the Gerber folder.

- Pick the newest revison/version
- Visit https://jlcpcb.com/
- Sign In or Register
- Click 'Order Now'
- Click 'Add Gerber File'
- Select the Gerber .zip file
- Select build options. (I recomend leave defaults, but change qty, color and select 'Specify a location' under 'Remove Order Numer' option. Also make sure both 'SMT Assembly' and 'Stencil' options are off as they are not needed for this.)
- Once your happy with your selection, place order and pay.
- Wait for PCB's to arrive.

### BOM (Build of Materials)

Build of materials can be found the BOM folder. Most of the PCB componants can be found in a .csv file, the rest you will need to source yourself from the .txt file. I would recomend places like Amazon, eBay and AliExpress.

- Find the .csv that matches the same revison as the Gerber file.
- Next visit https://lcsc.com/
- Sign In or Register
- Click on the 'BOM Tool' option.
- Now upload the .csv file.
- Set colume with C##### numbers to LCSC Part Number.
- Click Next
- Once your happy with your selection, place order and pay.
- Wait for parts to arrive.

Now all you need to do is soure the rest of the parts in the .txt file and your ready to start building.
