import esphome.codegen as cg
import esphome.config_validation as cv
import os
from esphome.components import i2c, sensor
from esphome.const import ICON_EMPTY, UNIT_EMPTY
from esphome.const import CONF_ID

DEPENDENCIES = ["i2c"]

bmv080_ns = cg.esphome_ns.namespace("bmv080")
BMV080Sensor = bmv080_ns.class_(
    "BMV080Sensor", cg.PollingComponent, i2c.I2CDevice
)

CONFIG_SCHEMA = (
    sensor.sensor_schema(
        BMV080Sensor,
        unit_of_measurement=UNIT_EMPTY,
        icon=ICON_EMPTY,
        accuracy_decimals=1,
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(i2c.i2c_device_schema(0x54))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)
    current_dir = os.path.dirname(os.path.realpath(__file__))
    safe_path = current_dir.replace('\\', '/')
    cg.add_platformio_option("build_flags", [
        f"-L{safe_path}", 
        "-l_bmv080", 
        "-l_postProcessor",
        f"-I{safe_path}"
    ])