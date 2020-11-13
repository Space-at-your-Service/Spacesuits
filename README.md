# Spacesuits
[temp name until spacesuit name is decided]

### [13 Nov 2020] Carl: Currently working on organising files into collaborative format
###### At this point anything here is just for reference, don't try to use it unless you know what you're doing
#### Hardware used:
+ Raspberry Pi ZW
+ Raspberry Pi 3B
+ Arduino Pro Mini


## System architecture guidelines

+ Lowest common denominator parts
++ COTS if available
+ 3V3 control bus
++ Avoid 5V, though many 5V MCU can be run at 3V3
+ 12-24V high power bus
++ Adjustable

## Mission operating parameters

+ Dark, cold environment
+ 2 hrs run time
+ 200m low bandwidth wireless range