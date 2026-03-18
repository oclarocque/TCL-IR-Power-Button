#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremote.h>

#define BAUDRATE 115200
#define MQQT_PORT 1883
#define WIFI_RETRY_DELAY_MS 5000
#define SUB_QOS_LVL 1
#define PIN_LED_IR 45
#define PIN_LED_STATUS RGB_BUILTIN
#define FREQ_CARRIER_KHZ 38

const char *ssid = "XXX";   // name of your WiFi network
const char *password = "XXX"; // password of the WiFi network

const char *ID = "Example_Light";   // Name of our device, must be unique
const char *TOPIC = "room/light";   // Topic to subcribe to
const char *STATE_TOPIC = "room/light/state";  // Topic to publish the light state to

/* Sequence in us of the TCL power on command to send */
static uint16_t tclPowerOnRawData[] = {
    /* Command */
    4039, 3948,  619, 1901, 552, 1958,  443, 2021, 619, 1878,  575,  959, 525, 998, 558, 1879, 576,
     922,  620, 1912,  584, 901,  597, 1902,  596, 941,  498,  961,  595, 881, 617, 881,  618, 880,
     601, 1896,  617, 1881, 618,  882,  612, 1885, 616,  882,  616, 1881, 616, 944, 497, 1939, 614,
    /* Gap */
    8511,
    /* Repeat */
    4039, 3948,  619, 1901, 552, 1958,  443, 2021, 619, 1878, 575,  959, 525, 998, 558, 1879, 576,
     922,  620, 1912, 584,  901,  597, 1902,  596, 941,  498, 961,  595, 881, 617, 881,  618, 880,
     601, 1896,  617, 1881, 618,  882,  612, 1885, 616,  882, 616, 1881, 616, 944, 497, 1939, 614
};

IPAddress broker(192, 168, 0, 2); // IP address of your MQTT broker eg. 192.168.1.50
WiFiClient wclient;

PubSubClient client(wclient); // Setup MQTT client

String response;
bool msg_received = false;

// Handle incomming messages from the broker
void incomming_msg_callback(char* topic, byte* payload, unsigned int length)
{
  msg_received = true;

  for (int i = 0; i < length; i++) {
    response += (char)payload[i];
  }

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(response);
}

// Connect to WiFi network
void setup_wifi(void)
{
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to network

  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

// Reconnect to client
void reconnect(void)
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID, "xxx", "xxx")) {
      client.subscribe(TOPIC, SUB_QOS_LVL);
      Serial.println("connected");
      Serial.print("Subcribed to: ");
      Serial.println(TOPIC);
      Serial.println('\n');
    } else {
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(WIFI_RETRY_DELAY_MS);
    }
  }
}

void tv_power_control_handle(void)
{
  IrSender.sendRaw(tclPowerOnRawData, sizeof(tclPowerOnRawData) / sizeof(uint16_t), FREQ_CARRIER_KHZ);

  if (response == "ON") { // Turn the light on
    //rgbLedWrite(PIN_LED_STATUS, 0, 0, RGB_BRIGHTNESS);  // Blue
    client.publish(STATE_TOPIC,"ON");
  } else if (response == "OFF") {  // Turn the light off
    //rgbLedWrite(PIN_LED_STATUS, 0, 0, 0);  // Blue
    client.publish(STATE_TOPIC,"OFF");
  }
}

void setup(void)
{
  Serial.begin(BAUDRATE); // Start serial communication at 115200 baud
  IrSender.begin(PIN_LED_IR);
  pinMode(PIN_LED_STATUS, OUTPUT); // Configure LIGHT_PIN as an output

  delay(100);

  setup_wifi(); // Connect to network
  client.setServer(broker, MQQT_PORT);
  client.setCallback(incomming_msg_callback);// Initialize the callback routine
}

void loop(void)
{
  if (!client.connected()) { // Reconnect if connection is lost
    reconnect();
  }
  client.loop();

  if (msg_received) {
    msg_received = false;
    tv_power_control_handle();
    response.clear();
  }
}
