#pragma once

namespace esphome {
namespace bl0906_carrot8848 {

enum class VoltageSamplingMode {
  TRANSFORMER,
  RESISTOR_DIVIDER
};

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

void set_transformer_ratio(float ratio);
void set_voltage_sampling_mode(VoltageSamplingMode mode);

}  // namespace bl0906_carrot8848
}  // namespace esphome
