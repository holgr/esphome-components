# esphome-components
A collection of my ESPHome components

## pmsa003i with AQI calculations

This includes AQI calculations for the pmsa003i sensor. It's basically a
port from the existing hm3301 component and combining it with the pmsa003i
parts.

Example configuration: 
```# Example configuration entry sensor:
  - platform: pmsa003i
    pm_1_0:
      name: "PM1.0"
    pm_2_5:
      name: "PM2.5"
    pm_10_0:
      name: "PM10.0"
    pmc_0_3:
      name: "PMC <0.3µm"
    pmc_0_5:
      name: "PMC <0.5µm"
    pmc_1_0:
      name: "PMC <1µm"
    pmc_2_5:
      name: "PMC <2.5µm"
    pmc_5_0:
      name: "PMC <5µm"
    pmc_10_0:
      name: "PMC <10µm"
    aqi:
      name: "AQI"
      calculation_type: "AQI"
```