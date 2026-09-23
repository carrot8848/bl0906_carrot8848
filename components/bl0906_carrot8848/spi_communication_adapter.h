#pragma once

#include "esphome/components/spi/spi.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/gpio.h"
#include "soc/soc.h"
#include <functional>
#include <string>
#include <cstdint>
#include <cstddef>

#include "bl0906_api.h"

namespace esphome {
namespace bl0906_carrot8848 {

static const char *const SPI_COMM_ADAPTER_TAG = "spi_comm_adapter";  // 模板函数中需要的日志TAG
#define TAG SPI_COMM_ADAPTER_TAG

enum class CommunicationError {
  SUCCESS = 0,
  TIMEOUT,
  CHECKSUM_ERROR,
  DEVICE_NOT_AVAILABLE,
  BUFFER_OVERFLOW,
  INVALID_RESPONSE,
  HARDWARE_ERROR,
  UNKNOWN_ERROR
};

// 具体类,无虚函数
class BL0906_API SpiCommunicationAdapter : public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW, spi::CLOCK_PHASE_TRAILING, spi::DATA_RATE_1MHZ> {
public:
  SpiCommunicationAdapter() = default;
  ~SpiCommunicationAdapter() = default;

  void set_spi_parent(spi::SPIComponent *parent);
  void set_cs_pin(GPIOPin *cs_pin);

  bool initialize();
  int32_t read_register(uint8_t address, bool* success = nullptr);
  bool write_register(uint8_t address, int16_t value);
  bool send_raw_command(const uint8_t* command, size_t length);

  bool is_available();
  std::string get_last_error() const;
  std::string get_adapter_type() const;
  bool self_test();

private:
  static constexpr uint32_t SPI_DELAY_US = 50;
  static constexpr uint32_t CS_SETUP_DELAY_US = 10;

  static constexpr uint32_t DEFAULT_RETRY_COUNT = 3;
  static constexpr uint32_t RETRY_DELAY_MS = 10;

  bool initialized_ = false;
  CommunicationError last_error_ = CommunicationError::SUCCESS;
  std::string last_error_message_;

  int32_t send_spi_read_command(uint8_t address, bool* success);
  bool send_spi_write_command(uint8_t address, int16_t value);

  bool safe_spi_operation(std::function<void()> operation) {
    operation();
    return true;
  }

  void set_error(CommunicationError error, const std::string& message);

  template<typename T>
  T execute_with_retry(std::function<T()> operation,
                       int max_retries = DEFAULT_RETRY_COUNT,
                       uint32_t retry_delay_ms = RETRY_DELAY_MS) {
    T result = T();
    bool success = false;
    for (int retry = 0; retry < max_retries; retry++) {
      result = operation();
      if (last_error_ == CommunicationError::SUCCESS) {
        success = true;
        break;
      }
      if (retry < max_retries - 1) {
        ESP_LOGD(TAG, "操作失败，重试中 (%d/%d)", retry + 1, max_retries);
        delay(retry_delay_ms);
      }
    }
    if (!success) {
      ESP_LOGE(TAG, "操作失败，已达最大重试次数");
    }
    return result;
  }

  int32_t parse_register_response(uint8_t address, uint8_t data_h, uint8_t data_m, uint8_t data_l);
  bool prepare_register_write_data(uint8_t address, int16_t value, uint8_t& data_h, uint8_t& data_m, uint8_t& data_l);
  bool is_16bit_register(uint8_t address);
  bool is_unsigned_register(uint8_t address);
  int32_t convert_to_signed_24bit(uint8_t data_h, uint8_t data_m, uint8_t data_l);
  uint32_t convert_to_unsigned_24bit(uint8_t data_h, uint8_t data_m, uint8_t data_l);
  int16_t convert_to_signed_16bit(uint8_t data_h, uint8_t data_l);
  int32_t int16_to_int24(int16_t value);
};

} // namespace bl0906_carrot8848
} // namespace esphome
