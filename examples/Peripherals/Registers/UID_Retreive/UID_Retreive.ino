/* UID Retreive sketch
* UID (Universal Identifier) is a ID that's etched to each MCU at factory release 
* so it's uniquely identifiable. This can help traceability and addressing devices 
* without having to craft a database yourself. This sketch retreives UID, MAC, Device
* and Revision ID of each MCU. Refer to the relevant datasheet to know where are these 
* values are stored in the registers.
* The code is provided "as is" with no liability.
*/

//Change depending on your MCU datasheet
#define DEVICE_ID_REG 0x0BFA070C
#define UID0 0x0BFA0700
#define UID1 0x0BFA0704
#define UID2 0x0BFA0708

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial to be ready

  Serial.println("STM32U385 Device Identifiers:");

  // Unique Device ID (96 bits / 12 bytes)
  uint32_t uid0 = *(uint32_t*)UID0;
  uint32_t uid1 = *(uint32_t*)UID1;
  uint32_t uid2 = *(uint32_t*)UID2;

  Serial.print("UID: ");
  Serial.print(uid2, HEX); Serial.print("-");
  Serial.print(uid1, HEX); Serial.print("-");
  Serial.println(uid0, HEX);

  // MAC Address: typically stored in UID for STM32U series
  // Use the lower 6 bytes of the 96-bit UID (commonly used)
  uint8_t mac[6] = {
    (uint8_t)(uid0 >>  0),
    (uint8_t)(uid0 >>  8),
    (uint8_t)(uid0 >> 16),
    (uint8_t)(uid1 >>  0),
    (uint8_t)(uid1 >>  8),
    (uint8_t)(uid1 >> 16)
  };

  Serial.print("MAC Address: ");
  for (int i = 0; i < 6; i++) {
    if (mac[i] < 0x10) Serial.print("0");
    Serial.print(mac[i], HEX);
    if (i < 5) Serial.print(":");
  }
  Serial.println();

  // Device ID (DEV_ID) and Revision ID (REV_ID)
  uint32_t dev_id_reg = *(uint32_t*)DEVICE_ID_REG;
  uint16_t dev_id = dev_id_reg & 0x0FFF;
  uint16_t rev_id = (dev_id_reg >> 16) & 0xFFFF;

  Serial.print("Device ID: 0x"); Serial.println(dev_id, HEX);
  Serial.print("Revision ID: 0x"); Serial.println(rev_id, HEX);
}

void loop() {
  // Nothing here
}
