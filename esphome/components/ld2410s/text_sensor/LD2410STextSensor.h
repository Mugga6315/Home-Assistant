#pragma once

#include "../LD2410S.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace ld2410s {

class LD2410STextSensor : public LD2410SListener, public Component, text_sensor::TextSensor {
 public:
    void set_fw_version_sensor(text_sensor::TextSensor *s) { fw_version_sensor_ = s; }

    void on_fw_version(std::string &version) override {
        if (fw_version_sensor_ != nullptr)
            fw_version_sensor_->publish_state(version);
    }

 private:
    text_sensor::TextSensor *fw_version_sensor_{nullptr};
};

}  // namespace ld2410s
}  // namespace esphome
