#pragma once

#include "../LD2410S.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace ld2410s {

class LD2410SMaxDistanceNumber : public number::Number, public Parented<LD2410S> {
 protected:
    void control(float value) override {
        this->publish_state(value);
        this->parent_->new_config.max_dist = (uint32_t)value;
    }
};

class LD2410SMinDistanceNumber : public number::Number, public Parented<LD2410S> {
 protected:
    void control(float value) override {
        this->publish_state(value);
        this->parent_->new_config.min_dist = (uint32_t)value;
    }
};

class LD2410SNoDelayNumber : public number::Number, public Parented<LD2410S> {
 protected:
    void control(float value) override {
        this->publish_state(value);
        this->parent_->new_config.delay = (uint32_t)value;
    }
};

class LD2410SStatusReportingFreqNumber : public number::Number, public Parented<LD2410S> {
 protected:
    void control(float value) override {
        this->publish_state(value);
        this->parent_->new_config.status_freq = (uint32_t)(value * 10);
    }
};

class LD2410SDistReportingFreqNumber : public number::Number, public Parented<LD2410S> {
 protected:
    void control(float value) override {
        this->publish_state(value);
        this->parent_->new_config.dist_freq = (uint32_t)(value * 10);
    }
};

}  // namespace ld2410s
}  // namespace esphome
