#pragma once
#include <cstdint>

namespace esphome {
namespace bl0906_carrot8848 {

enum class ChipModel : uint8_t {
  BL0906 = 0,
  BL0910 = 1
};

enum class CalibRegType {
  I_RMS,    // 电流有效值
  WATT,     // 功率
  CF_CNT,   // 脉冲计数
  RMSGN,    // 有效值增益
  RMSOS,    // 有效值偏置
  CHGN,     // 电流增益
  CHOS,     // 电流偏置
  WATTGN,   // 功率增益
  WATTOS,   // 功率偏置
  CHGN_V,   // 电压增益
  CHOS_V    // 电压偏置
};

struct ChipParams {
  uint8_t channel_count;
  const uint8_t* i_rms_addr;
  const uint8_t* watt_addr;
  const uint8_t* cf_cnt_addr;
  const uint8_t* rmsgn_addr;
  const uint8_t* rmsos_addr;
  const uint8_t* chgn_addr;
  const uint8_t* chos_addr;
  const uint8_t* wattgn_addr;
  const uint8_t* wattos_addr;
  const char* chip_name;
};

struct ube24_t {  // 无符号24位
  uint8_t l, m, h;
};

struct sbe24_t {  // 有符号24位
  uint8_t l, m;
  int8_t h;
};

struct DataPacket {
  uint8_t l, m, h, checksum;
};

// 以下定义均在预编译库 libbl0906_carrot8848.a 中
extern const ChipParams CHIP_PARAMS[];

uint8_t get_register_addr(ChipModel chip, CalibRegType type, int channel);
bool is_valid_calibration_register(ChipModel chip, uint8_t address);
bool is_valid_register_for_chip(ChipModel chip, uint8_t address);
CalibRegType get_register_type_by_address(ChipModel chip, uint8_t address);

extern const uint8_t V_RMS_ADDR;
extern const uint8_t FREQUENCY_ADDR;
extern const uint8_t TEMPERATURE_ADDR;
extern const uint8_t WATT_SUM_ADDR;
extern const uint8_t CF_SUM_ADDR;
extern const uint8_t MODE2_ADDR;
extern const uint8_t CHGN_V_ADDR;

extern const uint32_t MODE2_AC_FREQ_SEL_MASK;

}  // namespace bl0906_carrot8848
}  // namespace esphome
