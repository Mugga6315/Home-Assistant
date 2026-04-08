#include "esphome/core/log.h"
#include "LD2410S.h"

namespace esphome {
namespace ld2410s {

static const char *TAG = "ld2410s";

// ── setup / loop ──────────────────────────────────────────────────

void LD2410S::setup() {
    this->set_config_mode(true);
    CmdFrameT read_fw_cmd = this->prepare_read_fw_cmd();
    this->send_command(read_fw_cmd);
    CmdFrameT read_config_cmd = this->prepare_read_config_cmd();
    this->send_command(read_config_cmd);
    this->set_config_mode(false);
}

void LD2410S::loop() {
    if (!this->cmd_active_) {
        static uint8_t buffer[64];
        static size_t pos = 0;
        while (available()) {
            PackageType type = this->read_line(read(), buffer, pos++);
            if (type == PackageType::SHORT_DATA || type == PackageType::THRESHOLD) {
                this->process_data_package(type, buffer, pos);
                pos = 0;
            }
        }
    }
}

// ── Config mode ───────────────────────────────────────────────────

void LD2410S::set_config_mode(bool enabled) {
    CmdFrameT cmd;
    cmd.header = CMD_FRAME_HEADER;
    cmd.command = enabled ? START_CONFIG_MODE_CMD : END_CONFIG_MODE_CMD;
    cmd.data_length = 0;
    if (enabled) {
        memcpy(&cmd.data[0], &START_CONFIG_MODE_VALUE, sizeof(START_CONFIG_MODE_VALUE));
        cmd.data_length += sizeof(START_CONFIG_MODE_VALUE);
    }
    cmd.footer = CMD_FRAME_FOOTER;
    this->send_command(cmd);
}

// ── Apply config ──────────────────────────────────────────────────

void LD2410S::apply_config() {
    this->status_set_warning("Sending command to sensor");
    this->set_config_mode(true);
    CmdFrameT apply_cmd = this->prepare_apply_config_cmd();
    this->send_command(apply_cmd);
    this->set_config_mode(false);
    this->status_clear_warning();
}

// ── Auto threshold ────────────────────────────────────────────────

void LD2410S::start_auto_threshold_update() {
    this->status_set_warning("Sending command to sensor");
    this->set_config_mode(true);
    CmdFrameT threshold_cmd = this->prepare_threshold_cmd();
    this->send_command(threshold_cmd);
    this->set_config_mode(false);
    this->status_clear_warning();
}

// ── Command frame builders ────────────────────────────────────────

CmdFrameT LD2410S::prepare_read_config_cmd() {
    CmdFrameT cmd;
    cmd.header = CMD_FRAME_HEADER;
    cmd.command = READ_PARAMS_CMD;
    cmd.data_length = 0;

    memcpy(&cmd.data[cmd.data_length], &CFG_MAX_DETECTION_VALUE,  sizeof(CFG_MAX_DETECTION_VALUE));  cmd.data_length += sizeof(CFG_MAX_DETECTION_VALUE);
    memcpy(&cmd.data[cmd.data_length], &CFG_MIN_DETECTION_VALUE,  sizeof(CFG_MIN_DETECTION_VALUE));  cmd.data_length += sizeof(CFG_MIN_DETECTION_VALUE);
    memcpy(&cmd.data[cmd.data_length], &CFG_NO_DELAY_VALUE,       sizeof(CFG_NO_DELAY_VALUE));       cmd.data_length += sizeof(CFG_NO_DELAY_VALUE);
    memcpy(&cmd.data[cmd.data_length], &CFG_STATUS_FREQ_VALUE,    sizeof(CFG_STATUS_FREQ_VALUE));    cmd.data_length += sizeof(CFG_STATUS_FREQ_VALUE);
    memcpy(&cmd.data[cmd.data_length], &CFG_DISTANCE_FREQ_VALUE,  sizeof(CFG_DISTANCE_FREQ_VALUE));  cmd.data_length += sizeof(CFG_DISTANCE_FREQ_VALUE);
    memcpy(&cmd.data[cmd.data_length], &CFG_RESPONSE_SPEED_VALUE, sizeof(CFG_RESPONSE_SPEED_VALUE)); cmd.data_length += sizeof(CFG_RESPONSE_SPEED_VALUE);

    cmd.footer = CMD_FRAME_FOOTER;
    return cmd;
}

CmdFrameT LD2410S::prepare_apply_config_cmd() {
    CmdFrameT cmd;
    cmd.header = CMD_FRAME_HEADER;
    cmd.command = WRITE_PARAMS_CMD;
    cmd.data_length = 0;

    Config to_save = this->new_config;

    memcpy(&cmd.data[cmd.data_length], &CFG_MAX_DETECTION_VALUE,  sizeof(CFG_MAX_DETECTION_VALUE));  cmd.data_length += sizeof(CFG_MAX_DETECTION_VALUE);
    memcpy(&cmd.data[cmd.data_length], &to_save.max_dist,         sizeof(to_save.max_dist));         cmd.data_length += sizeof(to_save.max_dist);
    memcpy(&cmd.data[cmd.data_length], &CFG_MIN_DETECTION_VALUE,  sizeof(CFG_MIN_DETECTION_VALUE));  cmd.data_length += sizeof(CFG_MIN_DETECTION_VALUE);
    memcpy(&cmd.data[cmd.data_length], &to_save.min_dist,         sizeof(to_save.min_dist));         cmd.data_length += sizeof(to_save.min_dist);
    memcpy(&cmd.data[cmd.data_length], &CFG_NO_DELAY_VALUE,       sizeof(CFG_NO_DELAY_VALUE));       cmd.data_length += sizeof(CFG_NO_DELAY_VALUE);
    memcpy(&cmd.data[cmd.data_length], &to_save.delay,            sizeof(to_save.delay));            cmd.data_length += sizeof(to_save.delay);
    memcpy(&cmd.data[cmd.data_length], &CFG_STATUS_FREQ_VALUE,    sizeof(CFG_STATUS_FREQ_VALUE));    cmd.data_length += sizeof(CFG_STATUS_FREQ_VALUE);
    memcpy(&cmd.data[cmd.data_length], &to_save.status_freq,      sizeof(to_save.status_freq));      cmd.data_length += sizeof(to_save.status_freq);
    memcpy(&cmd.data[cmd.data_length], &CFG_DISTANCE_FREQ_VALUE,  sizeof(CFG_DISTANCE_FREQ_VALUE));  cmd.data_length += sizeof(CFG_DISTANCE_FREQ_VALUE);
    memcpy(&cmd.data[cmd.data_length], &to_save.dist_freq,        sizeof(to_save.dist_freq));        cmd.data_length += sizeof(to_save.dist_freq);
    memcpy(&cmd.data[cmd.data_length], &CFG_RESPONSE_SPEED_VALUE, sizeof(CFG_RESPONSE_SPEED_VALUE)); cmd.data_length += sizeof(CFG_RESPONSE_SPEED_VALUE);
    memcpy(&cmd.data[cmd.data_length], &to_save.resp_speed,       sizeof(to_save.resp_speed));       cmd.data_length += sizeof(to_save.resp_speed);

    cmd.footer = CMD_FRAME_FOOTER;
    return cmd;
}

CmdFrameT LD2410S::prepare_threshold_cmd() {
    CmdFrameT cmd;
    cmd.header = CMD_FRAME_HEADER;
    cmd.command = AUTO_UPDATE_THRESHOLD_CMD;
    cmd.data_length = 0;

    memcpy(&cmd.data[cmd.data_length], &THRESHOLD_TRIGGER_VALUE,   sizeof(THRESHOLD_TRIGGER_VALUE));   cmd.data_length += sizeof(THRESHOLD_TRIGGER_VALUE);
    memcpy(&cmd.data[cmd.data_length], &THRESHOLD_RETENTION_VALUE, sizeof(THRESHOLD_RETENTION_VALUE)); cmd.data_length += sizeof(THRESHOLD_RETENTION_VALUE);
    memcpy(&cmd.data[cmd.data_length], &THRESHOLD_TIME_VALUE,      sizeof(THRESHOLD_TIME_VALUE));      cmd.data_length += sizeof(THRESHOLD_TIME_VALUE);

    cmd.footer = CMD_FRAME_FOOTER;
    return cmd;
}

CmdFrameT LD2410S::prepare_read_fw_cmd() {
    CmdFrameT cmd;
    cmd.header = CMD_FRAME_HEADER;
    cmd.command = READ_FW_CMD;
    cmd.data_length = 0;
    cmd.footer = CMD_FRAME_FOOTER;
    return cmd;
}

// ── Blocking command send ─────────────────────────────────────────

void LD2410S::send_command(CmdFrameT frame) {
    this->cmd_active_ = true;
    uint32_t start_millis = millis();
    uint8_t retry = 3;
    uint8_t cmd_buffer[64];

    while (retry) {
        frame.length = 0;
        uint16_t frame_data_bytes = frame.data_length + 2;  // +2 for command word

        // HEADER
        memcpy(&cmd_buffer[frame.length], &frame.header, sizeof(frame.header));
        frame.length += sizeof(frame.header);
        // SIZE
        memcpy(&cmd_buffer[frame.length], &frame_data_bytes, sizeof(frame.data_length));
        frame.length += sizeof(frame.data_length);
        // COMMAND
        memcpy(&cmd_buffer[frame.length], &frame.command, sizeof(frame.command));
        frame.length += sizeof(frame.command);
        // DATA
        for (uint16_t i = 0; i < frame.data_length; i++) {
            cmd_buffer[frame.length++] = frame.data[i];
        }
        // FOOTER
        memcpy(&cmd_buffer[frame.length], &frame.footer, sizeof(frame.footer));
        frame.length += sizeof(frame.footer);

        // WRITE
        for (uint16_t i = 0; i < frame.length; i++) {
            this->write_byte(cmd_buffer[i]);
        }
        this->flush();

        bool reply = false;
        while (!reply) {
            uint8_t ack_buffer[64];
            size_t last_pos = 0;
            while (available()) {
                PackageType type = this->read_line(read(), ack_buffer, last_pos++);
                if (type == PackageType::ACK) {
                    reply = this->process_cmd_ack_package(ack_buffer, last_pos + 1);
                    last_pos = 0;
                }
            }
            delay_microseconds_safe(1450);
            if ((millis() - start_millis) > 1000) {
                start_millis = millis();
                retry--;
                break;
            }
        }
        if (reply) {
            retry = 0;
        }
    }
    this->cmd_active_ = false;
}

// ── Frame byte-by-byte detector ───────────────────────────────────

PackageType LD2410S::read_line(uint8_t data, uint8_t *buffer, size_t pos) {
    buffer[pos] = data;

    if (pos > 4) {
        if (memcmp(&buffer[pos - 3], &CMD_FRAME_FOOTER, sizeof(CMD_FRAME_FOOTER)) == 0) {
            return PackageType::ACK;
        } else if (buffer[pos] == DATA_FRAME_FOOTER && buffer[pos - 4] == DATA_FRAME_HEADER) {
            return PackageType::SHORT_DATA;
        } else if (memcmp(&buffer[pos - 3], &THRESHOLD_FOOTER, sizeof(THRESHOLD_FOOTER)) == 0) {
            return PackageType::THRESHOLD;
        }
    }
    return PackageType::UNKNOWN;
}

// ── ACK processing ────────────────────────────────────────────────

bool LD2410S::process_cmd_ack_package(uint8_t *buffer, int len) {
    CmdAckT ack = this->parse_ack(buffer, len);
    if (!ack.result) {
        ESP_LOGW(TAG, "Command 0x%04X failed", ack.command);
        return false;
    }
    ESP_LOGI(TAG, "Command 0x%04X success", ack.command);

    switch (ack.command) {
        case START_CONFIG_MODE_REPLY:
            ESP_LOGD(TAG, "Config mode enabled");
            break;
        case END_CONFIG_MODE_REPLY:
            ESP_LOGD(TAG, "Config mode disabled");
            break;
        case READ_PARAMS_REPLY:
            this->process_config_read_ack(ack.data);
            break;
        case WRITE_PARAMS_REPLY:
            ESP_LOGD(TAG, "Write config reply processed");
            break;
        case READ_FW_REPLY:
            this->process_read_fw_ack(ack.data);
            break;
        default:
            ESP_LOGD(TAG, "Unknown reply: 0x%04X", ack.command);
            break;
    }
    return true;
}

CmdAckT LD2410S::parse_ack(uint8_t *buffer, size_t length) {
    CmdAckT result;
    size_t start = SIZE_MAX;
    for (size_t i = 0; i < length; i++) {
        if (memcmp(&buffer[i], &CMD_FRAME_HEADER, sizeof(CMD_FRAME_HEADER)) == 0) {
            start = i;
            break;
        }
    }
    if (start == SIZE_MAX) {
        ESP_LOGE(TAG, "Can't find cmd header");
        result.result = false;
        return result;
    }
    result.length  = this->two_byte_to_int(buffer[start + 4], buffer[start + 5]);
    result.command = this->two_byte_to_int(buffer[start + 6], buffer[start + 7]);
    result.result  = (buffer[start + 8] == 0x00 && buffer[start + 9] == 0x00);
    for (size_t idx = 0; idx < result.length && idx < sizeof(result.data); idx++) {
        result.data[idx] = buffer[idx + 10];
    }
    return result;
}

// ── Data frame processing ─────────────────────────────────────────

void LD2410S::process_data_package(PackageType type, uint8_t *buffer, size_t pos) {
    switch (type) {
        case PackageType::SHORT_DATA:
            this->process_short_data_package(&buffer[1]);
            break;
        case PackageType::THRESHOLD:
            this->process_threshold_package(&buffer[4]);
            break;
        default:
            break;
    }
}

void LD2410S::process_short_data_package(uint8_t *data) {
    const bool presenceState = data[0] > 1;
    int distance = this->two_byte_to_int(data[1], data[2]);
    for (auto &listener : this->listeners_) {
        listener->on_presence(presenceState);
        listener->on_distance(distance);
    }
}

void LD2410S::process_threshold_package(uint8_t *data) {
    int progress = this->two_byte_to_int(data[3], data[4]);
    for (auto &listener : this->listeners_) {
        if (progress == 100) {
            listener->on_threshold_progress(0);
            listener->on_threshold_update(false);
        } else {
            listener->on_threshold_progress(progress);
            listener->on_threshold_update(true);
        }
    }
}

// ── Config read ACK handlers ──────────────────────────────────────

void LD2410S::process_config_read_ack(uint8_t *data) {
    int max_dist       = this->read_int(data,  0, 4);
    int min_dist       = this->read_int(data,  4, 4);
    int delay          = this->read_int(data,  8, 4);
    int status_freq    = this->read_int(data, 12, 4);
    int dist_freq      = this->read_int(data, 16, 4);
    int resp_speed     = this->read_int(data, 20, 4);

    this->current_config_.max_dist    = max_dist;
    this->current_config_.min_dist    = min_dist;
    this->current_config_.delay       = delay;
    this->current_config_.status_freq = status_freq;
    this->current_config_.dist_freq   = dist_freq;
    this->current_config_.resp_speed  = resp_speed;
    memcpy(&this->new_config, &this->current_config_, sizeof(this->current_config_));

#ifdef USE_NUMBER
    if (this->max_distance_number_)         this->max_distance_number_->publish_state(max_dist);
    if (this->min_distance_number_)         this->min_distance_number_->publish_state(min_dist);
    if (this->no_delay_number_)             this->no_delay_number_->publish_state(delay);
    if (this->status_reporting_freq_number_) this->status_reporting_freq_number_->publish_state(status_freq / 10.0f);
    if (this->distance_reporting_freq_number_) this->distance_reporting_freq_number_->publish_state(dist_freq / 10.0f);
#endif
#ifdef USE_SELECT
    if (this->response_speed_select_)
        this->response_speed_select_->publish_state(resp_speed == 5 ? RESPONSE_SPEED_NORMAL : RESPONSE_SPEED_FAST);
#endif

    ESP_LOGD(TAG, "Config: max=%d min=%d delay=%d status_freq=%d dist_freq=%d resp_speed=%d",
             max_dist, min_dist, delay, status_freq, dist_freq, resp_speed);
}

void LD2410S::process_read_fw_ack(uint8_t *data) {
    int major = static_cast<int>(data[0]);
    int minor = static_cast<int>(data[1]);
    int patch = static_cast<int>(data[2]);
    std::string version = "v" + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
    for (auto &listener : this->listeners_) {
        listener->on_fw_version(version);
    }
    ESP_LOGD(TAG, "Firmware: %s", version.c_str());
}

}  // namespace ld2410s
}  // namespace esphome
