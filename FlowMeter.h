#pragma once

#include "driver/pcnt.h"

#define PULSE_PIN_CLK_0 SCL
#define PULSE_PIN_CLK_1 SDA

#define COUNTER_MIN 0
#define COUNTER_MAX 32767

#define FLOW_RATE_PER_PULSE 0.46 // mL


typedef struct flow_meter_type {
  pcnt_unit_t unit;
  int16_t count;
  int16_t count_diff;
  int16_t last_count;
  uint32_t total_count = 0;

  double flow_rate;
  double total_flow = 0;
  uint32_t no_flow_count = 0;
} flow_meter;

// Added protypes by KH 2025-09-22
void setup_flow_meters();
void setup_flow_meter(flow_meter *meter, uint8_t pin, pcnt_unit_t unit);
void update_flow_meters();
void update_flow_meter(flow_meter *fm);
void reset_flow_meters();

extern flow_meter flow_meters[2];

