//#include <M5Core2.h>
#include "FlowMeter.h"

flow_meter flow_meters[2];


void setup_flow_meters() {
  setup_flow_meter(&flow_meters[0], SCL, PCNT_UNIT_0);
  setup_flow_meter(&flow_meters[1], SDA, PCNT_UNIT_1);
}


void setup_flow_meter(flow_meter *meter, uint8_t pin, pcnt_unit_t unit) {
  pinMode(pin, INPUT);

  pcnt_config_t pcnt_config = {};
  pcnt_config.pulse_gpio_num  = pin;
  pcnt_config.ctrl_gpio_num   = PCNT_PIN_NOT_USED;
  pcnt_config.lctrl_mode      = PCNT_MODE_KEEP;
  pcnt_config.hctrl_mode      = PCNT_MODE_KEEP;
  pcnt_config.pos_mode        = PCNT_COUNT_INC;
  pcnt_config.neg_mode        = PCNT_COUNT_DIS;
  pcnt_config.counter_h_lim   = COUNTER_MAX;//32767;
  pcnt_config.counter_l_lim   = COUNTER_MIN;//-32768;
  pcnt_config.unit            = unit;
  pcnt_config.channel         = PCNT_CHANNEL_0;

  pcnt_unit_config(&pcnt_config);

  pcnt_counter_pause(unit);
  pcnt_counter_clear(unit);
  pcnt_counter_resume(unit);

  meter->unit = unit;
}


void reset_flow_meters() {
  flow_meters[0].total_count = 0;
  flow_meters[0].total_flow = 0;
  flow_meters[0].last_count = 0;

  flow_meters[1].total_count = 0;
  flow_meters[1].total_flow = 0;
  flow_meters[1].last_count = 0;
}


void update_flow_meters() {
  update_flow_meter(&flow_meters[0]);
  update_flow_meter(&flow_meters[1]);
}


void update_flow_meter(flow_meter *fm) {
  pcnt_get_counter_value(fm->unit, &fm->count);
  fm->count_diff = fm->count - fm->last_count;

  if(fm->count_diff == 0) {
    // last_count == count
    fm->flow_rate = 0;
    fm->no_flow_count++;
  }
  else {
    if(fm->count_diff < 0) {
      fm->count_diff += COUNTER_MAX;
    }
    fm->total_count += fm->count_diff;
    fm->last_count = fm->count;

    fm->flow_rate = (double)fm->count_diff * FLOW_RATE_PER_PULSE;
    fm->total_flow += fm->flow_rate;

    fm->no_flow_count = 0;
  }
}