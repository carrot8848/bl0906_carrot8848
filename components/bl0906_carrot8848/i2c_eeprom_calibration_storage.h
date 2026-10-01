#pragma once
#include "esphome/components/i2c/i2c.h"
#include <string>
#include <vector>

#include "bl0906_api.h"

namespace esphome {
namespace bl0906_carrot8848 {

struct CalibrationEntry {
  uint8_t register_addr;
  int16_t value;
} __attribute__((packed));

enum class StorageResult {
  SUCCESS,
  INSTANCE_NOT_FOUND,
  STORAGE_FULL,
  INVALID_DATA,
  IO_ERROR,
  VERIFICATION_FAILED
};

enum class EEPROMType : uint8_t {
  TYPE_24C02 = 0x02,  // 256字节
  TYPE_24C04 = 0x04,  // 512字节
  TYPE_24C08 = 0x08,  // 1024字节
  TYPE_24C16 = 0x16   // 2048字节
};

struct EEPROMHeader {
  uint32_t magic;        
  uint16_t version;
  uint8_t eeprom_type;
  uint8_t max_instances;
  uint8_t instance_count;
  uint16_t header_crc;
  uint32_t data_crc;
  uint32_t timestamp;
  uint8_t reserved;
} __attribute__((packed));

class BL0906_API I2CEEPROMCalibrationStorage {
public:
  I2CEEPROMCalibrationStorage(i2c::I2CBus *i2c, EEPROMType type, uint8_t address = 0x50);

  bool init();
  bool read_instance(uint32_t instance_id, std::vector<CalibrationEntry>& entries);

private:
  i2c::I2CBus *i2c_;
  uint8_t address_;
  EEPROMType eeprom_type_;
  size_t eeprom_size_;
  size_t max_instances_;
  size_t entries_per_instance_;

  bool read_bytes(uint16_t addr, uint8_t *data, size_t len);

  uint32_t get_magic_for_type(EEPROMType type);
  bool validate_instance_id(uint32_t instance_id) const;
  bool validate_entries(const std::vector<CalibrationEntry>& entries) const;
  StorageResult validate_entries_count(size_t count, size_t max_count) const;
  bool deserialize_entries(const uint8_t* buffer, size_t buffer_size, std::vector<CalibrationEntry>& entries) const;

  void calculate_layout();
  uint16_t get_instance_offset(int instance_index);
  int find_instance_index(uint32_t instance_id);

  bool read_header(EEPROMHeader& header);

  StorageResult read_raw_data(uint32_t instance_id, uint8_t* buffer, size_t& buffer_size);

  void log_storage_error(const char* operation, StorageResult result, const char* details = nullptr) const;
  void log_data_validation_error(const char* field, uint32_t value, uint32_t max_value) const;
  void log_instance_operation(const char* operation, uint32_t instance_id, bool success, const char* details = nullptr) const;
  bool check_data_integrity(const uint8_t* data, size_t size) const;

  static constexpr size_t ENTRY_SERIALIZED_SIZE = 3;
  static constexpr uint32_t MIN_VALID_INSTANCE_ID = 0x00000001;
  static constexpr uint32_t MAX_VALID_INSTANCE_ID = 0xFFFFFFFE;

  static const char *const TAG;
};

}  // namespace bl0906_carrot8848
}  // namespace esphome
