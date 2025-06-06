/* Last Reset Reason Sketch
* This sketch will determine what caused the last reset on the STM32 MCU. Most microcontrollers
* have a register dedicated to storing the last reason of the chip, weather beeing from a 
* low power condition, software caused brown-out. Test it by resetting the MCU via the USER button
* which triggers the Reset_my_MCU() function or unplug the USB cable and repluggit back. Adjust your 
* UART, USER Button pin and registers accordingly. Use the MCU's datasheet and/or stm32yyyxxx.h for reference.
* The code is provided "as is" with no liability.
*/

#define USER_BTN_PIN USER_BTN  // Adjust this for your board

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
uint32_t saved_reset_flags = 0;

void Reset_My_MCU() {
  // Bellow is the Software reset condition
  //NVIC_SystemReset(); 

  // Bellow is the Watchdog Timer reset condition
  IWDG->KR = 0x5555;

  // Set prescaler and reload (for short timeout ~100ms)
  IWDG->PR = 0;          // Prescaler = /4
  IWDG->RLR = 100;       // Reload value (for short timeout)

  // Start the watchdog
  IWDG->KR = 0xCCCC;

  // Don't refresh it — let it timeout
  while (1);  // Wait for reset
}

void setup() {
  pinMode(USER_BTN_PIN, INPUT_PULLDOWN);

  Serial.begin(115200);
  while (!Serial);  // Wait for Serial

  // Save and analyze the reset flags
  saved_reset_flags = RCC->CSR;

  if (saved_reset_flags & RCC_CSR_BORRSTF)         last_reset_reason = (reset_reason)(last_reset_reason | BROWN_OUT);
  if (saved_reset_flags & RCC_CSR_PINRSTF)         last_reset_reason = (reset_reason)(last_reset_reason | NRST_PIN);
  if (saved_reset_flags & RCC_CSR_SFTRSTF)         last_reset_reason = (reset_reason)(last_reset_reason | SOFTWARE_RST);
  if (saved_reset_flags & RCC_CSR_IWDGRSTF)        last_reset_reason = (reset_reason)(last_reset_reason | INDEPENDENT_WDG);
  if (saved_reset_flags & RCC_CSR_WWDGRSTF)        last_reset_reason = (reset_reason)(last_reset_reason | WINDOW_WDG);
  if (saved_reset_flags & RCC_CSR_LPWRRSTF)        last_reset_reason = (reset_reason)(last_reset_reason | LOW_POWER);
  if (saved_reset_flags & RCC_CSR_OBLRSTF)         last_reset_reason = (reset_reason)(last_reset_reason | OPTION_BYTE_LOADER);

  // Clear reset flags
  RCC->CSR |= RCC_CSR_RMVF;
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
