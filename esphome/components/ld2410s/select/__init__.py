import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import ENTITY_CATEGORY_CONFIG
from .. import ld2410s_ns, LD2410S, CONF_LD2410S_ID

LD2410SResponseSpeedSelect = ld2410s_ns.class_(
    "LD2410SResponseSpeedSelect", cg.Component, select.Select
)

CONF_RESPONSE_SPEED = "response_speed"
RESPONSE_SPEED_OPTIONS = ["Normal", "Fast"]

CONFIG_SCHEMA = {
    cv.GenerateID(CONF_LD2410S_ID): cv.use_id(LD2410S),
    cv.Required(CONF_RESPONSE_SPEED): select.select_schema(
        LD2410SResponseSpeedSelect,
        entity_category=ENTITY_CATEGORY_CONFIG,
    ),
}


async def to_code(config):
    parent = await cg.get_variable(config[CONF_LD2410S_ID])
    if cfg := config.get(CONF_RESPONSE_SPEED):
        sel = await select.new_select(cfg, options=RESPONSE_SPEED_OPTIONS)
        await cg.register_parented(sel, config[CONF_LD2410S_ID])
        cg.add(parent.set_response_speed_select(sel))
