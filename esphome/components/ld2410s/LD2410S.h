#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#ifdef USE_NUMBER
#include "esphome/components/number/number.h"
#endif
#ifdef USE_BUTTON
#include "esphome/components/button/button.h"
#endif
#ifdef USE_SELECT
#include "esphome/components/select/select.h"
#endif

#include <vector>
#include <string>

namespace esphome {
namespace ld2410s {

// ── Frame markers ────────────────────────────────────────────────
// Short data format: single-byte header/footer
static const uint16_t DATA_FRAME_HEADER  = 0x6E;
static const uint16_t DATA_FRAME_FOOTER  = 0x62;

// Command/ACK frames: 4-byte header/footer (little-endian uint32)
static const uint32_t CMD_FRAME_HEADER   = 0xFAFBFCFD;  // wire: FD FC FB FA
static const uint32_t CMD_FRAME_FOOTER   = 0x01020304;  // wire: 04 03 02 01

// Threshold frames
static const uint32_t THRESHOLD_HEADER   = 0xF1F2F3F4;  // wire: F4 F3 F2 F1
static const uint32_t THRESHOLD_FOOTER   = 0xF5F6F7F8;  // wire: F8 F7 F6 F5

// ── Command words ─────────────────────────────────────────────────
static const uint16_t READ_FW_CMD              = 0x0000;
static const uint16_t START_CONFIG_MODE_CMD    = 0x00FF;
static const uint16_t END_CONFIG_MODE_CMD      = 0x00FE;
static const uint16_t WRITE_PARAMS_CMD         = 0x0070;
static const uint16_t READ_PARAMS_CMD          = 0x0071;
static const uint16_t AUTO_UPDATE_THRESHOLD_CMD = 0x0009;

// ── Config parameter IDs ──────────────────────────────────────────
static const uint16_t START_CONFIG_MODE_VALUE  = 0x0001;
static const uint16_t CFG_MAX_DETECTION_VALUE  = 0x0005;
static const uint16_t CFG_MIN_DETECTION_VALUE  = 0x000A;
static const uint16_t CFG_NO_DELAY_VALUE       = 0x0006;
static const uint16_t CFG_STATUS_FREQ_VALUE    = 0x0002;
static const uint16_t CFG_DISTANCE_FREQ_VALUE  = 0x000C;
static const uint16_t CFG_RESPONSE_SPEED_VALUE = 0x000B;
static const uint16_t THRESHOLD_TRIGGER_VALUE  = 0x0002;
static const uint16_t THRESHOLD_RETENTION_VALUE = 0x0001;
static const uint16_t THRESHOLD_TIME_VALUE     = 0x0078;

// ── ACK reply words ───────────────────────────────────────────────
static const uint16_t START_CONFIG_MODE_REPLY  = 0x01FF;
static const uint16_t END_CONFIG_MODE_REPLY    = 0x01FE;
static const uint16_t WRITE_PARAMS_REPLY       = 0x0170;
static const uint16_t READ_PARAMS_REPLY        = 0x0171;
static const uint16_t READ_FW_REPLY            = 0x0100;

static const std::string RESPONSE_SPEED_NORMAL = "Normal";
static const std::string RESPONSE_SPEED_FAST   = "Fast";

// ── Buffered configuration ────────────────────────────────────────
struct Config {
    uint32_t max_dist{0};
    uint32_t min_dist{0};
    uint32_t delay{0};
    uint32_t status_freq{0};
    uint32_t dist_freq{0};
    uint32_t resp_speed{0};
};

// ── Command frame structures ──────────────────────────────────────
struct CmdFrameT {
    uint32_t header{0};
    uint16_t length{0};
    uint16_t command{0};
    uint8_t  data[36]{};
    uint16_t data_length{0};
    uint32_t footer{0};
};

struct CmdAckT {
    uint16_t command{0};
    uint8_t  data[36]{};
    uint16_t length{0};
    bool     result{false};
};

enum class PackageType {
    ACK,
    SHORT_DATA,
    THRESHOLD,
    UNKNOWN
};

// ── Listener interface ────────────────────────────────────────────
class LD2410SListener {
 public:
    virtual void on_presence(bool presence) {}
    virtual void on_distance(int distance) {}
    virtual void on_threshold_update(bool running) {}
    virtual void on_threshold_progress(int progress) {}
    virtual void on_fw_version(std::string &fw) {}
};

// ── Main component ────────────────────────────────────────────────
class LD2410S : public uart::UARTDevice, public Component {
 public:
    Config new_config;

    void setup() override;
    void loop() override;
    float get_setup_priority() const override { return setup_priority::HARDWARE; }

    void register_listener(LD2410SListener *listener) { listeners_.push_back(listener); }

    void set_config_mode(bool enabled);
    void apply_config();
    void start_auto_threshold_update();

#ifdef USE_NUMBER
    void set_max_distance_number(number::Number *n)            { max_distance_number_ = n; }
    void set_min_distance_number(number::Number *n)            { min_distance_number_ = n; }
    void set_no_delay_number(number::Number *n)                { no_delay_number_ = n; }
    void set_status_reporting_freq_number(number::Number *n)   { status_reporting_freq_number_ = n; }
    void set_distance_reporting_freq_number(number::Number *n) { distance_reporting_freq_number_ = n; }
#endif
#ifdef USE_BUTTON
    void set_enable_config_button(button::Button *b)  { enable_config_button_  = b; }
    void set_disable_config_button(button::Button *b) { disable_config_button_ = b; }
    void set_apply_config_button(button::Button *b)   { apply_config_button_   = b; }
    void set_auto_threshold_button(button::Button *b) { auto_threshold_button_ = b; }
#endif
#ifdef USE_SELECT
    void set_response_speed_select(select::Select *s) { response_speed_select_ = s; }
#endif

 private:
    std::vector<LD2410SListener *> listeners_;
    Config current_config_;
    bool cmd_active_{false};

#ifdef USE_NUMBER
    number::Number *max_distance_number_{nullptr};
    number::Number *min_distance_number_{nullptr};
    number::Number *no_delay_number_{nullptr};
    number::Number *status_reporting_freq_number_{nullptr};
    number::Number *distance_reporting_freq_number_{nullptr};
#endif
#ifdef USE_BUTTON
    button::Button *enable_config_button_{nullptr};
    button::Button *disable_config_button_{nullptr};
    button::Button *apply_config_button_{nullptr};
    button::Button *auto_threshold_button_{nullptr};
#endif
#ifdef USE_SELECT
    select::Select *response_speed_select_{nullptr};
#endif

    CmdFrameT prepare_read_config_cmd();
    CmdFrameT prepare_apply_config_cmd();
    CmdFrameT prepare_threshold_cmd();
    CmdFrameT prepare_read_fw_cmd();
    void send_command(CmdFrameT cmd_frame);

    PackageType read_line(uint8_t data, uint8_t *buffer, size_t pos);
    bool process_cmd_ack_package(uint8_t *buffer, int len);
    void process_data_package(PackageType type, uint8_t *buffer, size_t pos);

    CmdAckT parse_ack(uint8_t *buffer, size_t length);
    void process_config_read_ack(uint8_t *data);
    void process_read_fw_ack(uint8_t *data);
    void process_short_data_package(uint8_t *data);
    void process_threshold_package(uint8_t *data);

    int read_int(uint8_t *buffer, size_t pos, size_t len) {
        unsigned int ret = 0;
        int shift = 0;
        for (size_t i = 0; i < len; i++) {
            ret |= static_cast<unsigned int>(buffer[pos + i]) << shift;
            shift += 8;
        }
        return ret;
    }
    int two_byte_to_int(uint8_t lo, uint8_t hi) { return (hi << 8) + lo; }
};

}  // namespace ld2410s
}  // namespace esphome
