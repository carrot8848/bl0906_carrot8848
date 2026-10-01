import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import CONF_ID
from . import BL0906Carrot8848, bl0906_carrot8848_ns, CONF_BL0906_CARROT8848_ID, BUTTON_ACTION_ENUM_MAP

CONF_FUNCTION = "function"

DEPENDENCIES = ["bl0906_carrot8848"]

BL0906ActionButton = bl0906_carrot8848_ns.class_(
    "BL0906ActionButton", button.Button, cg.Component
)

CONFIG_SCHEMA = button.button_schema(BL0906ActionButton).extend(
    {
        cv.GenerateID(CONF_BL0906_CARROT8848_ID): cv.use_id(BL0906Carrot8848),
        cv.Required(CONF_FUNCTION): cv.enum(BUTTON_ACTION_ENUM_MAP, lower=True),
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await button.register_button(var, config)
    parent = await cg.get_variable(config[CONF_BL0906_CARROT8848_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_action(cg.RawExpression(BUTTON_ACTION_ENUM_MAP[config[CONF_FUNCTION]])))
