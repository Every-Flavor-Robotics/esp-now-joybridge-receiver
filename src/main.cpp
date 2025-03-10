#include <WiFi.h>
#include <esp_now.h>

struct JoystickData
{
  bool a;
  bool b;
  bool x;
  bool y;
  bool lb;
  bool rb;
  bool dpad_up;
  bool dpad_down;
  bool dpad_left;
  bool dpad_right;
  bool start;
  bool back;
  float lt;
  float rt;
  float left_x;
  float left_y;
  float right_x;
  float right_y;
};

// Struct for service discovery
struct ServiceAnnouncement
{
  char service_name[16];
};

// Struct for client registration response
struct ClientRegistration
{
  char message[16];  // "REGISTER"
};

JoystickData joystick;

JoystickData receivedData;

// Callback executed when data is received
// void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData,
//                 int len)
// {
//   memcpy(&receivedData, incomingData, sizeof(receivedData));

//   //   Print all joystick data, 1 line
//   Serial.printf(
//       "A: %d, B: %d, X: %d, Y: %d, LB: %d, RB: %d, Dpad Up: %d, Dpad Down:
//       %d, " "Dpad Left: %d, Dpad Right: %d, Start: %d, Back: %d, LT: %f, RT:
//       %f, " "Left X: %f, Left Y: %f, Right X: %f, Right Y: %f\n",
//       receivedData.a, receivedData.b, receivedData.x, receivedData.y,
//       receivedData.lb, receivedData.rb, receivedData.dpad_up,
//       receivedData.dpad_down, receivedData.dpad_left,
//       receivedData.dpad_right, receivedData.start, receivedData.back,
//       receivedData.lt, receivedData.rt, receivedData.left_x,
//       receivedData.left_y, receivedData.right_x, receivedData.right_y);
// }

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData,
                int len)
{
  if (len == sizeof(ServiceAnnouncement))
  {
    // Handle Service Announcement
    ServiceAnnouncement announcement;
    memcpy(&announcement, incomingData, sizeof(announcement));

    Serial.printf(
        "Discovered Service: %s from MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        announcement.service_name, info->src_addr[0], info->src_addr[1],
        info->src_addr[2], info->src_addr[3], info->src_addr[4],
        info->src_addr[5]);

    // Register host as peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, info->src_addr, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) == ESP_OK)
    {
      Serial.println("Added discovered device as peer");

      // Send registration confirmation to host
      ClientRegistration regMsg = {"REGISTER"};
      esp_now_send(info->src_addr, (uint8_t *)&regMsg, sizeof(regMsg));
      Serial.println("Sent registration to host");
    }
    else
    {
      Serial.println("Failed to add peer");
    }
  }
  else if (len == sizeof(JoystickData))
  {
    // Handle Joystick Data
    memcpy(&joystick, incomingData, sizeof(JoystickData));

    Serial.printf("Joystick Data Received:\n");
    Serial.printf(
        "A:%d B:%d X:%d Y:%d LB:%d RB:%d DU:%d DD:%d DL:%d DR:%d START:%d "
        "BACK:%d\n",
        joystick.a, joystick.b, joystick.x, joystick.y, joystick.lb,
        joystick.rb, joystick.dpad_up, joystick.dpad_down, joystick.dpad_left,
        joystick.dpad_right, joystick.start, joystick.back);
    Serial.printf("LT:%.2f RT:%.2f LX:%.2f LY:%.2f RX:%.2f RY:%.2f\n",
                  joystick.lt, joystick.rt, joystick.left_x, joystick.left_y,
                  joystick.right_x, joystick.right_y);

    // Handle joystick data (e.g., update motor or LED states here)
  }
  else
  {
    Serial.printf("Unknown data received (len=%d)\n", len);
  }
}

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop()
{  // Clients just listen and register when they detect a broadcast
  delay(1000);
}
