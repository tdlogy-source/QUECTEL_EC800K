#include <PPP.h>
#define PPP_MODEM_APN "v-internet"
#define PPP_MODEM_RST       25  
#define PPP_MODEM_RST_LOW   false  
#define PPP_MODEM_TX        17  
#define PPP_MODEM_RX        16  
#define PPP_MODEM_RTS       -1 
#define PPP_MODEM_CTS       -1  
#define PPP_MODEM_FC        ESP_MODEM_FLOW_CONTROL_NONE  
#define PPP_MODEM_MODEL     PPP_MODEM_GENERIC 

void onEvent(arduino_event_id_t event, arduino_event_info_t info) {
  switch (event) {
    case ARDUINO_EVENT_PPP_START:        Serial.println("PPP Started"); break;
    case ARDUINO_EVENT_PPP_CONNECTED:    Serial.println("PPP Connected"); break;
    case ARDUINO_EVENT_PPP_GOT_IP:       Serial.println("PPP Got IP"); break;
    case ARDUINO_EVENT_PPP_LOST_IP:      Serial.println("PPP Lost IP"); break;
    case ARDUINO_EVENT_PPP_DISCONNECTED: Serial.println("PPP Disconnected"); break;
    case ARDUINO_EVENT_PPP_STOP:         Serial.println("PPP Stopped"); break;
    default:                             break;
  }
}

void testClient(const char *host, uint16_t port) {
  NetworkClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection Failed");
    return;
  }
  client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
  while (client.connected() && !client.available());
  while (client.available()) {
    client.read();  //Serial.write(client.read());
  }

  Serial.println("Connection Success");
  client.stop();
}

void setup() {
  Serial.begin(115200); // Thiết lập baud rate là 115200
  Serial.println(PPP.cmd("AT+CGATT?", 2000));  
  Serial.println(PPP.cmd("AT+SAPBR=1,1", 2000));  
  Serial.println(PPP.cmd("AT+SAPBR=2,1", 2000)); 
  PPP.cmd("AT+CIICR", 2000);  
  Serial.print("IP Address: ");
  Serial.println(PPP.cmd("AT+CIFSR", 2000));  

  Network.onEvent(onEvent);

  PPP.setApn(PPP_MODEM_APN);
  PPP.setResetPin(PPP_MODEM_RST, PPP_MODEM_RST_LOW);
  PPP.setPins(PPP_MODEM_TX, PPP_MODEM_RX, PPP_MODEM_RTS, PPP_MODEM_CTS, PPP_MODEM_FC);

  Serial.println("Starting the modem. It might take a while!");
  PPP.begin(PPP_MODEM_MODEL);

  Serial.print("Model: ");
  Serial.println(PPP.moduleName());
  Serial.print("IMEI: ");
  Serial.println(PPP.IMEI());

  bool attached = PPP.attached();
  if (!attached) {
    int i = 0;
    unsigned int s = millis();
    Serial.print("Waiting to connect to network");
    while (!attached && ((++i) < 600)) {
      Serial.print(".");
      delay(100);
      attached = PPP.attached();
    }
    Serial.print((millis() - s) / 1000.0, 1);
    Serial.println("s");
    attached = PPP.attached();
  }

  Serial.print("Attached: ");
  Serial.println(attached);
  Serial.print("State: ");
  Serial.println(PPP.radioState());
  if (attached) {
    Serial.print("Operator: ");
    Serial.println(PPP.operatorName());
    Serial.print("IMSI: ");
    Serial.println(PPP.IMSI());
    Serial.print("RSSI: ");
    Serial.println(PPP.RSSI());
    int ber = PPP.BER();
    if (ber > 0) {
      Serial.print("BER: ");
      Serial.println(ber);
      Serial.print("NetMode: ");
      Serial.println(PPP.networkMode());
    }

    Serial.println("Switching to data mode...");
    PPP.mode(ESP_MODEM_MODE_DATA);  
    if (!PPP.waitStatusBits(ESP_NETIF_CONNECTED_BIT, 1000)) {
      Serial.println("Failed to connect to internet!");
    } else {
      Serial.println("Connected to internet!");
    }
  } else {
    Serial.println("Failed to connect to network!");
  }
}

void loop() {
  if (PPP.connected()) {
    testClient("google.com", 80);
  }
  delay(20000);
}