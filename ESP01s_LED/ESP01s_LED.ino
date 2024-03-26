#include <ESP8266WiFi.h>
#include <PubSubClient.h>
 
const char* ssid = "WIFI名称";               //设置Wi-Fi名
const char* password = "WIFI密码";           //设置Wi-Fi密码
const char* mqtt_server = "MQTT服务器IP"; //设置MQTT服务器地址
 
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int ledPin = 0;    //设置ESP8266的GPIO0号脚为输出控制脚
 
//连接wifi
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  //等待WiFi连接
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //wifi连接成功后输出成功信息
  Serial.println("");
  Serial.println("WiFi Connected!");   //显示wifi连接成功
  Serial.println(WiFi.localIP());       //返回wifi分配的IP
  Serial.println(WiFi.macAddress());  //返回设备的MAC地址
  Serial.println(""); 
  randomSeed(micros());
}
 
//回调函数
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived[");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // 通过接收的信息 '1'或'0'来控制开或关
  if ((char)payload[0] == '1') {
    digitalWrite(ledPin, HIGH);    //接收到'1'GPIO0输出高电平
    Serial.println("HIGH");
  } else {
    digitalWrite(ledPin, LOW);     //接收到'0'GPIO0输出低电平
    Serial.println("LOW");
  }
}
 
void reconnect() {
    String clientId = "ESP-01";            //设备名
    if (client.connect(clientId.c_str())) {         //尝试连接MQTT
      Serial.println("MQTT connect success.");   //显示MQTT连接成功
      client.subscribe("LED_Switch");          //设置主题
      client.publish("LED_Switch", "0");        //向主题发送消息
    } else {
      delay(5000);                         //等待5秒重试
    }
}
 
void setup() {
  pinMode(ledPin, OUTPUT);             //设置GPIO0为输出脚
  digitalWrite(ledPin, LOW);             //GPIO0初始输出低电平
  Serial.begin(115200);
  setup_wifi();                        //连接wifi
  client.setServer(mqtt_server, 1883);     //设置MQTT服务器
  client.setCallback(callback);            //设置回调函数
}
 
void loop() {
  if (!client.connected()) {
    reconnect();              //尝试连接MQTT服务器并接收信息
  }
  client.loop();
}