#pragma once

#include "../LD2410S.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace ld2410s {

class LD2410SSensor : public LD2410SListener, public Component, sensor::Sensor {
 public:
    void set_distance_sensor(sensor::Sensor *s) { distance_sensor_ = s; }

    void on_distance(int distance_cm) override {
        if (distance_sensor_ != nullptr && (int)distance_sensor_->get_state() != distance_cm)
            distance_sensor_->publish_state((float)distance_cm);
    }

 private:
    sensor::Sensor *distance_sensor_{nullptr};
};

}  // namespace ld2410s
}  // namespace esphome
