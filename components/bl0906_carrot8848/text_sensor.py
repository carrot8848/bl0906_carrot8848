import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from . import BL0906Carrot8848, CONF_BL0906_CARROT8848_ID

DEPENDENCIES = ["bl0906_carrot8848"]

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(CONF_BL0906_CARROT8848_ID): cv.use_id(BL0906Carrot8848),
    cv.Required("calibration_status"): text_sensor.text_sensor_schema(
        icon="mdi:tune-vertical"
    ),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = await cg.get_variable(config[CONF_BL0906_CARROT8848_ID])
    
    if "calibration_status" in config:
        sens = await text_sensor.new_text_sensor(config["calibration_status"])
        cg.add(var.set_calibration_status_text_sensor(sens))
