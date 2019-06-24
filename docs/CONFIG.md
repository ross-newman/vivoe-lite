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
