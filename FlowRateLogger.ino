#include <M5Core2.h>
#include <SPI.h>
#include <SD.h>

// Flow Meter

#include "FlowMeter.h"

#define NO_FLOW_COUNT_THRESHOLD 300 // 5 min
//#define NO_FLOW_COUNT_THRESHOLD 10 // 10 sec for debug

#define UPDATE_INTERVAL	60 // 60sec = 1 min

// RTC

RTC_DateTypeDef rtc_date;
RTC_TimeTypeDef rtc_time;


// SD

bool log_started = false;
bool no_sd_card = false;
File log_file;


// Serial output

bool output_logdata_to_serial = true;


// Set up

void setup() {
  M5.begin();

  // Screen

  M5.Axp.ScreenBreath(10); // Reduce Power Consumption of Display
  M5.Lcd.setTextSize(2);  // テキストサイズ指定
  M5.Lcd.setCursor(0, 0);  // カーソル位置を設定

  // Peripherals

  setup_flow_meters();
  reset_flow_meters(); // added by KH 2025-09-22
  setup_time();
}


// Main Loop

void loop() {
  do_command_from_serial();
  update_flow_meters();

  M5.Rtc.GetDate(&rtc_date);
  M5.Rtc.GetTime(&rtc_time); 

  handle_log();
  show_status();

  //delay(5000);  //5sec
  delay( 1000*UPDATE_INTERVAL );  //1min as UPDATE_INTERVAL is 60
}


// Receive Commands via Serial Port

void do_command_from_serial() {
  if ( Serial.available() > 0 ) {
    String str = Serial.readStringUntil('\n');
    if (str.equalsIgnoreCase("ut")) {
      update_time();
    }
    else if (str.equalsIgnoreCase("updatetime")) {
      update_time();
    }
    if (str.equalsIgnoreCase("so")) {
      // Toggle serial output of log data
      output_logdata_to_serial = !output_logdata_to_serial;
    }
    else if (str.equalsIgnoreCase("serialout")) {
      // Toggle serial output of log data
      output_logdata_to_serial = !output_logdata_to_serial;
    }
  }
}


// Handle Log Data

void handle_log() {
  if (flow_meters[0].count_diff == 0 && flow_meters[1].count_diff == 0 && !log_started) {
    // Do nothing before first logging started
    // Serial.println("Waiting start.");
    no_sd_card = false;
  }
  else if (NO_FLOW_COUNT_THRESHOLD < flow_meters[0].no_flow_count && NO_FLOW_COUNT_THRESHOLD < flow_meters[1].no_flow_count) {
    if (log_file != NULL) {
      // Close Log File and Wait Next Flow Data.
      log_file.close(); 
      log_file = (File)NULL;
      Serial.println("Log file closed.");
      reset_flow_meters();  // Added by KH reset totals as next recording will be in the new file.  2025-09-22
    }
    no_sd_card = false;
  }
  else {
    // Create File if not exist
    if (log_file == NULL) {
      char filename[60];
      sprintf(filename, "/%04d%02d%02d_%02d%02d%02d%02d_flowrate.csv"
        , rtc_date.Year, rtc_date.Month, rtc_date.Date
        , rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, 0
        );
      log_file = SD.open(filename, FILE_WRITE); 

      if (log_file != NULL) {
        log_started = true;
        // reset_flow_meters(); // void by KH. Reset happens when the file is closed.  2025-09-22
        Serial.println("Log file created.");
      }
    }
  
    // Make log data
    char logdata[200]; // 15 + 10 * 8
    sprintf(logdata, "$FLOWRATE,%04d%02d%02d,%02d%02d%02d%02d,%d,%d,%.2f,%.2f,%d,%d,%.2f,%.2f\n"
        , rtc_date.Year, rtc_date.Month, rtc_date.Date
        , rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, 0
        , flow_meters[0].count_diff, flow_meters[0].total_count
        , flow_meters[0].flow_rate, flow_meters[0].total_flow
        , flow_meters[1].count_diff, flow_meters[1].total_count
        , flow_meters[1].flow_rate, flow_meters[1].total_flow
        );

    // Write flow rate data to SD
    if (log_file != NULL) {    
      log_file.printf(logdata);
      log_file.flush();
      no_sd_card = false;
    }
    else {
      no_sd_card = true;
    }

    // Output to Serial
    if (output_logdata_to_serial) {
      Serial.print(logdata);
    }
  }
}


// Show status on display

void show_status() {

  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.setTextColor(WHITE, BLACK);

  char datetime[50];
  sprintf(datetime, "%04d/%02d/%02d %02d:%02d:%02d.%02d\n\n"
  , rtc_date.Year, rtc_date.Month, rtc_date.Date
  , rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, 0
  );
  M5.Lcd.printf(datetime);

  M5.Lcd.printf("Port 1(SCL)\n");
  M5.Lcd.printf("Count/%2dSec  : %d\n", UPDATE_INTERVAL, flow_meters[0].count_diff);
  M5.Lcd.printf("Total Count  : %d\n", flow_meters[0].total_count);
  M5.Lcd.printf("F-Rate mL/%2dSec  : %.2f\n", UPDATE_INTERVAL, flow_meters[0].flow_rate );
  M5.Lcd.printf("Total Flow L : %.5f\n\n", flow_meters[0].total_flow / 1000);

  M5.Lcd.printf("Port 2(SDA)\n");
  M5.Lcd.printf("Count /%2dSec  : %d\n", UPDATE_INTERVAL, flow_meters[1].count_diff);
  M5.Lcd.printf("Total Count  : %d\n", flow_meters[1].total_count);
  M5.Lcd.printf("F-Rate ml/%2dSec   : %.2f\n", UPDATE_INTERVAL, flow_meters[1].flow_rate );
  M5.Lcd.printf("Total Flow L : %.5f\n\n", flow_meters[1].total_flow / 1000);
  
  if (log_file != NULL) {
    M5.Lcd.setTextColor(RED, DARKGREY);
    M5.Lcd.printf("Data Logging\n");
  }
  else {
    if (no_sd_card) {
      M5.Lcd.setTextColor(YELLOW, RED);
      M5.Lcd.printf("NO SD CARD!\n");
    }
    else {
      M5.Lcd.setTextColor(BLUE, DARKGREY);
      M5.Lcd.printf("Waiting Data\n");
    }
  }
}
