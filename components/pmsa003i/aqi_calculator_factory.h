#pragma once

#include "caqi_calculator.h"
#include "aqi_calculator.h"

namespace esphome {
namespace pmsa003i {

enum AQICalculatorType { CAQI_TYPE = 0, AQI_TYPE = 1 };

class AQICalculatorFactory {
 public:
  AbstractAQICalculator *get_calculator(AQICalculatorType type) {
    if (type == 0) {
      return caqi_calculator_;
    } else if (type == 1) {
      return aqi_calculator_;
    }

    return nullptr;
  }

 protected:
  CAQICalculator *caqi_calculator_ = new CAQICalculator();
  AQICalculator *aqi_calculator_ = new AQICalculator();
};

}  // namespace pmsa003i
}  // namespace esphome
