#pragma once

#include "bl0906_api.h"

namespace esphome {
namespace bl0906_carrot8848 {

enum class VoltageSamplingMode {
  TRANSFORMER,
  RESISTOR_DIVIDER
};

// 物理与校准系数常量：仅库内使用，已降级为隐藏符号（-fvisibility=hidden），
// 不对固件侧导出；如需对外暴露请加 BL0906_API 并更新符号校验清单
extern const float Vref;   // V
extern const int Gain_V;   // 1, 2, 8, 16
extern const int Gain_I;   // 1, 2, 8, 16
extern const float RL;     // Ω

extern float Rt;

extern float Ki;      // 电流系数
extern float Kv;      // 电压系数
extern float Kp;      // 功率系数
extern float Ke;      // kWh/pulse
extern float Kp_sum;  // WATT_SUM 用
extern float Ke_sum;  // CF_SUM 用

BL0906_API void set_transformer_ratio(float ratio);
BL0906_API void set_voltage_sampling_mode(VoltageSamplingMode mode);

}  // namespace bl0906_carrot8848
}  // namespace esphome
