#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {
    #include "bmv080_defs.h"
}

namespace esphome {
namespace bmv080 {

class BMV080Sensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override{};
  void dump_config() override;

  static int8_t bmv080_i2c_read(bmv080_sercom_handle_t sercomHandle, uint16_t header, uint16_t* payload, uint16_t len);
  static int8_t bmv080_i2c_write(bmv080_sercom_handle_t sercomHandle, uint16_t header, const uint16_t* payload, uint16_t len);
  static int8_t bmv080_delay(uint32_t period_ms);

 protected:
  bmv080_handle_t sensorHandle = nullptr;
  TaskHandle_t sensor_task_handle_ = nullptr;
  
  static void sensor_task(void* parameter);
};

}
} 
