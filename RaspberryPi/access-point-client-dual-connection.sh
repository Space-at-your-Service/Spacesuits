curl https://raw.githubusercontent.com/lukicdarkoo/rpi-wifi/master/configure | bash -s -- -a mesh4 meshberry -c 'SSID' wifiPa$$wort

 /etc/network/interfaces:

post-up iw ap0 set power_save off
post-up iw wlan0 set power_save off