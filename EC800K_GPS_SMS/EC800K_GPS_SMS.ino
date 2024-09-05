#include <HardwareSerial.h>

#define simSerial               Serial2
#define MCU_SIM_BAUDRATE        115200
#define MCU_SIM_TX_PIN          17
#define MCU_SIM_RX_PIN          16
#define MCU_SIM_EN_PIN          15

#define PHONE_NUMBER            "Nhap so dien thoai" 

void sim_at_wait() {
    delay(100);
    while (simSerial.available()) {
        Serial.write(simSerial.read());
    }
}

bool sim_at_cmd(String cmd) {
    Serial.print("Sending command: ");
    Serial.println(cmd);
    simSerial.println(cmd);
    delay(500); // 
    sim_at_wait();
    return true;
}

bool sim_at_send(char c) {
    simSerial.write(c);
    return true;
}

void sent_sms() {
    sim_at_cmd("AT+CMGF=1"); // 
    String temp = "AT+CMGS=\"";
    temp += PHONE_NUMBER;
    temp += "\"";
    sim_at_cmd(temp);
    sim_at_cmd("Test SMS from Quectel EC800K");
    sim_at_send(0x1A); // 
}



void get_gps_data() {
    // Bật GPS
    sim_at_cmd("AT+QGPS=1"); 
    delay(10000); 


    sim_at_cmd("AT+QGPS?");

    sim_at_cmd("AT+QGPSLOC=0"); // Nhận thông tin GPS
}


void setup() {
    pinMode(MCU_SIM_EN_PIN, OUTPUT); 
    digitalWrite(MCU_SIM_EN_PIN, LOW);
    delay(20);
    
    Serial.begin(115200);
    Serial.println("\n\n\n\n-----------------------\nSystem started!!!!");

    delay(8000);
    simSerial.begin(MCU_SIM_BAUDRATE, SERIAL_8N1, MCU_SIM_RX_PIN, MCU_SIM_TX_PIN);

    
    Serial.println("Checking AT command...");
    sim_at_cmd("AT");

    
    Serial.println("Getting product info...");
    sim_at_cmd("ATI");

  
    Serial.println("Checking SIM status...");
    sim_at_cmd("AT+CPIN?");

 
    Serial.println("Checking signal quality...");
    sim_at_cmd("AT+CSQ");


    Serial.println("Getting IMSI...");
    sim_at_cmd("AT+CIMI");

    pinMode(2, OUTPUT); 
    digitalWrite(2, HIGH);

    sent_sms();
    delay(5000);   

  
    get_gps_data();
  
}

void loop() {     
    if (Serial.available()) {
        char c = Serial.read();
        simSerial.write(c);
    }
    sim_at_wait();
}