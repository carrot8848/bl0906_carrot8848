#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

// 必须最先包含
#include "bl0906_chip_params.h"

#include "spi_communication_adapter.h"
#include "bl0906_calibration.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esphome/core/defines.h"
#include "esphome/core/log.h"
#include "esphome/components/time/real_time_clock.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <memory>

#include "i2c_eeprom_calibration_storage.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome_time = esphome::time;

namespace esphome {
namespace bl0906_carrot8848 {

enum class StatisticsSensorType {
  YESTERDAY_ENERGY,
  TODAY_ENERGY,
  WEEK_ENERGY,
  MONTH_ENERGY,
  YEAR_ENERGY,
  YESTERDAY_TOTAL_ENERGY,
  TODAY_TOTAL_ENERGY,
  WEEK_TOTAL_ENERGY,
  MONTH_TOTAL_ENERGY,
  YEAR_TOTAL_ENERGY
};

// 硬件资料
// https://www.belling.com.cn/media/file_object/bel_product/BL0906/guide/BL0906%20APP%20Note_V1.02.pdf
// https://www.belling.com.cn/media/file_object/bel_product/BL0906/datasheet/BL0906_V1.02_cn.pdf

// 寄存器数据为大端序(低-中-高)

class BL0906Carrot8848;
class EnergyStatisticsManager;

struct RawSensorData {
  uint32_t temperature_raw;
  uint32_t frequency_raw;
  uint32_t voltage_raw;

  struct ChannelData {
    uint32_t current_raw;
    int32_t power_raw;
    uint32_t energy_raw;
    bool is_valid;
  } channels[10];

  int32_t power_sum_raw;
  uint32_t energy_sum_raw;
  bool energy_sum_valid;  // false时跳过持久化更新(避免0误判为回绕)

  uint32_t timestamp;
  bool read_complete;
};

struct EnergyPersistenceData {
  uint32_t magic;
  uint32_t schema_version;
  uint32_t instance_id;

  uint32_t persistent_cf_count[11];  // [0-9]通道 [10]总和
  uint32_t last_cf_count[11];
  uint32_t save_count;
  uint32_t checksum;
};

class BL0906Carrot8848 : public PollingComponent {
 public:

  ~BL0906Carrot8848() {
    if (energy_nvs_handle_ != 0) {
      nvs_close(energy_nvs_handle_);
    }
  }

  static const char *const BL0906_CARROT8848_ID;

  enum class State {
    IDLE,
    READ_BASIC_SENSORS,
    READ_CHANNELS,
    READ_TOTAL_DATA,
    PROCESS_PERSISTENCE,
    UPDATE_STATISTICS,
    PUBLISH_SENSORS,
  };

  // 行=SensorType, 列=通道(单通道类型固定列0)
  enum class SensorType {
    VOLTAGE = 0,
    FREQUENCY,
    TEMPERATURE,
    POWER_SUM,
    ENERGY_SUM,
    TOTAL_ENERGY_SUM,
    CURRENT,
    POWER,
    ENERGY,
    TOTAL_ENERGY,
    SENSOR_TYPE_COUNT
  };
  static constexpr int SINGLE_CHANNEL_TYPE_COUNT = 6;
  static constexpr int MAX_SENSOR_CHANNELS = 10;

  void set_sensor(SensorType type, sensor::Sensor *sensor, int channel = 0) {
    int row = static_cast<int>(type);
    int col = (row < SINGLE_CHANNEL_TYPE_COUNT) ? 0 : channel;
    if (col >= 0 && col < MAX_SENSOR_CHANNELS
        && (row < SINGLE_CHANNEL_TYPE_COUNT || is_valid_channel(col))) {
      sensors_[row][col] = sensor;
    }
  }

  sensor::Sensor* get_sensor(SensorType type, int channel = 0) const {
    int row = static_cast<int>(type);
    int col = (row < SINGLE_CHANNEL_TYPE_COUNT) ? 0 : channel;
    if (col < 0 || col >= MAX_SENSOR_CHANNELS) {
      return nullptr;
    }
    if (row >= SINGLE_CHANNEL_TYPE_COUNT && !is_valid_channel(col)) {
      return nullptr;
    }
    return sensors_[row][col];
  }

  void set_channel_sensors(SensorType type, const std::vector<sensor::Sensor*>& sensors) {
    for (size_t i = 0; i < sensors.size() && i < static_cast<size_t>(MAX_SENSOR_CHANNELS); ++i) {
      set_sensor(type, sensors[i], i);
    }
  }

  std::vector<sensor::Sensor*> get_channel_sensors(SensorType type) const {
    std::vector<sensor::Sensor*> result;
    for (int i = 0; i < cached_params_->channel_count; ++i) {
      result.push_back(get_sensor(type, i));
    }
    return result;
  }

  enum class FreqAdaptMode {
    OFF,    // 默认50Hz
    AUTO,   // 自动检测
    HZ60    // 强制60Hz
  };

  void set_chip_model(ChipModel model) {
    chip_model_ = model;
    cached_params_ = &CHIP_PARAMS[static_cast<uint8_t>(model)];
    ESP_LOGI("bl0906_carrot8848", "设置芯片型号: %s", cached_params_->chip_name);
  }

  ChipModel get_chip_model() const { return chip_model_; }
  const ChipParams& get_chip_params() const { return *cached_params_; }

  void loop() override;
  void update();
  void setup() override;
  BL0906Carrot8848();

  int get_max_channels() const { return cached_params_->channel_count; }
  const char* get_chip_name() const { return cached_params_->chip_name; }

  void set_communication_adapter(std::unique_ptr<SpiCommunicationAdapter> adapter);

  bool write_register_value(uint8_t address, int16_t value);  // 供Number组件使用

  float convert_raw_to_value(uint8_t address, int32_t raw_value);  // 唯一数据转换入口

  bool turn_off_write_protect();

  int32_t send_read_command_and_receive(uint8_t address, bool* success);  // 唯一读取入口,success区分失败与0值

  void apply_calibration_values();

  void setup_energy_persistence();
  void save_energy_data();
  void load_energy_data();
  void reset_energy_data();
  void erase_energy_nvs_namespace();  // 仅擦除电量数据,不动校准

  uint32_t calculate_data_checksum(const EnergyPersistenceData& data);
  bool validate_persistence_data(const EnergyPersistenceData& data, const char* context);  // "save"查合理性,"load"加查magic/version/id/checksum
  void load_valid_data(const EnergyPersistenceData& data);

  static bool is_data_sane(const EnergyPersistenceData& data);  // CF_count不应超过0x7FFFFFFF

  void build_nvs_namespace_name(char *buf, size_t buf_size) const;

  uint32_t get_save_count() const { return current_save_count_; }
  void reset_save_count() { current_save_count_ = 0; }

  void set_time_component(esphome_time::RealTimeClock *time_comp);
  void set_statistics_sensor(StatisticsSensorType type, sensor::Sensor *sensor, int channel = 0);
  EnergyStatisticsManager* get_energy_statistics_manager() const { return energy_stats_manager_.get(); }

  void set_calibration_status_text_sensor(text_sensor::TextSensor *sensor) { calibration_status_text_sensor_ = sensor; }
  void update_calibration_status();
  void set_calibration_status(const std::string& status);

  float calculate_total_energy_from_cf_count(int channel) const;

  void diagnose_energy_persistence();
  void diagnose_energy_statistics();

  void set_instance_id(uint32_t id);
  uint32_t get_instance_id() const;
  bool validate_instance_id_for_chip(uint32_t id) const;

  void set_eeprom_type(EEPROMType type) { eeprom_type_ = type; }
  void set_i2c_parent(i2c::I2CBus *parent) { i2c_parent_ = parent; }
  void set_i2c_address(uint8_t address) { i2c_address_ = address; }

  void set_freq_adapt_mode(FreqAdaptMode mode) { freq_adapt_mode_ = mode; }

 protected:
  std::unique_ptr<SpiCommunicationAdapter> comm_adapter_;

  State current_state_{State::IDLE};

  RawSensorData current_data_;
  bool data_collection_complete_;
  uint32_t data_read_start_time_;

  // 逐通道读取,避免单次loop()过长
  int current_read_channel_{0};
  int successful_read_channels_{0};

  sensor::Sensor *sensors_[static_cast<int>(SensorType::SENSOR_TYPE_COUNT)][MAX_SENSOR_CHANNELS]{};

  text_sensor::TextSensor *calibration_status_text_sensor_{nullptr};

  bool is_valid_channel(int channel) const {
    return channel >= 0 && channel < cached_params_->channel_count;
  }

  void process_energy_persistence(const RawSensorData& data);
  void update_energy_statistics(const RawSensorData& data);
  void publish_all_sensors(const RawSensorData& data);

  bool read_all_channels_data();
  bool read_single_channel_data(int channel);

  // 持续通讯失败时降级: loop()停采集,update()探活,成功自愈
  void enter_degraded_mode_();
  bool probe_communication_();
  void publish_sensors_unavailable_();

  bool degraded_mode_{false};
  uint8_t consecutive_failed_cycles_{0};
  static constexpr uint8_t DEGRADED_AFTER_CYCLES = 3;

 public:
  static BL0906Carrot8848 *bl0906_instance;
  static SemaphoreHandle_t global_flash_mutex_;
  static void set_instance(BL0906Carrot8848 *instance) {
    bl0906_instance = instance;
  }

  void init_mutex() {
    if (mutex_ == nullptr) {
      mutex_ = xSemaphoreCreateMutex();
      if (mutex_ == nullptr) {
        ESP_LOGE("bl0906_carrot8848", "Failed to create mutex");
      }
    }
  }

  bool lock(int timeout_ms = 100) {
    if (mutex_ == nullptr) {
      ESP_LOGW("bl0906_carrot8848", "Mutex not initialized");
      return false;
    }
    return xSemaphoreTake(mutex_, timeout_ms / portTICK_PERIOD_MS) == pdTRUE;
  }

  void unlock() {
    if (mutex_ != nullptr) {
      xSemaphoreGive(mutex_);
    }
  }

  SemaphoreHandle_t mutex_{nullptr};

  nvs_handle_t energy_nvs_handle_{0};
  uint32_t persistent_cf_count_[11]{0};  // 软件累计CF, [10]总和
  uint32_t last_cf_count_[11]{0};        // 上次硬件CF
  uint32_t last_save_time_{0};
  uint32_t current_save_count_{0};
  bool energy_persistence_enabled_{true};
  bool last_cf_count_needs_save_{false};  // 仅芯片重启后为true
  bool energy_persistence_initialized_{false};

  std::unique_ptr<EnergyStatisticsManager> energy_stats_manager_;
  bool energy_statistics_enabled_{true};
  esphome_time::RealTimeClock *time_component_{nullptr};

  // 缓存manager创建前的set调用,setup()统一apply
  struct PendingStatSensor {
    StatisticsSensorType type;
    sensor::Sensor *sensor;
    int channel;
  };
  std::vector<PendingStatSensor> pending_stat_sensors_;

  uint32_t saved_total_cf_{0};

 private:
  ChipModel chip_model_{ChipModel::BL0906};
  const ChipParams* cached_params_{&CHIP_PARAMS[0]};

  uint32_t instance_id_{0};

  EEPROMType eeprom_type_{EEPROMType::TYPE_24C02};
  i2c::I2CBus *i2c_parent_{nullptr};
  uint8_t i2c_address_{0x50};

  std::unique_ptr<I2CEEPROMCalibrationStorage> calibration_storage_;

  bool init_calibration_storage();
  bool load_calibration_data();

  FreqAdaptMode freq_adapt_mode_{FreqAdaptMode::OFF};
  bool freq_adapted_{false};

  static constexpr float FREQ_DETECT_THRESHOLD_LOW = 55.0f;
  static constexpr float FREQ_DETECT_THRESHOLD_HIGH = 65.0f;
  static constexpr uint8_t FREQ_DETECTION_SAMPLES = 3;

  static constexpr uint32_t ENERGY_PERSIST_MAGIC = 0x4E52474E;  // "NRGN"
  static constexpr uint32_t ENERGY_PERSIST_SCHEMA_VERSION = 2;

  static constexpr uint32_t CRITICAL_HEAP_THRESHOLD = 15360;  // 15KB
  static constexpr uint32_t WARNING_HEAP_THRESHOLD = 20480;   // 20KB
  static constexpr uint32_t SAFE_HEAP_THRESHOLD = 30720;      // 30KB

 private:
  float detect_grid_frequency();
  bool set_ac_frequency_mode(bool is_60hz);
  uint32_t read_mode2_register();
  bool write_mode2_register(uint32_t value);

  bool write_register_24bit(uint8_t address, uint32_t value);
};

}  // namespace bl0906_carrot8848
}  // namespace esphome
