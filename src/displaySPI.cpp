#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>

#include "displaySPI.h"
#include "config.h"
#include "ride_time.h"

static bool first_start = true;

U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(
  U8G2_R0,
  SPI_CS_PIN,
  SPI_DC_PIN, 
  SPI_RES_PIN
);

void display_init()
{
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setPowerSave(0);

  if (first_start)
  {
  display_start_screen();
  delay(4000);
  first_start = false;
  }
}

// Sets Display
void display_OLED(float    speed,
                  float    distance,
                  RideTime time,
                  float    cal_per_hour,
                  float    cal_burned,
                  float    lifetime_mi)
{
    u8g2.clearBuffer();

    u8g2.setCursor(0, 12);
    u8g2.print(speed, 1);
    u8g2.print(" MPH");

    u8g2.setCursor(0,24);
    u8g2.print(distance, 2);
    u8g2.print(" MI");

    u8g2.setCursor(0, 36);
    char timeString[10];
    snprintf(
        timeString, sizeof(timeString), "%02u:%02u:%02u", time.hours, time.minutes, time.seconds);
    u8g2.print(timeString);

    u8g2.setCursor(0, 48);
    u8g2.print(static_cast<uint16_t>(cal_per_hour));
    u8g2.print(" CAL/HR");
    
    u8g2.setCursor(0, 60);
    u8g2.print(static_cast<uint16_t>(cal_burned));
    u8g2.print(" CAL");

    u8g2.sendBuffer();
}

void display_eeprom_corrupt()
{
  u8g2.clearBuffer();

  u8g2.setCursor(20, 26);
  u8g2.print(" EEPROM CORRUPTED");
  u8g2.setCursor(20, 40);
  u8g2.print("ODOMETER RESET");

  u8g2.sendBuffer();
}

void display_off()
{
  u8g2.setPowerSave(1);
}

void display_on()
{
  u8g2.setPowerSave(0);
}

void display_start_screen()
{
  u8g2.clearBuffer();

  // --- Bike geometry (centered, slightly narrower) ---
  const uint8_t bike_center_x = 64;
  const uint8_t wy = 30;
  const uint8_t wr = 9;

  // Slightly shorter wheelbase (was 36)
  const uint8_t wheel_spacing = 32;
  const uint8_t wx1 = bike_center_x - wheel_spacing / 2; // rear wheel
  const uint8_t wx2 = bike_center_x + wheel_spacing / 2; // front wheel

  // Wheels
  u8g2.drawCircle(wx1, wy, wr);
  u8g2.drawCircle(wx2, wy, wr);

  // Hubs
  u8g2.drawCircle(wx1, wy, 2);
  u8g2.drawCircle(wx2, wy, 2);

  // Frame points (minor proportional tweak)
  const uint8_t bbx   = bike_center_x;
  const uint8_t bby   = wy;
  const uint8_t seatx = wx1 + 9;
  const uint8_t seaty = wy - 10;
  const uint8_t headx = wx2;
  const uint8_t heady = wy - 14;

  // Frame
  u8g2.drawLine(wx1, wy, bbx, bby);          // chainstay
  u8g2.drawLine(bbx, bby, wx2, wy);          // down tube
  u8g2.drawLine(bbx, bby, seatx, seaty);     // seat tube
  u8g2.drawLine(seatx, seaty, headx, heady); // top tube
  u8g2.drawLine(headx, heady, wx2, wy);      // fork

  // Crank
  u8g2.drawCircle(bbx, bby, 3);
  u8g2.drawLine(bbx, bby, bbx + 5, bby + 2);

  // Seat
  u8g2.drawLine(seatx, seaty, seatx, seaty - 4);
  u8g2.drawLine(seatx - 5, seaty - 5, seatx + 5, seaty - 5);

  // Handlebars (wide but compact)
  const uint8_t bar_y = heady - 3;
  u8g2.drawLine(headx - 7, bar_y, headx + 7, bar_y);
  u8g2.drawLine(headx, heady, headx, bar_y);

  // --- Title ---
  u8g2.setFont(u8g2_font_7x13_tf);
  const char* title = "BIKE SPEEDO";
  uint8_t title_x = (128 - u8g2.getStrWidth(title)) / 2;
  u8g2.setCursor(title_x, 56);
  u8g2.print(title);

  u8g2.sendBuffer();
}
