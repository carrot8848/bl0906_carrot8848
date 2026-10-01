"""BL0906 Carrot8848 - 简化版电能计量组件（仅SPI通信，预编译库发布）"""
import os

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, spi
from esphome import pins
from esphome.core import CORE
from esphome.const import (
    CONF_ID,
    CONF_CS_PIN,
    CONF_SPI_ID,
    CONF_TIME_ID,
    CONF_I2C_ID,
    CONF_ADDRESS,
)

CODEOWNERS = ["@carrot8848"]
AUTO_LOAD = ["sensor", "text_sensor", "button"]  # 生产版不包含number组件
MULTI_CONF = True

bl0906_carrot8848_ns = cg.esphome_ns.namespace("bl0906_carrot8848")
BL0906Carrot8848 = bl0906_carrot8848_ns.class_("BL0906Carrot8848", cg.PollingComponent)

SpiCommunicationAdapter = bl0906_carrot8848_ns.class_("SpiCommunicationAdapter", spi.SPIDevice)

SensorType = bl0906_carrot8848_ns.enum("SensorType", is_class=True)
StatisticsSensorType = bl0906_carrot8848_ns.enum("StatisticsSensorType", is_class=True)
EEPROMType = bl0906_carrot8848_ns.enum("EEPROMType", is_class=True)
FreqAdaptMode = bl0906_carrot8848_ns.enum("FreqAdaptMode", is_class=True)

from .config_mappings import (
    EEPROM_TYPES,
    FREQ_ADAPT_MODES,
    VOLTAGE_SAMPLING_MODES,
    CHIP_MODELS,
)

CONF_BL0906_CARROT8848_ID = "bl0906_carrot8848_id"
CONF_CHIP_MODEL = "chip_model"

CONF_EEPROM_TYPE = "eeprom_type"
CONF_INSTANCE_ID = "instance_id"
CONF_FREQ_ADAPT = "freq_adapt"
CONF_VOLTAGE_SAMPLING_MODE = "voltage_sampling_mode"
CONF_ENERGY_STATISTICS = "energy_statistics"
CONF_TRANSFORMER_RATIO = "transformer_ratio"

# C++ 枚举映射
CHIP_MODEL_ENUM_MAP = {
    "bl0906": "esphome::bl0906_carrot8848::ChipModel::BL0906",
    "bl0910": "esphome::bl0906_carrot8848::ChipModel::BL0910",
}
EEPROM_TYPE_ENUM_MAP = {
    "24c02": "esphome::bl0906_carrot8848::EEPROMType::TYPE_24C02",
    "24c04": "esphome::bl0906_carrot8848::EEPROMType::TYPE_24C04",
    "24c08": "esphome::bl0906_carrot8848::EEPROMType::TYPE_24C08",
    "24c16": "esphome::bl0906_carrot8848::EEPROMType::TYPE_24C16",
}
FREQ_ADAPT_ENUM_MAP = {
    "off": "esphome::bl0906_carrot8848::BL0906Carrot8848::FreqAdaptMode::OFF",
    "auto": "esphome::bl0906_carrot8848::BL0906Carrot8848::FreqAdaptMode::AUTO",
    "60": "esphome::bl0906_carrot8848::BL0906Carrot8848::FreqAdaptMode::HZ60",
}
VOLTAGE_SAMPLING_ENUM_MAP = {
    "transformer": "esphome::bl0906_carrot8848::VoltageSamplingMode::TRANSFORMER",
    "resistor_divider": "esphome::bl0906_carrot8848::VoltageSamplingMode::RESISTOR_DIVIDER",
}
# 按钮功能映射（bl0906_buttons.h 中 Bl0906ButtonAction）
BUTTON_ACTION_ENUM_MAP = {
    "reset_energy": "esphome::bl0906_carrot8848::Bl0906ButtonAction::RESET_ENERGY",
    "save_energy": "esphome::bl0906_carrot8848::Bl0906ButtonAction::SAVE_ENERGY",
    "diagnose_persistence": "esphome::bl0906_carrot8848::Bl0906ButtonAction::DIAGNOSE_PERSISTENCE",
    "diagnose_statistics": "esphome::bl0906_carrot8848::Bl0906ButtonAction::DIAGNOSE_STATISTICS",
}

def validate_energy_statistics_config(config):
    """验证电量统计配置的完整性"""
    energy_statistics_enabled = config[CONF_ENERGY_STATISTICS]
    time_id_configured = CONF_TIME_ID in config

    if energy_statistics_enabled and not time_id_configured:
        raise cv.Invalid(
            f"When '{CONF_ENERGY_STATISTICS}' is set to true, "
            f"'{CONF_TIME_ID}' must be configured. "
            f"Energy statistics require a time component to track daily, weekly, "
            f"monthly and yearly energy consumption."
        )
    
    return config

# 基础配置模式
BASE_CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(BL0906Carrot8848),
    cv.Optional(CONF_CHIP_MODEL, default="bl0906"): cv.enum(CHIP_MODELS, lower=True),

    cv.Required(CONF_INSTANCE_ID): cv.hex_uint32_t,
    cv.Optional(CONF_FREQ_ADAPT, default="off"): cv.enum(FREQ_ADAPT_MODES, lower=True),
    cv.Optional(CONF_VOLTAGE_SAMPLING_MODE, default="transformer"): cv.enum(VOLTAGE_SAMPLING_MODES, lower=True),
    cv.Optional(CONF_TRANSFORMER_RATIO, default=2000.0): cv.All(
        cv.positive_float,
        cv.Range(min=1.0, max=50000.0)
    ),
    cv.Optional(CONF_ENERGY_STATISTICS, default=False): cv.boolean,
    cv.Optional(CONF_TIME_ID): cv.use_id("time"),
    cv.Optional(CONF_SPI_ID): cv.use_id(spi.SPIComponent),
    cv.Optional(CONF_CS_PIN): pins.gpio_output_pin_schema,
    cv.Optional(CONF_EEPROM_TYPE, default="24c02"): cv.one_of(*EEPROM_TYPES, lower=True),
    cv.Required(CONF_I2C_ID): cv.use_id(i2c.I2CBus),
    cv.Optional(CONF_ADDRESS, default=0x50): cv.i2c_address,
}).extend(cv.polling_component_schema("60s"))

CONFIG_SCHEMA = cv.All(
    BASE_CONFIG_SCHEMA,
    validate_energy_statistics_config
)

async def to_code(config):
    # === BL0906_PREBUILT_LINK_BEGIN === 发布形态专用段, assemble_reference_component.py 组装源码形态时整段剥离
    # 预编译库链接: IDF构建不用外部组件CMakeLists.txt,须以单条-Wl标志注入
    # (单元素不受链接标志排序影响,whole-archive与gc-sections兼容)
    if CORE.target_platform != "esp32":
        raise cv.Invalid(
            "bl0906_carrot8848 组件以预编译库发布，当前仅支持 ESP32 平台"
        )
    from esphome.components import esp32

    target = esp32.get_esp32_variant().lower()  # prebuilt 目录用小写变体名
    lib_path = os.path.join(
        os.path.dirname(os.path.abspath(__file__)),
        "prebuilt", target, "libbl0906_carrot8848.a",
    )
    if not os.path.isfile(lib_path):
        raise cv.Invalid(
            f"BL0906: 未找到 {target} 平台的预编译库 ({lib_path})。"
            "本组件以预编译二进制发布，请确认组件版本包含该平台的 prebuilt 文件。"
        )
    cg.add_build_flag(f"-Wl,--whole-archive,{lib_path},--no-whole-archive")
    # === BL0906_PREBUILT_LINK_END ===

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    chip_model = config[CONF_CHIP_MODEL]
    cg.add(var.set_chip_model(cg.RawExpression(CHIP_MODEL_ENUM_MAP[chip_model])))

    spi_component = await cg.get_variable(config[CONF_SPI_ID])
    cs_pin = await cg.gpio_pin_expression(config[CONF_CS_PIN])

    # 独立块作用域避免多实例重复声明spi_adapter
    cg.add(cg.RawExpression(f"""
        {{
            auto spi_adapter = std::unique_ptr<esphome::bl0906_carrot8848::SpiCommunicationAdapter>(
                new esphome::bl0906_carrot8848::SpiCommunicationAdapter());
            spi_adapter->set_spi_parent({spi_component});
            spi_adapter->set_cs_pin({cs_pin});
            {var}->set_communication_adapter(std::move(spi_adapter));
        }}
    """))

    i2c_component = await cg.get_variable(config[CONF_I2C_ID])
    cg.add(var.set_i2c_parent(i2c_component))
    eeprom_type = config[CONF_EEPROM_TYPE]
    cg.add(var.set_eeprom_type(cg.RawExpression(EEPROM_TYPE_ENUM_MAP[eeprom_type])))
    cg.add(var.set_i2c_address(config[CONF_ADDRESS]))

    instance_id = config[CONF_INSTANCE_ID]
    cg.add(var.set_instance_id(instance_id))

    freq_adapt_mode = config[CONF_FREQ_ADAPT]
    cg.add(var.set_freq_adapt_mode(cg.RawExpression(FREQ_ADAPT_ENUM_MAP[freq_adapt_mode])))

    voltage_sampling_mode = config[CONF_VOLTAGE_SAMPLING_MODE]
    cg.add(cg.RawExpression(
        f'esphome::bl0906_carrot8848::set_voltage_sampling_mode('
        f'{VOLTAGE_SAMPLING_ENUM_MAP[voltage_sampling_mode]})'
    ))

    transformer_ratio = config[CONF_TRANSFORMER_RATIO]
    cg.add(cg.RawExpression(
        f'esphome::bl0906_carrot8848::set_transformer_ratio({transformer_ratio}f)'
    ))
    cg.add(cg.RawExpression(f'ESP_LOGD("bl0906_carrot8848", "互感器变比设置为: {transformer_ratio}:1 (运行时)");'))

    energy_statistics_enabled = config[CONF_ENERGY_STATISTICS]
    cg.add(cg.RawExpression(f'{var}->energy_statistics_enabled_ = {str(energy_statistics_enabled).lower()}'))

    if CONF_TIME_ID in config:
        time_component = await cg.get_variable(config[CONF_TIME_ID])
        cg.add(var.set_time_component(time_component))
