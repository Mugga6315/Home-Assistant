import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import CONF_ID, DEVICE_CLASS_OCCUPANCY, CONF_HAS_TARGET
from .. import ld2410s_ns, LD2410S, CONF_LD2410S_ID

LD2410SBinarySensor = ld2410s_ns.class_(
    "LD2410SBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONFIG_SCHEMA = cv.All(
    cv.COMPONENT_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(LD2410SBinarySensor),
            cv.GenerateID(CONF_LD2410S_ID): cv.use_id(LD2410S),
            cv.Optional(CONF_HAS_TARGET): binary_sensor.binary_sensor_schema(
                device_class=DEVICE_CLASS_OCCUPANCY
            ),
        }
    )
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    if CONF_HAS_TARGET in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_HAS_TARGET])
        cg.add(var.set_presence_sensor(sens))
    parent = await cg.get_variable(config[CONF_LD2410S_ID])
    cg.add(parent.register_listener(var))
