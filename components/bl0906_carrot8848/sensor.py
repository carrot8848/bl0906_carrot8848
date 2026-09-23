import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor

from .config_mappings import (
    CHIP_MODELS,
    GLOBAL_SENSOR_CONFIGS,
    CHANNEL_SENSOR_TEMPLATES,
    is_statistics_sensor,
    get_sensor_enum_expression,
)
from . import BL0906Carrot8848, CONF_BL0906_CARROT8848_ID

DEPENDENCIES = ["bl0906_carrot8848"]

def build_channel_sensor_schema():
    schema_dict = {}
    for sensor_key, config in CHANNEL_SENSOR_TEMPLATES.items():
        schema_dict[cv.Optional(sensor_key)] = sensor.sensor_schema(
            unit_of_measurement=config["unit"],
            accuracy_decimals=config["accuracy"],
            device_class=config["device_class"],
            state_class=config["state_class"],
        )
    return cv.Schema(schema_dict)

def build_config_schema():
    schema_dict = {cv.GenerateID(CONF_BL0906_CARROT8848_ID): cv.use_id(BL0906Carrot8848)}

    for key, config in GLOBAL_SENSOR_CONFIGS.items():
        schema_dict[cv.Optional(key)] = sensor.sensor_schema(
            unit_of_measurement=config["unit"],
            accuracy_decimals=config["accuracy"],
            device_class=config["device_class"],
            state_class=config["state_class"],
        )

    # 通道数在C++层面按芯片型号验证
    channel_schema = build_channel_sensor_schema()
    max_channels = max(chip_info["max_channels"] for chip_info in CHIP_MODELS.values())
    for i in range(1, max_channels + 1):
        schema_dict[cv.Optional(f"ch{i}")] = channel_schema

    return cv.Schema(schema_dict)

CONFIG_SCHEMA = build_config_schema()

async def to_code(config):
    var = await cg.get_variable(config[CONF_BL0906_CARROT8848_ID])

    # 通道数在C++层面按芯片型号验证
    max_channels = max(chip_info["max_channels"] for chip_info in CHIP_MODELS.values())
    sum_channel_index = max_channels  # "总和"通道索引

    for sensor_key, sensor_config in GLOBAL_SENSOR_CONFIGS.items():
        if sensor_key in config:
            sens = await sensor.new_sensor(config[sensor_key])
            sensor_type = sensor_config["type"]

            is_stats = is_statistics_sensor(sensor_type)
            enum_expr = cg.RawExpression(get_sensor_enum_expression(sensor_type, is_stats))

            if is_stats:
                # *_TOTAL_ENERGY 注册到"总和"通道,避免和 ch1 的 *_ENERGY 冲突
                cg.add(var.set_statistics_sensor(enum_expr, sens, sum_channel_index))
            else:
                cg.add(var.set_sensor(enum_expr, sens, 0))

    for i in range(1, max_channels + 1):
        channel_key = f"ch{i}"
        if channel_key in config:
            channel_config = config[channel_key]
            channel_index = i - 1

            for sensor_key, sensor_config in CHANNEL_SENSOR_TEMPLATES.items():
                if sensor_key in channel_config:
                    sens = await sensor.new_sensor(channel_config[sensor_key])
                    sensor_type = sensor_config["type"]

                    is_stats = is_statistics_sensor(sensor_type)
                    enum_expr = cg.RawExpression(get_sensor_enum_expression(sensor_type, is_stats))

                    if is_stats:
                        cg.add(var.set_statistics_sensor(enum_expr, sens, channel_index))
                    else:
                        cg.add(var.set_sensor(enum_expr, sens, channel_index))
