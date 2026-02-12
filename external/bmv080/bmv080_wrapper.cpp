#include "esphome/core/log.h"
#include "bmv080_wrapper.h"

namespace esphome {
namespace bmv080 {

static const char *TAG = "empty_i2c_sensor.sensor";

void BMV080Sensor::setup(){
    ESP_LOGCONFIG(TAG, "Starte Empty I2C Sensor an Adresse 0x%02X...", this->address_);
  
  // Teste die Verbindung
  i2c::ErrorCode err = this->write(nullptr, 0);
  if (err == i2c::ERROR_OK) {
    ESP_LOGI(TAG, "Sensor gefunden und antwortet!");
  } else {
    ESP_LOGE(TAG, "Sensor nicht gefunden! Fehlercode: %d", (int)err);
    this->mark_failed(); 
  }
}

void BMV080Sensor::update(){

}

void BMV080Sensor::dump_config(){
    LOG_SENSOR("", "Empty I2C Sensor", this);
    LOG_I2C_DEVICE(this);
}

}  // namespace EmptyI2CSensor
}  // namespace esphome