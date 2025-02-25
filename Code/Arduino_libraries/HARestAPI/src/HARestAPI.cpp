#include "HARestAPI.h"

/* Ask user to supply appropiate Client 
HARestAPI::HARestAPI(void)	{
	this->wclient = &dwclient;
}

void HARestAPI::useSSL(void) {
	_ssl = true;
	this->wsclient = &dwsclient;
} 
*/

HARestAPI::HARestAPI(WiFiClient &client)
{
  this->wclient = &client;
}

HARestAPI::HARestAPI(WiFiClientSecure &client)
{
  _ssl = true;
  this->wsclient = &client;
}

void HARestAPI::setHAServer(String HAServer)
{
  _serverip = HAServer;
  if (_ssl)
    _port = 443;
  else
    _port = 8123;
}

void HARestAPI::setHAServer(String HAServer, uint16_t Port)
{
  _serverip = HAServer;
  _port = Port;
}

void HARestAPI::setHAServer(String HAServer, uint16_t Port, String Password)
{
  _serverip = HAServer;
  _port = Port;
  _password = Password;
  _passwordset = true;
}

void HARestAPI::setHAPassword(String Password)
{
  _password = Password;
  _passwordset = true;
}

void HARestAPI::setRGB(uint8_t Red, uint8_t Green, uint8_t Blue)
{
  _red = Red;
  _green = Green;
  _blue = Blue;
  _rgbset = true;
}

void HARestAPI::setDebugMode(bool Debug)
{
  _debug = Debug;
}

void HARestAPI::setTimeOut(uint16_t TimeOut)
{
  _time_out = TimeOut;
}

void HARestAPI::setComponent(String Component)
{
  _component = Component;
}

void HARestAPI::setURL(String URL)
{
  _url = URL;
}

void HARestAPI::setFingerPrint(String FingerPrint)
{
  _fingerprint = FingerPrint;
}

String HARestAPI::sendGetHA(String URL)
{
  String posturl, replystr;
  this->http = new HTTPClient();

  if (_ssl)
  {
    posturl = "https://" + _serverip + ":" + _port + URL;
    if (_debug)
    {
      Serial.print("Connecting: ");
      Serial.println(posturl);
    }
    if ( _fingerprint.length() > 0)
    {
      #ifdef ESP8266
      wsclient->setFingerprint(_fingerprint.c_str());
      #elseif defined(ESP32)
      if (wsclient->verify(_fingerprint.c_str(), _serverip.c_str()))
      {
        Serial.println("certificate matches");
      }
      else
      {
        Serial.println("certificate doesn't match");
        _skip_sendurl = true;
      }
      #endif
    }
    else
    {
      wsclient->setInsecure();
    }
    if (!_skip_sendurl)
    {
      http->begin(*wsclient, posturl);
      if (_time_out)
        http->setTimeout(_time_out);
      http->addHeader("User-Agent", "HA Rest API Client");
      http->addHeader("Accept", "*/*");
      http->addHeader("Authorization", "Bearer " + _password);
      int httpCode = http->GET();
      if (httpCode > 0)
      {
        if (_debug)
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          replystr += http->getString();
        }
      }
      else
      {
        if (_debug)
          Serial.printf("[HTTPS] GET... failed, error: %s\n", http->errorToString(httpCode).c_str());
      }
      http->end();
    }
    _skip_sendurl = false;
  }
  else
  {
    posturl = "http://" + _serverip + ":" + _port + URL;
    if (_debug)
    {
      Serial.print("Connecting: ");
      Serial.println(posturl);
    }
    http->begin(*wclient, posturl);
    if (_time_out)
      http->setTimeout(_time_out);
    http->addHeader("User-Agent", "HA Rest API Client");
    http->addHeader("Accept", "*/*");
    http->addHeader("Authorization", "Bearer " + _password);
    int httpCode = http->GET();
    if (httpCode > 0)
    {
      if (_debug)
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        replystr += http->getString();
      }
    }
    else
    {
      if (_debug)
        Serial.printf("[HTTP] GET... failed, error: %s\n", http->errorToString(httpCode).c_str());
    }
    http->end();
  }

  if (_debug)
  {
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(replystr);
    Serial.println("==========");
    Serial.println("closing connection");
  }
  delete http;
  return replystr;
}

bool HARestAPI::sendPostHA(String URL, String message)
{
  bool reply;
  String posturl, replystr;
  this->http = new HTTPClient();

  if (_ssl)
  {
    posturl = "https://" + _serverip + ":" + _port + URL;
    if (_debug)
    {
      Serial.print("Connecting: ");
      Serial.println(posturl);
    }
    if ( _fingerprint.length() > 0 )
    {
      #ifdef ESP8266
      wsclient->setFingerprint(_fingerprint.c_str());
      #elseif defined(ESP32)
      if (wsclient->verify(_fingerprint.c_str(), _serverip.c_str()))
      {
        Serial.println("certificate matches");
      }
      else
      {
        Serial.println("certificate doesn't match");
        _skip_sendurl = true;
      }
      #endif
    }
    else
    {
      wsclient->setInsecure();
    }
    if (!_skip_sendurl)
    {
      http->begin(*wsclient, posturl);
      if (_time_out)
        http->setTimeout(_time_out);
      http->addHeader("User-Agent", "HA Rest API Client");
      http->addHeader("Accept", "*/*");
      http->addHeader("Authorization", "Bearer " + _password);
      int httpCode = http->POST(message);
      if (httpCode > 0)
      {
        if (_debug)
          Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
        {
          replystr += http->getString();
          reply = true;
        }
      }
      else
      {
        if (_debug)
          Serial.printf("[HTTPS] POST... failed, error: %s\n", http->errorToString(httpCode).c_str());
        reply = false;
      }
      http->end();
    }
    _skip_sendurl = false;
  }
  else
  {
    posturl = "http://" + _serverip + ":" + _port + URL;
    if (_debug)
    {
      Serial.print("Connecting: ");
      Serial.println(posturl);
    }
    http->begin(*wclient, posturl);
    if (_time_out)
      http->setTimeout(_time_out);
    http->addHeader("User-Agent", "HA Rest API Client");
    http->addHeader("Accept", "*/*");
    http->addHeader("Authorization", "Bearer " + _password);
    int httpCode = http->POST(message);
    if (httpCode > 0)
    {
      if (_debug)
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY)
      {
        replystr += http->getString();
        reply = true;
      }
    }
    else
    {
      if (_debug)
        Serial.printf("[HTTP] POST... failed, error: %s\n", http->errorToString(httpCode).c_str());
      reply = false;
    }
    http->end();
  }
  if (_debug)
  {
    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(replystr);
    Serial.println("==========");
    Serial.println("closing connection");
  }
  delete http;
  return reply;
}

bool HARestAPI::sendPostHA(String message)
{
  return sendPostHA(_url, message);
}

String HARestAPI::sendGetHA(void)
{
  return sendGetHA(_url);
}

bool HARestAPI::sendCustomHAData(String URL, String Message)
{
  return sendPostHA(URL, Message);
}

bool HARestAPI::sendHA(void)
{
  String Message = "{\"entity_id\":\"" + _component + "\"}";
  return sendPostHA(_url, Message);
}

bool HARestAPI::sendHAComponent(String Component)
{
  String Message = "{\"entity_id\":\"" + Component + "\"}";
  return sendPostHA(_url, Message);
}

bool HARestAPI::sendHAComponent(String URL, String Component)
{
  String Message = "{\"entity_id\":\"" + Component + "\"}";
  return sendPostHA(URL, Message);
}

bool HARestAPI::sendHAArea(String Component)
{
  String Message = "{\"area_id\":\"" + Component + "\"}";
  return sendPostHA(_url, Message);
}

bool HARestAPI::sendHAArea(String URL , String Component)
{
  String Message = "{\"area_id\":\"" + Component + "\"}";
  return sendPostHA(URL, Message);
}

bool HARestAPI::sendHAURL(String URL)
{
  String Message = "{\"entity_id\":\"" + _component + "\"}";
  return sendPostHA(URL, Message);
}

bool HARestAPI::sendHALight(bool LightStatus)
{
  String Message = "{\"entity_id\":\"" + _component + "\"}";
  if (LightStatus)
  {
    return sendPostHA("/api/services/light/turn_on", Message);
  }
  else
  {
    return sendPostHA("/api/services/light/turn_off", Message);
  }
}

bool HARestAPI::sendHALight(bool LightStatus, String Component)
{
  String Message = "{\"entity_id\":\"" + Component + "\"}";
  if (LightStatus)
  {
    return sendPostHA("/api/services/light/turn_on", Message);
  }
  else
  {
    return sendPostHA("/api/services/light/turn_off", Message);
  }
}

bool HARestAPI::sendHASwitch(bool LightStatus)
{
  String Message = "{\"entity_id\":\"" + _component + "\"}";
  if (LightStatus)
  {
    return sendPostHA("/api/services/switch/turn_on", Message);
  }
  else
  {
    return sendPostHA("/api/services/switch/turn_off", Message);
  }
}

bool HARestAPI::sendHASwitch(bool LightStatus, String Component)
{
  String Message = "{\"entity_id\":\"" + Component + "\"}";
  if (LightStatus)
  {
    return sendPostHA("/api/services/switch/turn_on", Message);
  }
  else
  {
    return sendPostHA("/api/services/switch/turn_off", Message);
  }
}

bool HARestAPI::sendHARGBLight(void)
{
  String Message = "{\"entity_id\":\"" + _component + "\"," +
                   "\"rgb_color\":[" + String(_red) + "," + String(_green) + "," + String(_blue) + "]}";
  return sendPostHA("/api/services/light/turn_on", Message);
}

bool HARestAPI::sendHARGBLight(String Component)
{
  String Message = "{\"entity_id\":\"" + Component + "\"," +
                   "\"rgb_color\":[" + String(_red) + "," + String(_green) + "," + String(_blue) + "]}";
  return sendPostHA("/api/services/light/turn_on", Message);
}

bool HARestAPI::sendHARGBLight(uint8_t Red, uint8_t Green, uint8_t Blue)
{
  String Message = "{\"entity_id\":\"" + _component + "\"," +
                   "\"rgb_color\":[" + String(Red) + "," + String(Green) + "," + String(Blue) + "]}";
  return sendPostHA("/api/services/light/turn_on", Message);
}

bool HARestAPI::sendHARGBLight(String Component, uint8_t Red, uint8_t Green, uint8_t Blue)
{
  String Message = "{\"entity_id\":\"" + Component + "\"," +
                   "\"rgb_color\":[" + String(Red) + "," + String(Green) + "," + String(Blue) + "]}";
  return sendPostHA("/api/services/light/turn_on", Message);
}

bool HARestAPI::sendHAAutomation(bool AutoStatus)
{
  String Message = "{\"entity_id\":\"" + _component + "\"}";
  if (AutoStatus)
  {
    return sendPostHA("/api/services/automation/turn_on", Message);
  }
  else
  {
    return sendPostHA("/api/services/automation/turn_off", Message);
  }
}

bool HARestAPI::sendHAAutomation(bool AutoStatus, String Component)
{
  String Message = "{\"entity_id\":\"" + Component + "\"}";
  if (AutoStatus)
  {
    return sendPostHA("/api/services/automation/turn_on", Message);
  }
  else
  {
    return sendPostHA("/api/services/automation/turn_off", Message);
  }
}
