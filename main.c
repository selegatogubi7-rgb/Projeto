#include <stdio.h>
#include <Adafruit_Fingerprint.h> //aparenta ser a biblioteca para o sensor biometrico
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <WiFi.h>
#define WIFI_SSID //define a rede  
#define WIFI_SENHA //define a senha
#include <PubSubClient.h>
#define MQTT_BROKER "broker.com" //esse site desse ser substituido pelo site de broker que iremos usar
#define MQTT_PORTA  1883 //não sei se está certo mas tava nas aulas do Ruy
#define MQTT_USUARIO "usuario"
#define MQTT_SENHA   "senha"
#define MQTT_CLIENT_ID "chamada"

WiFiClient rede;
PubSubClient clienteMQTT(espClient);

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS3231 rtc;
HardwareSerial mySerial(2);
Adafruit_Fingerprint finger(&mySerial);

void iniciarsesorbiometrico () {
    finger.begin (57600); //aparentemente 57600 é o valor  padrão

    if finger.verifyPassword()) {
        serial.println("Sensor encontrado") //tudo será serial. pois ele faz a comunicação com o monitor serial 
    }   else {
        serial.println("Sensor não encontrado");
        serial.println("Veririfique o sensor");
        while(1); //trava a execução
    }
    finger.getParameters(); //Pede ao sensor que envie suas informações internas sem chamar a função primeiro, os dados ficam zerados.
}

void iniciarlcd () {
    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.createChar(3, heart);

    lcd.setCursor(0, 1);
    lcd.print(" Inicializando..");

    serial.println("LCD inicializado.");
    delay(1500); // Tempo para ler a mensagem inicial

}

void iniciaretc () {
     if (! rtc.begin()) {
    lcd.setCursor(0, 0);
    lcd.print("Erro no RTC!");
    while (1) delay(10); // Trava o código se o RTC não for encontrado
}
    
    if (!rtc.isrunning()) {
        Serial.println("RTC parado! Ajustando data/hora...");
        // Ajusta para a data/hora da compilacao
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}
    Serial.print("RTC OK - Data/Hora: ");  // imprime o texto inicial
    Serial.print(agora.day());             // imprime o número do dia    
    Serial.print("/");                     // imprime uma barra
    Serial.print(agora.month());           // imprime o mês              
    Serial.print("/");                     // imprime outra barra
    Serial.print(agora.year());            // imprime o ano
    Serial.print(" ");                     // imprime o espaço entre data e hora
    Serial.print(agora.hour());            // imprime a hora
    Serial.print(":");                     // imprime dois pontos
    Serial.print(agora.minute());          // imprime os minutos
    Serial.print(":");                     // imprime mais dois pontos
    Serial.println(agora.second());        // imprime os segundos e pula linha
}

void iniciarwifi  (){
    Serial.print("Conectando ao WiFi: ");
    Serial.println(WIFI_SSID);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Conectando WiFi");

    WiFi.begin(WIFI_SSID, WIFI_SENHA);

    int tentativas = 0;
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        tentativas++;

    if (tentativas > 15) { //timeout - 15 é um número bom?
            Serial.println("\nERRO: Falha ao conectar no WiFi!");
            lcd.setCursor(0, 1);
            lcd.print("WiFi: FALHOU! ");
            return;
        }
    }

    Serial.println("\nWiFi conectado!");

    lcd.setCursor(0, 1);
    lcd.print("WiFi: OK ");
    delay(1000);
}

void inicializarMQTT() {
    clienteMQTT.setServer(MQTT_BROKER, MQTT_PORTA);

    Serial.print("Conectando ao broker...");

     int tentativas = 0;
    while (!clienteMQTT.connected()) {
        if (clienteMQTT.connect(MQTT_CLIENT_ID, MQTT_USUARIO, MQTT_SENHA)) {
            Serial.println("MQTT conectado!");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("MQTT: OK        ");
            delay(1000);
        } else {
            Serial.print("Falhou, rc=");
            Serial.print(clienteMQTT.state());
            Serial.println(" Tentando novamente em 3s...");
            tentativas++;

            if (tentativas > 5) {
                Serial.println("ERRO: Nao foi possivel conectar ao MQTT.");
                lcd.setCursor(0, 1);
                lcd.print("MQTT: FALHOU!   ");
                return;
            }
            delay(3000);
        }
    }
}
