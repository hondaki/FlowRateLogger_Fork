//#include <M5Core2.h>
#include <WiFi.h>
#include <time.h>

#define WIFI_TRIAL_COUNT 30
#define NTP_TRIAL_COUNT 30

const char* ssid     = "SSID";
const char* password = "PASS";

//const char* ntpServer = "NTP.th.net";
//const char* ntpServer = "time.thai.net";
//const char* ntpServer = "pool.ntp.in.th";
//const char* ntpServer = "time1.aliyun.com";
const char* ntpServer = "ntp.nict.jp";

const long gmtOffset_sec     = 7 * 3600L; // Thailand
const int daylightOffset_sec = 0;


void printLocalTime() {  // Output current time.  输出当前时间
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {  // Return 1 when the time is successfully
                                     // obtained.  成功获取到时间返回1
        M5.Lcd.println("Failed to obtain time");
        return;
    }
    M5.Lcd.println(&timeinfo,
                   "%A, %B %d \n%Y %H:%M:%S");  // Screen prints date and time.
                                                // 屏幕打印日期和时间
}


// Reset system time vit RTC
// For time stamp of SD files.
void setup_time() {
  RTC_DateTypeDef date;
  RTC_TimeTypeDef time;
  M5.Rtc.GetDate(&date);
  M5.Rtc.GetTime(&time);
  M5.Rtc.SetDate(&date);
  M5.Rtc.SetTime(&time);
}


void update_time() {

  M5.Lcd.clear(BLACK);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("\nConnecting to %s", ssid);

  WiFi.begin(ssid, password);
  for (int i = 0; i < WIFI_TRIAL_COUNT; i++) {
    if (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      M5.Lcd.print(".");
    }
  }

  if (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.println("\nWi-Fi Connection Failed!");
    delay(2000);
    return;
  }

  M5.Lcd.println("\nWi-Fi Connected!");

  M5.Lcd.println("Connect NTP.");

  struct tm timeinfo;
  for (int i = 0; i < NTP_TRIAL_COUNT; i++) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    if (getLocalTime(&timeinfo)) {
      break;
    }

    delay(1000);
    M5.Lcd.print(".");
  }

  if (!getLocalTime(&timeinfo)) {
    M5.Lcd.println("\nNTP Connection Failed!");
    delay(2000);
    return;
  }
  else {
    // Set to RTC
    RTC_DateTypeDef date;
    date.WeekDay = timeinfo.tm_wday;
    date.Month = timeinfo.tm_mon + 1;
    date.Date = timeinfo.tm_mday;
    date.Year = timeinfo.tm_year + 1900;
    if (!M5.Rtc.SetDate(&date)) Serial.println("wrong date set!");

    RTC_TimeTypeDef time;
    time.Hours   = timeinfo.tm_hour;
    time.Minutes = timeinfo.tm_min;
    time.Seconds = timeinfo.tm_sec;
    if (!M5.Rtc.SetTime(&time)) Serial.println("wrong time set!");
  }

  printLocalTime();

  WiFi.disconnect(true);  // Disconnect wifi.  断开wifi连接
  WiFi.mode(WIFI_OFF);  // Set the wifi mode to off.  设置wifi模式为关闭
  delay(100);

  M5.Lcd.println("Time Updated!");
  delay(2000);
}
