#pragma once

namespace esphome {
namespace pmsa003i {

class AbstractAQICalculator {
 public:
  virtual uint16_t get_aqi(uint16_t pm2_5_value, uint16_t pm10_0_value) = 0;
};

}  // namespace pmsa003i
}  // namespace esphome
