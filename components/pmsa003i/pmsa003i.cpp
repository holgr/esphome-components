#include "pmsa003i.h"
#include "esphome/core/log.h"
#include <cstring>

namespace esphome {
namespace pmsa003i {

static const char *const TAG = "pmsa003i";

void PMSA003IComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up pmsa003i...");

  PM25AQIData data;
  bool successful_read = this->read_data_(&data);

  if (!successful_read) {
    this->mark_failed();
    return;
  }
}

void PMSA003IComponent::dump_config() {
  LOG_I2C_DEVICE(this);

  LOG_SENSOR("  ", "PM1.0", this->pm_1_0_sensor_);
  LOG_SENSOR("  ", "PM2.5", this->pm_2_5_sensor_);
  LOG_SENSOR("  ", "PM10.0", this->pm_10_0_sensor_);
  LOG_SENSOR("  ", "AQI", this->aqi_sensor_);
}

void PMSA003IComponent::update() {
  PM25AQIData data;

  bool successful_read = this->read_data_(&data);

  // Update sensors
  if (successful_read) {
    this->status_clear_warning();
    ESP_LOGV(TAG, "Read success. Updating sensors.");
    // int16_t pm_1_0_value = -1;
    int16_t pm_2_5_value = -1;
    int16_t pm_10_0_value = -1;

    if (this->standard_units_) {
      if (this->pm_1_0_sensor_ != nullptr) {
        this->pm_1_0_sensor_->publish_state(data.pm10_standard);
      }
      if (this->pm_2_5_sensor_ != nullptr) {
        this->pm_2_5_sensor_->publish_state(data.pm25_standard);
        pm_2_5_value = this->pm_2_5_sensor_->get_state();
      }
      if (this->pm_10_0_sensor_ != nullptr) {
        this->pm_10_0_sensor_->publish_state(data.pm100_standard);
        pm_10_0_value = this->pm_10_0_sensor_->get_state();
      }
    } else {
      if (this->pm_1_0_sensor_ != nullptr) {
        this->pm_1_0_sensor_->publish_state(data.pm10_env);
      }
      if (this->pm_2_5_sensor_ != nullptr) {
        this->pm_2_5_sensor_->publish_state(data.pm25_env);
        pm_2_5_value = this->pm_2_5_sensor_->get_state();
      }
      if (this->pm_10_0_sensor_ != nullptr) {
        this->pm_10_0_sensor_->publish_state(data.pm100_env);
        pm_10_0_value = this->pm_10_0_sensor_->get_state();
      }
    }

    /*
    if (this->pm_1_0_sensor_ != nullptr) {
      pm_1_0_value = get_sensor_value_(data_buffer_, PM_1_0_VALUE_INDEX);
    }

    if (this->pm_2_5_sensor_ != nullptr) {
      pm_2_5_value = get_sensor_value_(data_buffer_, PM_2_5_VALUE_INDEX);
    }

    if (this->pm_10_0_sensor_ != nullptr) {
      pm_10_0_value = get_sensor_value_(data_buffer_, PM_10_0_VALUE_INDEX);
    }
    */

    uint16_t aqi_value = -1;
    if (this->aqi_sensor_ != nullptr && pm_2_5_value != -1 && pm_10_0_value != -1) {
      AbstractAQICalculator *calculator = this->aqi_calculator_factory_.get_calculator(this->aqi_calc_type_);
      aqi_value = calculator->get_aqi(pm_2_5_value, pm_10_0_value);
    }

    if (this->pmc_0_3_sensor_ != nullptr)
      this->pmc_0_3_sensor_->publish_state(data.particles_03um);
    if (this->pmc_0_5_sensor_ != nullptr)
      this->pmc_0_5_sensor_->publish_state(data.particles_05um);
    if (this->pmc_1_0_sensor_ != nullptr)
      this->pmc_1_0_sensor_->publish_state(data.particles_10um);
    if (this->pmc_2_5_sensor_ != nullptr)
      this->pmc_2_5_sensor_->publish_state(data.particles_25um);
    if (this->pmc_5_0_sensor_ != nullptr)
      this->pmc_5_0_sensor_->publish_state(data.particles_50um);
    if (this->pmc_10_0_sensor_ != nullptr)
      this->pmc_10_0_sensor_->publish_state(data.particles_100um);

    if (aqi_value != -1)
      this->aqi_sensor_->publish_state(aqi_value);

  } else {
    this->status_set_warning();
    ESP_LOGV(TAG, "Read failure. Skipping update.");
  }
}

bool PMSA003IComponent::read_data_(PM25AQIData *data) {
  const uint8_t num_bytes = 32;
  uint8_t buffer[num_bytes];

  this->read_bytes_raw(buffer, num_bytes);

  // https://github.com/adafruit/Adafruit_PM25AQI

  // Check that start byte is correct!
  if (buffer[0] != 0x42) {
    return false;
  }

  // get checksum ready
  int16_t sum = 0;
  for (uint8_t i = 0; i < 30; i++) {
    sum += buffer[i];
  }

  // The data comes in endian'd, this solves it so it works on all platforms
  uint16_t buffer_u16[15];
  for (uint8_t i = 0; i < 15; i++) {
    buffer_u16[i] = buffer[2 + i * 2 + 1];
    buffer_u16[i] += (buffer[2 + i * 2] << 8);
  }

  // put it into a nice struct :)
  memcpy((void *) data, (void *) buffer_u16, 30);

  return (sum == data->checksum);
}

}  // namespace pmsa003i
}  // namespace esphome
