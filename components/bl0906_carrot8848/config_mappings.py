from esphome.const import (
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_HERTZ,
    UNIT_KILOWATT_HOURS,
    UNIT_VOLT,
    UNIT_WATT,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    CONF_CURRENT,
    CONF_ENERGY,
    CONF_FREQUENCY,
    CONF_POWER,
    CONF_TEMPERATURE,
    CONF_VOLTAGE,
)

CHIP_MODELS = {
    "bl0906": {
        "max_channels": 6
    },
    "bl0910": {
        "max_channels": 10
    }
}

EEPROM_TYPES = {
    "24c02": "24c02",
    "24c04": "24c04", 
    "24c08": "24c08",
    "24c16": "24c16"
}

FREQ_ADAPT_MODES = {
    "off": "OFF",
    "auto": "AUTO",
    "60": "HZ60"
}

VOLTAGE_SAMPLING_MODES = {
    "transformer": "TRANSFORMER",
    "resistor_divider": "RESISTOR_DIVIDER"
}

SENSOR_TYPES = {
    "VOLTAGE": 0,
    "FREQUENCY": 1,
    "TEMPERATURE": 2,
    "CURRENT": 3,
    "POWER": 4,
    "ENERGY": 5,
    "POWER_SUM": 6,
    "ENERGY_SUM": 7,
    "TOTAL_ENERGY": 8,
    "TOTAL_ENERGY_SUM": 9
}

STATISTICS_SENSOR_TYPES = {
    "YESTERDAY_ENERGY": 0,
    "TODAY_ENERGY": 1,
    "WEEK_ENERGY": 2,
    "MONTH_ENERGY": 3,
    "YEAR_ENERGY": 4,
    "YESTERDAY_TOTAL_ENERGY": 5,
    "TODAY_TOTAL_ENERGY": 6,
    "WEEK_TOTAL_ENERGY": 7,
    "MONTH_TOTAL_ENERGY": 8,
    "YEAR_TOTAL_ENERGY": 9
}

DEVICE_PROPERTY_TEMPLATES = {
    "voltage": {
        "unit": UNIT_VOLT,
        "accuracy": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
        "icon": "mdi:lightning-bolt-outline",
    },
    "current": {
        "unit": UNIT_AMPERE,
        "accuracy": 3,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
        "icon": "mdi:current-ac",
    },
    "power": {
        "unit": UNIT_WATT,
        "accuracy": 1,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
        "icon": "mdi:flash",
    },
    "energy": {
        "unit": UNIT_KILOWATT_HOURS,
        "accuracy": 3,
        "device_class": DEVICE_CLASS_ENERGY,
        "state_class": STATE_CLASS_TOTAL_INCREASING,
        "icon": "mdi:lightning-bolt",
    },
    "frequency": {
        "unit": UNIT_HERTZ,
        "accuracy": 1,
        "device_class": DEVICE_CLASS_FREQUENCY,
        "state_class": STATE_CLASS_MEASUREMENT,
        "icon": "mdi:sine-wave",
    },
    "temperature": {
        "unit": UNIT_CELSIUS,
        "accuracy": 1,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
        "icon": "mdi:thermometer",
    },
}

GLOBAL_SENSOR_CONFIGS = {
    CONF_FREQUENCY: {
        "type": "FREQUENCY",
        **DEVICE_PROPERTY_TEMPLATES["frequency"]
    },
    CONF_TEMPERATURE: {
        "type": "TEMPERATURE",
        **DEVICE_PROPERTY_TEMPLATES["temperature"]
    },
    CONF_VOLTAGE: {
        "type": "VOLTAGE",
        **DEVICE_PROPERTY_TEMPLATES["voltage"]
    },
    "power_sum": {
        "type": "POWER_SUM",
        **DEVICE_PROPERTY_TEMPLATES["power"],
        "icon": "mdi:sigma",
    },
    "energy_sum": {
        "type": "ENERGY_SUM",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:sigma",
    },
    "total_energy_sum": {
        "type": "TOTAL_ENERGY_SUM",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:sigma",
    },
    "yesterday_total_energy": {
        "type": "YESTERDAY_TOTAL_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-minus",
    },
    "today_total_energy": {
        "type": "TODAY_TOTAL_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-today",
    },
    "week_total_energy": {
        "type": "WEEK_TOTAL_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-week",
    },
    "month_total_energy": {
        "type": "MONTH_TOTAL_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-month",
    },
    "year_total_energy": {
        "type": "YEAR_TOTAL_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar",
    },
}

CHANNEL_SENSOR_TEMPLATES = {
    CONF_CURRENT: {
        "type": "CURRENT",
        **DEVICE_PROPERTY_TEMPLATES["current"]
    },
    CONF_POWER: {
        "type": "POWER",
        **DEVICE_PROPERTY_TEMPLATES["power"]
    },
    CONF_ENERGY: {
        "type": "ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"]
    },
    "total_energy": {
        "type": "TOTAL_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:counter",
    },
    "yesterday_energy": {
        "type": "YESTERDAY_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-minus",
    },
    "today_energy": {
        "type": "TODAY_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-today",
    },
    "week_energy": {
        "type": "WEEK_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-week",
    },
    "month_energy": {
        "type": "MONTH_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar-month",
    },
    "year_energy": {
        "type": "YEAR_ENERGY",
        **DEVICE_PROPERTY_TEMPLATES["energy"],
        "icon": "mdi:calendar",
    },
}


def is_statistics_sensor(sensor_type):
    return sensor_type in STATISTICS_SENSOR_TYPES

def get_sensor_enum_expression(sensor_type, is_statistics=False):
    if is_statistics:
        return f"esphome::bl0906_carrot8848::StatisticsSensorType::{sensor_type}"
    else:
        return f"esphome::bl0906_carrot8848::BL0906Carrot8848::SensorType::{sensor_type}"

