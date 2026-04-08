#pragma once

#include "esphome/components/button/button.h"
#include "../LD2410S.h"

namespace esphome {
namespace ld2410s {

class LD2410SEnableConfigButton : public button::Button, public Parented<LD2410S> {
 protected:
    void press_action() override { this->parent_->set_config_mode(true); }
};

class LD2410SDisableConfigButton : public button::Button, public Parented<LD2410S> {
 protected:
    void press_action() override { this->parent_->set_config_mode(false); }
};

class LD2410SApplyConfigButton : public button::Button, public Parented<LD2410S> {
 protected:
    void press_action() override { this->parent_->apply_config(); }
};

class LD2410SAutoConfigThreshold : public button::Button, public Parented<LD2410S> {
 protected:
    void press_action() override { this->parent_->start_auto_threshold_update(); }
};

}  // namespace ld2410s
}  // namespace esphome
