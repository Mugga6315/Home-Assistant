#pragma once

#include "../LD2410S.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace ld2410s {

class LD2410SBinarySensor : public LD2410SListener, public Component, binary_sensor::BinarySensor {
 public:
    void set_presence_sensor(binary_sensor::BinarySensor *s) { presence_sensor_ = s; }

    void on_presence(bool presence) override {
        if (presence_sensor_ != nullptr && presence_sensor_->state != presence)
            presence_sensor_->publish_state(presence);
    }

 private:
    binary_sensor::BinarySensor *presence_sensor_{nullptr};
};

}  // namespace ld2410s
}  // namespace esphome
