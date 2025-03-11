#include "joybridge_receiver.h"

// For convenience, bring the namespace into scope.
using namespace JoyBridge;

// Static instance pointer (we assume a singleton JoyBridgeReceiver)
JoyBridgeReceiver *JoyBridgeReceiver::_instance = nullptr;

// Define a file‑scope master MAC storage and timeout constant.
static uint8_t masterMAC[6] = {0};
static const unsigned long MASTER_TIMEOUT = 1000;  // 1000 ms timeout

// ---------------------------------------------------------------------------
// JoyBridgeReceiver Class Implementation
// ---------------------------------------------------------------------------

// Constructor: initialize member variables.
JoyBridgeReceiver::JoyBridgeReceiver()
    : _connected(false), _lastReceivedTime(0), _dataCallback(nullptr)
{
  // Zero out latest joystick data.
  memset(&_latestData, 0, sizeof(_latestData));
}

// begin() initializes WiFi and ESP‑NOW and registers our callback.
bool JoyBridgeReceiver::begin(String service_name)
{
  // Confirm that the service name is not too long, 16 characters max.
  if (service_name.length() > 16)
  {
    Serial.println("Service name too long (max 16 characters)");
    return false;
  }

  strncpy(_service_name, service_name.c_str(), 16);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK)
  {
    Serial.println("ESP-NOW init failed");
    return false;
  }

  // Set our static instance pointer for the callback forwarding.
  _instance = this;

  // Register the static callback.
  esp_now_register_recv_cb(JoyBridgeReceiver::onDataRecv);

  // Initialize the connection status and timestamp.
  _lastReceivedTime = millis();
  _connected = false;

  return true;
}

// loop() should be called periodically from the main loop.
void JoyBridgeReceiver::loop()
{
  // Check for communication dropouts.
  checkTimeout();
}

// getJoystickData() returns the most recent joystick data.
JoystickData JoyBridgeReceiver::getJoystickData() const { return _latestData; }

// isConnected() returns true if a master (service) is registered.
bool JoyBridgeReceiver::isConnected() const { return _connected; }

// setDataCallback() registers a user callback to be invoked when new
// joystick data is received.
void JoyBridgeReceiver::setDataCallback(DataCallback callback)
{
  _dataCallback = callback;
}

// Static callback: forwards the ESP-NOW event to our instance.
void JoyBridgeReceiver::onDataRecv(const esp_now_recv_info_t *info,
                                   const uint8_t *incomingData, int len)
{
  if (_instance != nullptr)
  {
    _instance->handleDataRecv(info, incomingData, len);
  }
}

// handleDataRecv() processes incoming ESP-NOW data.
void JoyBridgeReceiver::handleDataRecv(const esp_now_recv_info_t *info,
                                       const uint8_t *incomingData, int len)
{
  // First, check if the message is a ServiceAnnouncement.
  if (len == sizeof(ServiceAnnouncement))
  {
    ServiceAnnouncement announcement;
    memcpy(&announcement, incomingData, sizeof(announcement));
    Serial.printf(
        "Discovered joybridge: %s from MAC: "
        "%02X:%02X:%02X:%02X:%02X:%02X\n",
        announcement.service_name, info->src_addr[0], info->src_addr[1],
        info->src_addr[2], info->src_addr[3], info->src_addr[4],
        info->src_addr[5]);

    // If we aren't already connected, and the service name matches ours, add
    // the master as a peer and send a registration confirmation.
    if (!_connected &&
        strncmp(announcement.service_name, _service_name, 16) == 0)
    {
      esp_now_peer_info_t peerInfo = {};
      memcpy(peerInfo.peer_addr, info->src_addr, 6);
      peerInfo.channel = 0;
      peerInfo.encrypt = false;
      if (esp_now_add_peer(&peerInfo) == ESP_OK)
      {
        Serial.println("Added joybridge!");
        // Save master's MAC address.
        memcpy(masterMAC, info->src_addr, 6);
        // Send registration confirmation.
        ClientRegistration regMsg = {"REGISTER"};
        esp_now_send(info->src_addr, (uint8_t *)&regMsg, sizeof(regMsg));
        _connected = true;
      }
      else
      {
        Serial.println("Failed to add peer");
      }
    }
  }
  // Else, if message length equals that of JoystickData, update our data.
  else if (len == sizeof(JoystickData))
  {
    _lastReceivedTime = millis();
    memcpy(&_latestData, incomingData, sizeof(JoystickData));

    // Serial.println("Joystick Data Received:");
    // Serial.printf(
    //     "A:%d B:%d X:%d Y:%d LB:%d RB:%d DU:%d DD:%d DL:%d DR:%d START:%d "
    //     "BACK:%d\n",
    //     _latestData.a, _latestData.b, _latestData.x, _latestData.y,
    //     _latestData.lb, _latestData.rb, _latestData.dpad_up,
    //     _latestData.dpad_down, _latestData.dpad_left, _latestData.dpad_right,
    //     _latestData.start, _latestData.back);
    // Serial.printf("LT:%.2f RT:%.2f LX:%.2f LY:%.2f RX:%.2f RY:%.2f\n",
    //               _latestData.lt, _latestData.rt, _latestData.left_x,
    //               _latestData.left_y, _latestData.right_x,
    //               _latestData.right_y);

    // If a user callback is registered, call it.
    if (_dataCallback)
    {
      _dataCallback(_latestData);
    }
  }
  else
  {
    Serial.printf("Unknown data received (len=%d)\n", len);
  }
}

// checkTimeout() checks if no ESP-NOW message has been received
// within MASTER_TIMEOUT. If so, remove the master peer and mark disconnected.
void JoyBridgeReceiver::checkTimeout()
{
  if (_connected && ((millis() - _lastReceivedTime) > MASTER_TIMEOUT))
  {
    Serial.println(
        "No message received from joybridge! for timeout period. Removing "
        "and "
        "reinitializing...");
    if (esp_now_del_peer(masterMAC) == ESP_OK)
    {
      Serial.println("Peer removed successfully.");
    }
    else
    {
      Serial.println("Failed to remove peer.");
    }
    _connected = false;
    memset(masterMAC, 0, sizeof(masterMAC));
  }
}
