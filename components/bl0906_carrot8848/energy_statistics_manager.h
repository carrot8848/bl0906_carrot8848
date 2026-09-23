#pragma once

#include "esphome/core/component.h"
#include "esphome/core/time.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/log.h"
#include <atomic>
#include <ctime>
#include <mutex>
#include "nvs_flash.h"

#include "bl0906_api.h"

namespace esphome_time = esphome::time;

namespace esphome {
namespace bl0906_carrot8848 {

class BL0906Carrot8848;
enum class StatisticsSensorType;

static constexpr uint8_t MAX_TOTAL_CHANNELS = 11;  // 10通道 + 总和

enum class EnergyPeriod {
  YESTERDAY,
  TODAY,
  THIS_WEEK,
  THIS_MONTH,
  THIS_YEAR
};

struct CompactTimeSnapshot {
  time_t timestamp;
  uint16_t year;
  uint8_t month;
  uint8_t day_of_month;
  uint8_t day_of_week;
};

struct OptimizedEnergyStatistics {
  CompactTimeSnapshot period_times[5];  // [0]昨日 [1]今日 [2]周 [3]月 [4]年
  uint32_t period_persistent_cf_count[MAX_TOTAL_CHANNELS][5];  // [0]昨日差值 [1..4]周期起点CF
  uint32_t current_persistent_cf_count[MAX_TOTAL_CHANNELS];
  time_t last_update_timestamp;
  uint32_t checksum;
  bool updating;
};

class BL0906_API EnergyStatisticsManager : public PollingComponent {
public:
  EnergyStatisticsManager(BL0906Carrot8848* parent);

  static constexpr uint32_t STATS_PERSIST_MAGIC = 0x53544154;  // "STAT"
  static constexpr uint32_t STATS_PERSIST_SCHEMA_VERSION = 2;

  void setup() override;
  void set_time_component(esphome_time::RealTimeClock *time_comp);
  void set_update_interval(uint32_t update_interval) { PollingComponent::set_update_interval(update_interval); }

  void update_persistent_cf_count(int channel, uint32_t current_cf_count);
  void check_period_changes();
  void update() override;

  void set_sensor(StatisticsSensorType type, sensor::Sensor *sensor, int channel = 0);

  void diagnose_energy_statistics();

private:
  BL0906Carrot8848* parent_;
  esphome_time::RealTimeClock *time_component_{nullptr};

  mutable std::mutex statistics_mutex_;
  OptimizedEnergyStatistics statistics_{};

  sensor::Sensor *yesterday_energy_sensors_[MAX_TOTAL_CHANNELS]{};
  sensor::Sensor *today_energy_sensors_[MAX_TOTAL_CHANNELS]{};
  sensor::Sensor *week_energy_sensors_[MAX_TOTAL_CHANNELS]{};
  sensor::Sensor *month_energy_sensors_[MAX_TOTAL_CHANNELS]{};
  sensor::Sensor *year_energy_sensors_[MAX_TOTAL_CHANNELS]{};

  std::atomic<uint32_t> current_persistent_cf_count_[MAX_TOTAL_CHANNELS]{};

  std::atomic<bool> time_initialized_{false};
  std::atomic<time_t> last_check_timestamp_{0};
  uint16_t last_check_year_{0};  // 本地日期字段(非UTC)
  uint8_t last_check_month_{0};
  uint8_t last_check_day_of_month_{0};
  uint8_t last_check_day_of_week_{0};
  int16_t last_check_day_of_year_{0};
  std::atomic<bool> snapshot_dirty_{false};
  std::atomic<bool> cf_count_ready_{false};  // CF原子值就绪前禁止发布(避免回绕误判)
  std::atomic<bool> snapshot_loaded_from_nvs_{false};  // 快照是否已从NVS加载

  nvs_handle_t stats_nvs_handle_{0};  // 独立NVS namespace
  char stats_nvs_namespace_[16]{};
  bool stats_nvs_opened_{false};

  ESPTime get_current_time() const;
  bool is_time_valid() const;
  CompactTimeSnapshot create_time_snapshot(const ESPTime &time) const;

  // 掩码可叠加;跨周/月/年时必跨日
  enum PeriodChangeFlag : uint8_t {
    PERIOD_NONE  = 0,
    PERIOD_DAY   = 1 << 0,
    PERIOD_WEEK  = 1 << 1,
    PERIOD_MONTH = 1 << 2,
    PERIOD_YEAR  = 1 << 3,
  };
  uint8_t detect_period_change(const ESPTime &last_time, const ESPTime &current_time) const;

  void handle_new_day();
  void handle_period_transition(int period_index, const char* period_name);

  void create_current_snapshot_safe();
  void time_related_initialization();

  bool open_stats_nvs();
  void close_stats_nvs();
  bool save_statistics_snapshot();
  bool load_statistics_snapshot();
  void build_stats_nvs_namespace(char *buf, size_t buf_size) const;
  uint32_t calculate_stats_checksum(const uint8_t *data, size_t size) const;

  float calculate_energy_for_period(int channel, EnergyPeriod period) const;
  float calculate_total_energy_for_period(EnergyPeriod period) const;
};

}  // namespace bl0906_carrot8848
}  // namespace esphome
