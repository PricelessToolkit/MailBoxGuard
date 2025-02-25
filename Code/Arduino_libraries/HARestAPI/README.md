# HARestAPI

An Arduino library to talk to Home Assistant using [Rest API](https://www.home-assistant.io/developers/rest_api/) made for ESP8266.

[![HARestAPI](https://img.youtube.com/vi/XV_X3e7xwDE/0.jpg)](https://www.youtube.com/watch?v=XV_X3e7xwDE)


## Beta
* Can do POST effectively and not GET (reply is too long for esp8266). Try using `sendGetHA(URL,Component)` and provide feedback.

## Using the Library
* Download this GitHub [library](https://github.com/debsahu/HARestAPI/archive/master.zip).
* In Arduino, Goto Sketch -> Include Library -> Add .ZIP Library... and point to the zip file downloaded.

To use in your sketch include these lines.
```
#include <HARestAPI.h>
```
Declare clients before setup().
If using HA with no SSL declare WiFiClient and pass it to HARestAPI.
```
WiFiClient client;
HARestAPI ha(client);
```
If using HA with SSL declare WiFiClientSecure and pass it to HARestAPI.
```
WiFiClientSecure sclient;
HARestAPI ha(sclient);
```
In setup make sure to setup IP and port of HA server. Default is 8123 and if using SSL 443.
```
const char* ha_ip = "192.168.0.xxx";
uint16_t ha_port = 8123;
const char* ha_pwd = "HA_PASSWORD"; //long-lived password. On HA, Profile > Long-Lived Access Tokens > Create Token

ha.setHAServer(ha_ip, ha_port);
ha.setHAPassword(ha_pwd);
  
// Optional, but can use SHA1 fingerprint to confirm one is connecting to 
String fingerprint = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";
ha.setFingerPrint(fingerprint);
```
All the commands below do the same thing

**Home Assistant**

URL: "/api/services/light/turn_on" (HA -> Developer Tools -> Services -> Service)

Component: "light.bedroom_light" (HA -> Developer Tools -> Services -> Entity)

```
    // 1. Send custom DATA to HA
    ha.sendCustomHAData("/api/services/light/turn_on", "{\"entity_id\":\"light.bedroom_light\"}");
```
```
    // 2. Set component and send light to HA // Will set tmpURL to "/api/services/light/turn_on"
    ha.setComponent("light.bedroom_light");
    ha.sendHALight(true);
```
```  
    // 3. Set light on/off as bool and component is 2nd input, Send Light to HA 
    //        Will set tmpURL to "/api/services/light/turn_on"
    ha.sendHALight(true, "light.bedroom_light");
```
```  
    // 4. Set component and URL is 2nd input, Send custom URL to HA
    ha.setComponent("light.bedroom_light");
    ha.sendHAURL("/api/services/light/turn_on");
```
```  
    // 5. Set URL and Send component to HA
    ha.setURL("/api/services/light/turn_on");
    ha.sendHAComponent("light.bedroom_light");
```
```  
    // 6. Set URL 1st argument, component 2nd, send to HA
    ha.sendHAComponent("/api/services/light/turn_on", "light.bedroom_light");
```
```  
    // 7. Set URL , Set component, send to HA
    ha.setURL("/api/services/light/turn_on");
    ha.setComponent("light.bedroom_light");
    ha.sendHA();
```
```  
    // 8. Set URL and Send area to HA
    ha.setURL("/api/services/light/turn_on");
    ha.sendHAArea("bedroom");
```
