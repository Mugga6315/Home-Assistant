import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    DEVICE_CLASS_DISTANCE,
    DEVICE_CLASS_DURATION,
    DEVICE_CLASS_FREQUENCY,
    ENTITY_CATEGORY_CONFIG,
    UNIT_SECOND,
    UNIT_HERTZ,
)
from .. import ld2410s_ns, LD2410S, CONF_LD2410S_ID

LD2410SMaxDistanceNumber         = ld2410s_ns.class_("LD2410SMaxDistanceNumber",         number.Number)
LD2410SMinDistanceNumber         = ld2410s_ns.class_("LD2410SMinDistanceNumber",         number.Number)
LD2410SNoDelayNumber             = ld2410s_ns.class_("LD2410SNoDelayNumber",             number.Number)
LD2410SStatusReportingFreqNumber = ld2410s_ns.class_("LD2410SStatusReportingFreqNumber", number.Number)
LD2410SDistReportingFreqNumber   = ld2410s_ns.class_("LD2410SDistReportingFreqNumber",   number.Number)

CONF_MAX_DISTANCE            = "max_distance"
CONF_MIN_DISTANCE            = "min_distance"
CONF_NO_DELAY                = "no_delay"
CONF_STATUS_REPORTING_FREQ   = "status_reporting_frequency"
CONF_DISTANCE_REPORTING_FREQ = "distance_reporting_frequency"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_LD2410S_ID): cv.use_id(LD2410S),
        cv.Optional(CONF_MAX_DISTANCE): number.number_schema(
            LD2410SMaxDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_MIN_DISTANCE): number.number_schema(
            LD2410SMinDistanceNumber,
            device_class=DEVICE_CLASS_DISTANCE,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_NO_DELAY): number.number_schema(
            LD2410SNoDelayNumber,
            device_class=DEVICE_CLASS_DURATION,
            unit_of_measurement=UNIT_SECOND,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_STATUS_REPORTING_FREQ): number.number_schema(
            LD2410SStatusReportingFreqNumber,
            device_class=DEVICE_CLASS_FREQUENCY,
            unit_of_measurement=UNIT_HERTZ,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_DISTANCE_REPORTING_FREQ): number.number_schema(
            LD2410SDistReportingFreqNumber,
            device_class=DEVICE_CLASS_FREQUENCY,
            unit_of_measurement=UNIT_HERTZ,
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LD2410S_ID])

    if cfg := config.get(CONF_MAX_DISTANCE):
        n = await number.new_number(cfg, min_value=0, max_value=16, step=1)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(parent.set_max_distance_number(n))

    if cfg := config.get(CONF_MIN_DISTANCE):
        n = await number.new_number(cfg, min_value=0, max_value=16, step=1)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(parent.set_min_distance_number(n))

    if cfg := config.get(CONF_NO_DELAY):
        n = await number.new_number(cfg, min_value=10, max_value=120, step=1)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(parent.set_no_delay_number(n))

    if cfg := config.get(CONF_STATUS_REPORTING_FREQ):
        n = await number.new_number(cfg, min_value=0.5, max_value=8.0, step=0.5)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(parent.set_status_reporting_freq_number(n))

    if cfg := config.get(CONF_DISTANCE_REPORTING_FREQ):
        n = await number.new_number(cfg, min_value=0.5, max_value=8.0, step=0.5)
        await cg.register_parented(n, config[CONF_LD2410S_ID])
        cg.add(parent.set_distance_reporting_freq_number(n))
