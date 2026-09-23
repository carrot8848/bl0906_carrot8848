#pragma once

#include "bl0906_api.h"

namespace esphome {
namespace bl0906_carrot8848 {

enum class VoltageSamplingMode {
  TRANSFORMER,
  RESISTOR_DIVIDER
};

BL0906_API extern const float Vref;   // V
BL0906_API extern const int Gain_V;   // 1, 2, 8, 16
BL0906_API extern const int Gain_I;   // 1, 2, 8, 16
BL0906_API extern const float RL;     // Ω

BL0906_API extern float Rt;

BL0906_API extern float Ki;      // 电流系数
BL0906_API extern float Kv;      // 电压系数
BL0906_API extern float Kp;      // 功率系数
BL0906_API extern float Ke;      // kWh/pulse
BL0906_API extern float Kp_sum;  // WATT_SUM 用
BL0906_API extern float Ke_sum;  // CF_SUM 用

BL0906_API void set_transformer_ratio(float ratio);
BL0906_API void set_voltage_sampling_mode(VoltageSamplingMode mode);

}  // namespace bl0906_carrot8848
}  // namespace esphome
