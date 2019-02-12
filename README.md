# vivoe-lite
# Dependancies
This VIVOE (Vetronics Infrastructure for Video Over Ethernet) environment is currently tested on Ubuntu 18.04 LTS. Please ensure you have the following packages installed prior to building the application:
```
sudo apt install libcairo2-dev libxt-dev libsdl2-dev doxygen
```
# Conformance to UK MoD / NATO Defence Standards
## DEF STAN 23-009 - Generic Vehicle Architecture (GVA)
Defence Standard 23-09 is currently split into the parts below:

*  Part 0 - The GVA Approach
*  Part 1 - Infrastructure (Data and Power)
*  Part 2 - Human Machine Interface
*  Part 3 - Health and Usage Monitoring (issued Dec 2015)
*  Part 4 - Physical Interfaces (withdrawn)
*  Part 5 - GVA Data Model and Model Driven Architecture[6]
*  Part 6 - Security (TBD)
*  Part 7 - Common Services (TBD)
*  Part 8 - Safety (TBD)

## DEF STAN 00-083 - Vetronics Infrastructure for Video Over Ethernet (VIVOE)
Defence Standard 00-083 is currently split into the parts below

*  Vetronics Infrastructure for Video Over Ethernet Part: 0 : Guidance 
*  Vetronics Infrastructure for Video Over Ethernet Part: 1 : Standards and Protocols 
*  Vetronics Infrastructure for Video Over Ethernet Part: 2 : Extensions for Audio and Acoustic Data 

This standard is also mandated in the NATO version of GVA (NGVA). 

## STANAG 4754 - NATO Generic Vehicle Architecture (NGVA)
NGVA is a NATO Standardisation Agreement (STANAG 4754) based on open standards to design and integrate multiple electronic sub-systems onto military vehicles which are controllable from a multifunction crew display and control unit. 

## STANAG 4697 - Platform Extended VIdeo StandarD (PLEVID)
AIM The aim of this NATO standardization agreement (STANAG) is to respond to the following interoperability requirements. Adopts DEF STAN 00-083 and allows for GigE.

# Rendering
The HMI uses Cairo for rendering the GUI components.

# Links
* https://www.cairographics.org/
* https://www.vetronics.org/
* https://en.wikipedia.org/wiki/Generic_Vehicle_Architecture
