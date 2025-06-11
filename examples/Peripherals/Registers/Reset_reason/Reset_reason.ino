/* Last Reset Reason Sketch
* This sketch will determine what caused the last reset on the STM32 MCU. Most microcontrollers
* have a register dedicated to storing the last reason of the chip, weather beeing from a 
* low power condition, software caused brown-out. Test it by resetting the MCU via the USER button
* which triggers the Reset_my_MCU() function or unplug the USB cable and repluggit back. Adjust your 
* UART, USER Button pin and registers accordingly. Use the MCU's datasheet and/or stm32yyyxxx.h for reference.
* The code is provided "as is" with no liability.
*/

#include "stm32yyxx_ll_rcc.h"
#include "IWatchdog.h"

#define USER_BTN_PIN USER_BTN  // Adjust this for your board

// Enumerator for combining reset flag bits into one byte then display them
enum reset_reason {
  UNKNOWN_RESET          = 0,
  BROWN_OUT              = 1 << 0,
  NRST_PIN               = 1 << 1,
  SOFTWARE_RST           = 1 << 2,
  INDEPENDENT_WDG        = 1 << 3,
  WINDOW_WDG             = 1 << 4,
  LOW_POWER              = 1 << 5,
  OPTION_BYTE_LOADER     = 1 << 6
};

reset_reason last_reset_reason = UNKNOWN_RESET;

void Reset_My_MCU() { //There are a few reset conditions. Keep the one you wish to use and comment out the others.
  // Bellow is the Software reset condition
  //NVIC_SystemReset(); 

  // Bellow is the Watchdog Timer reset condition
  IWatchdog.begin(1000); //1ms tick then reset
  while (1);  // Wait for reset
}

void setup() {
  pinMode(USER_BTN_PIN, INPUT_PULLDOWN);

  Serial.begin(115200);
  while (!Serial);  // Wait for Serial

  if (LL_RCC_IsActiveFlag_BORRST())     last_reset_reason = (reset_reason)(last_reset_reason | BROWN_OUT);
  if (LL_RCC_IsActiveFlag_PINRST())     last_reset_reason = (reset_reason)(last_reset_reason | NRST_PIN);
  if (LL_RCC_IsActiveFlag_SFTRST())     last_reset_reason = (reset_reason)(last_reset_reason | SOFTWARE_RST);
  if (LL_RCC_IsActiveFlag_IWDGRST())    last_reset_reason = (reset_reason)(last_reset_reason | INDEPENDENT_WDG);
  if (LL_RCC_IsActiveFlag_WWDGRST())    last_reset_reason = (reset_reason)(last_reset_reason | WINDOW_WDG);
  if (LL_RCC_IsActiveFlag_LPWRRST())    last_reset_reason = (reset_reason)(last_reset_reason | LOW_POWER);
  if (LL_RCC_IsActiveFlag_OBLRST())     last_reset_reason = (reset_reason)(last_reset_reason | OPTION_BYTE_LOADER);

  // Clear reset flags
  LL_RCC_ClearResetFlags();
}

void loop() {
  Serial.println("Last reset reason:");

  if (last_reset_reason & BROWN_OUT)          Serial.println(" - Brown-out reset");
  if (last_reset_reason & SOFTWARE_RST)       Serial.println(" - Software reset");
  if (last_reset_reason & INDEPENDENT_WDG)    Serial.println(" - Independent Watchdog reset");
  if (last_reset_reason & WINDOW_WDG)         Serial.println(" - Window Watchdog reset");
  if (last_reset_reason & LOW_POWER)          Serial.println(" - Low-power reset");
  if (last_reset_reason & OPTION_BYTE_LOADER) Serial.println(" - Option byte loader reset");
  if (last_reset_reason & NRST_PIN)           Serial.println(" - Pin reset (NRST or software)"); //last case so the rest take precedence before issuing NRST
  if (last_reset_reason == UNKNOWN_RESET)     Serial.println(" - Unknown or no flags set");

  // Trigger software reset on button press
  if (digitalRead(USER_BTN_PIN) == HIGH) {
    Serial.println("Button pressed → Triggering reset...");
    delay(300);  // Debounce
    Reset_My_MCU();
  }

  delay(1000);
}
