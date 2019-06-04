# vivoe-lite
# Dependancies
This VIVOE (Vetronics Infrastructure for Video Over Ethernet) environment is currently tested on Ubuntu 18.04 LTS. Please ensure you have the following packages installed prior to building the application:
```
sudo apt install libcairo2-dev libxt-dev libsdl2-dev doxygen
```
# Configuration
GVA parameters and system wide setting are read in from the .xml configuration files. These are listed below:
## config.xml
This is a list of the config filenames. This is the only file that needs to be read into the HMI application to be able to read all the settings.
```
<?xml version="1.0"?>
<Config>
  <XmlFiles>
     <GvaConfig>gva-config.xml</GvaConfig>
     <VideoSourcesConfig>video-sources-config.xml</VideoSourcesConfig>
     <DisplaysConfig>displays-config.xml</DisplaysConfig>
     <HmiCommanderConfig>hmi-commander-config.xml</HmiCommanderConfig>
     <DdsConfig>dds-config.xml</DdsConfig>
  </XmlFiles>
</Config>
```
## gva-config.xml
Nearly all the input sources are 4:3 input ratio with the exception of SXVGA which is 5:4. 16:9 may be added later when the HMI spec allows. Colours are taken from the HMI spec.
```
<?xml version="1.0"?>
<Gva>
  <Resolutions>
    <Mode>
      <Name>QVGA</Name>
      <Dimension>320x240</Dimension>
    </Mode>
    <Mode>
      <Name>VGA</Name>
      <Dimension>640x480</Dimension>
    </Mode>
    <Mode>
      <Name>PAL</Name>
      <Dimension>768x5760</Dimension>
    </Mode>
    <Mode>
      <Name>SVGA</Name>
      <Dimension>800x600</Dimension>
    </Mode>
    <Mode>
      <Name>XVGA</Name>
      <Dimension>1024x768</Dimension>
    </Mode>
    <Mode>
      <Name>HIRES</Name>
      <Dimension>1280x960</Dimension>
    </Mode>
    <Mode>
      <Name>SXVGA</Name>
      <Dimension>1280x1024</Dimension>
    </Mode>
    <Mode>
      <Name>SXVGA+</Name>
      <Dimension>1400x1050</Dimension>
    </Mode>
    <Mode>
      <Name>UXVGA</Name>
      <Dimension>1600x1200</Dimension>
    </Mode>
    <Mode>
      <Name>QXVGA</Name>
      <Dimension>2048x1200</Dimension>
    </Mode>
  </Resolutions>
  <Presentation>
    <ColourOnGoodOkSafe>#00FF00</ColourOnGoodOkSafe>
    <ColourAdvisoryWarning>#FFFF00</ColourAdvisoryWarning>
    <ColourCaution>#FF9900</ColourCaution>
    <ColourArmedDangerCriticalWarningFailureStop>#FF0000</ColourArmedDangerCriticalWarningFailureStop>
    <ColourNotAvailableOff>#7F7F7F</ColourNotAvailableOff>
    <ColourBackgroundSoftKeySoftLabel>#004B00</ColourBackgroundSoftKeySoftLabel>
  </Presentation>
</Gva>
```
## video-sources-config.xml
Valid inputs will use the allowed resolutions from **gva-config.xml**
```
<?xml version="1.0"?>
<Inputs>
</Inputs>
```
## displays-config.xml
This file describes the screen types available on the platform
```
<?xml version="1.0"?>
<Displays>
  <Screen>
    <Name>Commander_1<Name>
    <Resolution>VGA<Resolution>
  </Screen>
  <Screen>
    <Name>Driver_1<Name>
    <Resolution>VGA<Resolution>
  </Screen>
  <Screen>
    <Name>Driver_2<Name>
    <Resolution>VGA<Resolution>
  </Screen>
  <Screen>
    <Name>Gunner_1<Name>
    <Resolution>VGA<Resolution>
  </Screen>
</Displays>
```
## hmi-commander-config.xml
This describes the HMI screens and funstions available to the Commander.
```
<?xml version="1.0"?>
<Commander>
  <Screen>
    <Name>Commander_1<\Name>
    <SYS>...<\SYS>
    <BMS>...<\BMS>
  </Screen>
</Commander>
```
# Conformance to UK MoD / NATO Defence Standards
The sample HMI provided was created to comply with the design guidline layed out in Defence Standard 23-09 Part 2 Generic Vehicle Architecture (GVA) Human Machine Interface, Issue 3 as shown below.
## DEF STAN 23-09 - Generic Vehicle Architecture (GVA)
Defence Standard 23-09 is currently split into the parts below:

*  Part 0 - The GVA Approach
*  Part 1 - Infrastructure (Data and Power)
*  **Part 2 - Human Machine Interface**
*  Part 3 - Health and Usage Monitoring (issued Dec 2015)
*  Part 4 - Physical Interfaces (withdrawn)
*  Part 5 - GVA Data Model and Model Driven Architecture[6]
*  Part 6 - Security (TBD)
*  Part 7 - Common Services (TBD)
*  Part 8 - Safety (TBD)

## DEF STAN 00-83 - Vetronics Infrastructure for Video Over Ethernet (VIVOE)
For the video streaming element of the sample HMI the RTP raw video streams conform to Defence Standard 00-83 Vetronics Infrastructure
for Video Over Ethernet Part 1: Standards and Protocols, Issue 2. Specifically Appendix B2 YCbCr 4:2:2 Encoded Uncompressed Video.

Defence Standard 00-083 is currently split into the parts below.

*  Vetronics Infrastructure for Video Over Ethernet Part: 0 : Guidance 
*  **Vetronics Infrastructure for Video Over Ethernet Part: 1 : Standards and Protocols**
*  Vetronics Infrastructure for Video Over Ethernet Part: 2 : Extensions for Audio and Acoustic Data 

This standard is also mandated in the NATO version of GVA (NGVA). 

## STANAG 4754 - NATO Generic Vehicle Architecture (NGVA)
NGVA is a NATO Standardisation Agreement (STANAG 4754) based on open standards to design and integrate multiple electronic sub-systems onto military vehicles which are controllable from a multifunction crew display and control unit. 

## STANAG 4697 - Platform Extended VIdeo StandarD (PLEVID)
AIM The aim of this NATO standardization agreement (STANAG) is to respond to the following interoperability requirements. Adopts DEF STAN 00-083 and allows for GigE.

# Rendering
The HMI uses Cairo for rendering the GUI components.

![GVA HMI](images/GVA_HMI.png)

**GVA HMI Example layout**

# Links
* https://www.cairographics.org/
* https://www.vetronics.org/
* https://en.wikipedia.org/wiki/Generic_Vehicle_Architecture
