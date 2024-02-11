# Champion WiFi Automatic Transfer Switch WiFi->serial bridge

## WiFi
This platformIO project uses an esp8266 (wemos D1 mini in my case) with an SSD1306 OLED to provide a WiFi->serial bridge to connect to the Champion WiFi enabled automatic transfer switches that integrate with their home standby generators. This is needed as their implemetation for generator "connectivity" is a WiFi access point with a hardcoded gateway IP (192.168.0.90) that you can't otherwise change.

The idea is that an esp* device will sit between a computer that needs to pull the real-time generator/ATS data when issued a command over UART0 (the USB interface by default on most of these esp* devices). The order of operations is this:
1. That esp* device will connect to the ATS WiFi network (`Champion-NNNNNNNNN`) and listen for a command from the host PC over serial. 
2. If that command is received the esp* then issues an HTTP GET request to the `http://192.168.0.90/get_info` URL, which returns a JSON object with real-time values for status, voltage, current, etc...
3. That JSON string is returned as-is to the host PC, and will need to be transformed by the host PC. (It's a big JSON string, and many of the keys are misspelled so you may want to do some manipulation anyway.)

No other generator or transfer switch control is implemented, just data monitoring. You can take this and use it how you want to integrate data into Home Assistant or some other monitoring service.

**NOTE:** Champion provides this URL as a *convenience*, and does not intend it to be used for long-term monitoring. That's their words anyway.

## Native serial interface
This transfer switch also provides a USB-B port in the upper left hand corner of the ATS, which presents itself as an ACM device (`/dev/ttyACM0` on my Linux laptop when I tried.). The connection is at 9600 8N1. Once connected typing `?` gives a list of commands you can issue. Typing `1` will give you the real-time status.

This USB port is _RIGHT NEXT TO LIVE 240V AC_!! **BE CAREFUL IF YOU USE IT!!**