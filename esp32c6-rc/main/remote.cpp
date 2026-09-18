#include "remote.hpp"
#include "bridge.h"

extern "C" {
    #include "esp_wifi.h"
    #include "esp_now.h"
    #include "esp_rom_crc.h"
}

int32_t RemoteControl::applyDeadzone(int32_t axis) {
    using namespace ConstantValues;
    if (axis < DEAD_ZONE && axis > -DEAD_ZONE) {
        return 0; 
    } else {
        return axis;
    }   
}

void RemoteControl::calculateCRC(DroneControlPacket* dronePacket) {
    using namespace ConstantValues;

    std::array<uint8_t, PACKET_DATA_SIZE> buffer;

    std::memcpy(buffer.data(), dronePacket, PACKET_DATA_SIZE);

    dronePacket->crcValue = esp_rom_crc32_le(N_CRC_CALC_VALUE, buffer.data(), PACKET_DATA_SIZE);
}

DroneControlPacket RemoteControl::getAndClearPacket() {
    DroneControlPacket packet;

    {
        using namespace ConstantValues;
        std::lock_guard scoped_lock(RemoteControl::m_packetMutex);
        packet = m_sharedPacket;
        m_sharedPacket.crcValue = 0;
    }

    return packet;
}

void RemoteControl::sendPacket(DroneControlPacket *packet) {
    using namespace ConstantValues;

    std::array<uint8_t, PACKET_SIZE> buffer;

    RemoteControl::calculateCRC(packet);
        
    std::memcpy(buffer.data(), packet, PACKET_SIZE);

    esp_now_send(RemoteControl::m_peer.peer_addr, buffer.data(), PACKET_SIZE);
}

void RemoteControl::initLed() {
    using namespace ConstantValues;

    gpio_reset_pin(GREEN_LED);
    gpio_set_direction(GREEN_LED, GPIO_MODE_OUTPUT);
    gpio_set_level(GREEN_LED, SET_LOW);
    gpio_set_pull_mode(GREEN_LED, GPIO_FLOATING);
}

void RemoteControl::initRemoteConnection() {
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));

    ESP_ERROR_CHECK(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B|WIFI_PROTOCOL_11G|WIFI_PROTOCOL_11N|WIFI_PROTOCOL_LR) );

    ESP_ERROR_CHECK(esp_now_init());

    std::memcpy(m_peer.peer_addr, ConstantValues::RECEIVER_MAC.data(), ConstantValues::MAC_LENGTH);
    m_peer.channel = 1;
    m_peer.encrypt = false;

    ESP_ERROR_CHECK(esp_now_add_peer(&RemoteControl::m_peer));
}

/*
    Functions triggered by bluepad callbacks
*/
void RemoteControl::handlePadData(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons, uint8_t dpad) {
    using namespace ButtonMasks;

    int32_t ay = applyDeadzone(axis_y);
    int32_t arx = applyDeadzone(axis_rx);
    int32_t ary = applyDeadzone(axis_ry);

    m_currentPacket.throttle = scaleAxisValue(ay);
    m_currentPacket.roll = scaleAxisValue(arx);
    m_currentPacket.pitch = scaleAxisValue(ary);
        
    if (buttons & BUTTON_SHOULDER_L) {
        m_currentPacket.buttonControlReg |= ControlRegister::YAW_LEFT;
    } else if ((dpad & BUTTON_SHOULDER_L) == 0) {
        m_currentPacket.buttonControlReg &= ~ControlRegister::YAW_LEFT;
    }

    if (buttons & BUTTON_SHOULDER_R) {
        m_currentPacket.buttonControlReg |= ControlRegister::YAW_RIGHT;
    } else if ((dpad & BUTTON_SHOULDER_R) == 0) {
        m_currentPacket.buttonControlReg &= ~ControlRegister::YAW_RIGHT;
    }

    if (buttons & BUTTON_A) {
        m_currentPacket.buttonControlReg |= ControlRegister::START_STOP_ENGINE;
    } else if ((dpad & BUTTON_A) == 0) {
        m_currentPacket.buttonControlReg &= ~ControlRegister::START_STOP_ENGINE;
    }

    if(dpad & DPAD_UP) {
        m_currentPacket.buttonControlReg |= ControlRegister::CAM_UP;
    } else if ((dpad & DPAD_UP) == 0) {
        m_currentPacket.buttonControlReg &= ~ControlRegister::CAM_UP;
    }

    if(dpad & DPAD_DOWN) {
        m_currentPacket.buttonControlReg |= ControlRegister::CAM_DOWN;
    } else if ((dpad & DPAD_DOWN) == 0) {
        m_currentPacket.buttonControlReg &= ~ControlRegister::CAM_DOWN;
    }

    {
        std::lock_guard scoped_lock(RemoteControl::m_packetMutex);
        m_sharedPacket = m_currentPacket;
    }
}

void RemoteControl::padDisconnected() {
    constexpr uint32_t DELAY_TILL_DISCONNECT = 40U;
    {
        std::lock_guard scoped_lock(RemoteControl::m_packetMutex);
        m_sharedPacket.buttonControlReg |= ControlRegister::NO_PAD;
    }
    vTaskDelay(pdMS_TO_TICKS(DELAY_TILL_DISCONNECT));
    xEventGroupClearBitsFromISR(indicatorEventGroupHandle, ConstantValues::xPadConnected);
}

void RemoteControl::padReady() {
    xEventGroupSetBits(indicatorEventGroupHandle, ConstantValues::xPadConnected);
    //clear in case of reconnect
    m_sharedPacket.buttonControlReg &= ~ControlRegister::NO_PAD;
}

/*
    callbacks for bluepad
*/
void call_remote_control(int32_t axis_y, int32_t axis_rx, int32_t axis_ry, uint32_t buttons, uint8_t dpad) {
    RemoteControl::handlePadData(axis_y, axis_rx, axis_ry, buttons, dpad);
}

void call_pad_disconnected() {
    RemoteControl::padDisconnected();
}

void call_pad_ready() {
    RemoteControl::padReady();
}
