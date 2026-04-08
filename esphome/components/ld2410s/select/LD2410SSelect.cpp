#include "LD2410SSelect.h"

namespace esphome {
namespace ld2410s {

void LD2410SResponseSpeedSelect::control(const std::string &value) {
    this->publish_state(value);
    this->parent_->new_config.resp_speed = (value == "Normal") ? 5 : 10;
}

}  // namespace ld2410s
}  // namespace esphome
