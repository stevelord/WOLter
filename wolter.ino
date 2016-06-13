/*
 * WOLter: Wake On Lan waker 
 * 
 * Modify your constants with IP, MAC etc and delay, then compile
 * and run somewhere with a suitable power source to hand.
 * 
 * Needs Adafruit CC3000 library and a CC3000 board.
 * 
 */

#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <utility/socket.h>

#define ADAFRUIT_CC3000_IRQ 3
#define ADAFRUIT_CC3000_VBAT 5
#define ADAFRUIT_CC3000_CS 10

Adafruit_CC3000 cc3000 = Adafruit_CC3000(ADAFRUIT_CC3000_CS, ADAFRUIT_CC3000_IRQ, ADAFRUIT_CC3000_VBAT, SPI_CLOCK_DIVIDER); 
Adafruit_CC3000_Client client;
const unsigned long connectTimeout  = 15L * 1000L;
const unsigned long responseTimeout = 15L * 1000L;

#define WLAN_SSID       "XXXXXXXX"
#define WLAN_PASS       "XXXXXXXX"
// Cab be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY WLAN_SEC_WPA2



void wolMe() {

  byte buf[102] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte tmac[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }; // Target MAC address
  uint32_t ip = cc3000.IP2U32(192, 168, 0, 255); // CC3000 expects IPs in specific format, so I have to call helper
  unsigned long startTime, t = 0L;
  
  Serial.println(F("\r\nAttempting connection..."));
  startTime = millis();
  do {
    client = cc3000.connectUDP(ip, 9);
  } while((!client.connected()) &&
          ((millis() - startTime) < connectTimeout));

  if(client.connected()) {
    Serial.println("Building WoL packet...");

    memset(buf, 0, sizeof(buf));
    memset(buf, 255, 8);
    // Assemble and issue request packet
    for (int ix=6; ix<102; ix++)
      buf[ix]=tmac[ix%6];
    

    Serial.println("Issuing WOL request...");
    client.write(buf, sizeof(buf));
    Serial.println("Closing connection");
    delay(500);
    client.close();
    Serial.println("Request sent. Check target.");
  }
}

bool wifiStatus(){
  uint32_t ipAddress, netmask, gateway, dhcp, dns;
if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcp, &dns))
  {
    Serial.println("Unable to obtain network status.");
    return false;
  }
  else
  {
    Serial.print("\nIP: "); cc3000.printIPdotsRev(ipAddress);
    Serial.print("\nNetmask: "); cc3000.printIPdotsRev(netmask);
    Serial.print("\nGateway: "); cc3000.printIPdotsRev(gateway);
    Serial.print("\nDHCP: "); cc3000.printIPdotsRev(dhcp);
    Serial.print("\nDNS: "); cc3000.printIPdotsRev(dns);
    Serial.println();
    return true;
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("\nInitialising CC3000...");
  if (!cc3000.begin()){
    Serial.println("Couldn't initialise CC3000 board. Check you've connected everything up correctly.");
    while(1);
  }

  Serial.println("Blatting old connection profiles");
  if (!cc3000.deleteProfiles()){
    Serial.println("Something's wrong with profile deletion");
    while(1);
  }

  Serial.println("Attempting to connect to Wi-Fi network");
  if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println("Wifi Connection failed. Check your settings or antenna.");
    while(1);
  } else {
    Serial.println("Connected! Now trying to grab an IP address.");
  }

  while (!cc3000.checkDHCP()){
    delay(100);
  }  
  
  
}

void loop() {
  // put your main code here, to run repeatedly:
  wifiStatus();
  wolMe();
  delay(5000);

}
