#include <WiFi.h>

IPAddress findNetwork(){

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  

  // Wait until open wifi found
  while(!(WiFi.status() == WL_CONNECTED)){
    int n = WiFi.scanNetworks();

    for (int i = 0; i < n; ++i) {

      if(WiFi.encryptionType(i) == WIFI_AUTH_OPEN && WiFi.SSID(i) != "Sony 75 Tv.v,"){
        WiFi.begin(WiFi.SSID(i).c_str());
      }

      /*
      esp_netif_t* netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
      if (netif) {
          esp_netif_dhcpc_stop(netif);
      }
      */
      break;

    }
    delay(1000);
  }

  return WiFi.localIP();
}