#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

extern "C" {
    #include "bmv080.h"
    #include "bmv080_defs.h"
}

namespace esphome {
namespace bmv080 {

class BMV080Sensor : public sensor::Sensor, public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void update() override;
  void dump_config() override;
};

}  // namespace EmptyI2CSensor
}  // namespace esphome