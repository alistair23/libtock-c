/*
   RadioLib Non-Arduino Tock Library LoRaWAN test application

   Licensed under the MIT or Apache License

   Copyright (c) 2023 Alistair Francis <alistair@alistair23.me>
 */

// include the library
#include <RadioLib.h>

// include the hardware abstraction layer
#include "libtockHal.h"

// Include some libtock-c helpers
#include <libtock-sync/sensors/humidity.h>
#include <libtock-sync/sensors/moisture.h>
#include <libtock-sync/sensors/rainfall.h>
#include <libtock-sync/sensors/temperature.h>
#include <libtock-sync/storage/kv.h>

#include "CayenneLPP.h"

/* These need to be updated to use values from your LoRaWAN server */
uint64_t joinEUI   = 0x0000000000000000;
uint64_t devEUI    = 0x0000000000000000;
uint8_t appKey[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t nwkKey[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// regional choices: EU868, US915, AU915, AS923, IN865, KR920, CN780, CN500
const LoRaWANBand_t* Region = &AU915;
const uint8_t subBand       = 2;

#define MAX_PAYLOAD_SIZE 20

#define JOIN_EUI_KEY_LEN  8
uint8_t join_eui_key_buf[JOIN_EUI_KEY_LEN] = "joinEUI";

#define DEV_EUI_KEY_LEN  7
uint8_t dev_eui_key_buf[DEV_EUI_KEY_LEN] = "devEUI";

#define NWK_KEY_KEY_LEN  7
uint8_t nwk_key_key_buf[NWK_KEY_KEY_LEN] = "nwkKey";

#define APP_KEY_KEY_LEN  7
uint8_t app_key_key_buf[APP_KEY_KEY_LEN] = "appKey";

#define INTERNAL_NONCE_KEY_LEN  10
uint8_t internal_nonce_key_buf[INTERNAL_NONCE_KEY_LEN] = "int-nonce";

#define INTERNAL_SESSION_KEY_LEN  12
uint8_t internal_session_key_buf[INTERNAL_SESSION_KEY_LEN] = "int-session";

#define KV_DATA_LEN 8
uint8_t kv_data_buf[KV_DATA_LEN];

uint8_t internal_nonce_data_buf[RADIOLIB_LORAWAN_NONCES_BUF_SIZE];
uint8_t internal_session_data_buf[RADIOLIB_LORAWAN_SESSION_BUF_SIZE];

static libtock_alarm_t wdt_alarm;

// Retrieve the joinEUI from the Tock K/V store
static int retrieve_join_eui(void) {
  uint32_t value_len;
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_get(join_eui_key_buf, JOIN_EUI_KEY_LEN, kv_data_buf, KV_DATA_LEN, &value_len);

  if (ret == RETURNCODE_SUCCESS) {
    // We found the key, set the global variable
    joinEUI = (uint64_t) kv_data_buf[0] |
              ((uint64_t) kv_data_buf[1] << 8) |
              ((uint64_t) kv_data_buf[2] << 16) |
              ((uint64_t) kv_data_buf[3] << 24) |
              ((uint64_t) kv_data_buf[4] << 32) |
              ((uint64_t) kv_data_buf[5] << 40) |
              ((uint64_t) kv_data_buf[6] << 48) |
              ((uint64_t) kv_data_buf[7] << 56);
    return 0;
  } else {
    return 1;
  }
}

// Retrieve the devEUI from the Tock K/V store
static int retrieve_dev_eui(void) {
  uint32_t value_len;
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_get(dev_eui_key_buf, DEV_EUI_KEY_LEN, kv_data_buf, KV_DATA_LEN, &value_len);

  if (ret == RETURNCODE_SUCCESS) {
    // We found the key, set the global variable
    devEUI = (uint64_t) kv_data_buf[0] |
             ((uint64_t) kv_data_buf[1] << 8) |
             ((uint64_t) kv_data_buf[2] << 16) |
             ((uint64_t) kv_data_buf[3] << 24) |
             ((uint64_t) kv_data_buf[4] << 32) |
             ((uint64_t) kv_data_buf[5] << 40) |
             ((uint64_t) kv_data_buf[6] << 48) |
             ((uint64_t) kv_data_buf[7] << 56);
    return 0;
  } else {
    return 1;
  }
}

// Retrieve the nwkKey from the Tock K/V store
static int retrieve_nwk_key(void) {
  uint32_t value_len;
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_get(nwk_key_key_buf, NWK_KEY_KEY_LEN, nwkKey, 16, &value_len);

  if (ret == RETURNCODE_SUCCESS) {
    return 0;
  } else {
    return 1;
  }
}

// Retrieve the appKey from the Tock K/V store
static int retrieve_app_key(void) {
  uint32_t value_len;
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_get(app_key_key_buf, APP_KEY_KEY_LEN, appKey, 16, &value_len);

  if (ret == RETURNCODE_SUCCESS) {
    return 0;
  } else {
    return 1;
  }
}

static int retrieve_internal_nonces(void) {
  uint32_t value_len;
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_get(internal_nonce_key_buf, INTERNAL_NONCE_KEY_LEN, internal_nonce_data_buf,
                           RADIOLIB_LORAWAN_NONCES_BUF_SIZE, &value_len);

  if (ret == RETURNCODE_SUCCESS) {
    return 0;
  } else {
    return 1;
  }
}

static int set_internal_nonces(const uint8_t* persistentBuffer) {
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_set(internal_nonce_key_buf, INTERNAL_NONCE_KEY_LEN, persistentBuffer,
                           RADIOLIB_LORAWAN_NONCES_BUF_SIZE);

  if (ret == RETURNCODE_SUCCESS) {
    printf("Set internal nonce buffer\r\n");
    return 0;
  } else {
    return 1;
  }
}

static int retrieve_internal_session(void) {
  uint32_t value_len;
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_get(internal_session_key_buf, INTERNAL_SESSION_KEY_LEN, internal_session_data_buf,
                           RADIOLIB_LORAWAN_SESSION_BUF_SIZE, &value_len);

  if (ret == RETURNCODE_SUCCESS) {
    return 0;
  } else {
    return 1;
  }
}

static int set_internal_session(const uint8_t* persistentBuffer) {
  returncode_t ret;

  if (!libtock_kv_exists()) {
    return 1;
  }

  ret = libtocksync_kv_set(internal_session_key_buf, INTERNAL_SESSION_KEY_LEN, persistentBuffer,
                           RADIOLIB_LORAWAN_SESSION_BUF_SIZE);

  if (ret == RETURNCODE_SUCCESS) {
    printf("Set internal session buffer\r\n");
    return 0;
  } else {
    printf("Fail internal session buffer\r\n");
    return 1;
  }
}

// Retrieve the LoRaWAN keys from the Tock K/V store
static int retrieve_keys(void) {
  if (retrieve_join_eui() == 0) {
    printf("Retrieve joinEUI key from storage: 0x%lx%lx\r\n",
           (uint32_t)(joinEUI >> 32), (uint32_t)joinEUI);
  } else {
    printf("Unable to retrieve joinEUI key from storage\r\n");
    return 1;
  }

  if (retrieve_dev_eui() == 0) {
    printf("Retrieve devEUI key from storage: 0x%lx%lx\r\n",
           (uint32_t)(devEUI >> 32), (uint32_t)devEUI);
  } else {
    printf("Unable to retrieve devEUI key from storage\r\n");
    return 1;
  }

  if (retrieve_nwk_key() == 0) {
    printf("Retrieve nwkKey key from storage\r\n");
  } else {
    printf("Unable to retrieve nwkKey key from storage\r\n");
    return 1;
  }

  if (retrieve_app_key() == 0) {
    printf("Retrieve appKey key from storage\r\n");
  } else {
    printf("Unable to retrieve appKey key from storage\r\n");
    return 1;
  }

  return 0;
}

static int garbage_collect(void) {
  int ret = libtocksync_kv_garbage_collect();

  if (ret == RETURNCODE_SUCCESS) {
    return 0;
  } else {
    printf("Garbage Collection Failed\r\n");
    return 1;
  }
}

static void wdt_alarm_cb(__attribute__ ((unused)) uint32_t now,
                         __attribute__ ((unused)) uint32_t scheduled,
                         __attribute__ ((unused)) void*    opaque) {

  tock_restart(1);
}

static bool within_percent(int current, int past) {
  int lowest  = (past * 90) / 100;
  int highest = (past * 110) / 100;

  if (past == 0 && current != 0) {
    return true;
  }

  if (lowest <= current && current <= highest) {
    return false;
  }

  return true;
}

// the entry point for the program
int main(void) {
  CayenneLPP Payload(MAX_PAYLOAD_SIZE);

  // Setup alarm to reset app in 5 minutes
  libtock_alarm_in_ms(5 * 60 * 1000, wdt_alarm_cb, NULL, &wdt_alarm);

  // Retrieve the LoRaWAN keys from the Tock K/V store
  if (retrieve_keys()) {
    return 1;
  }

  if (garbage_collect()) {
    return 1;
  }

  printf("[SX1261] Initialising Radio ... \r\n");

  // create a new instance of the HAL class
  TockRadioLibHal* hal = new TockRadioLibHal();
  int state;

  // now we can create the radio module
  // pinout corresponds to the SparkFun LoRa Thing Plus - expLoRaBLE
  // NSS pin:   0
  // DIO1 pin:  2
  // NRST pin:  4
  // BUSY pin:  1
  SX1262 tock_module = new Module(hal, RADIOLIB_RADIO_NSS, RADIOLIB_RADIO_DIO_1, RADIOLIB_RADIO_RESET,
                                  RADIOLIB_RADIO_BUSY);
  LoRaWANNode node(&tock_module, Region, subBand);

  // Setup the radio
  // The settings here work for the SparkFun LoRa Thing Plus - expLoRaBLE
  node.scanGuard   = 30;
  tock_module.XTAL = true;
  state = tock_module.begin(915.0);

  if (state != RADIOLIB_ERR_NONE) {
    printf("begin failed, code %d\r\n", state);
    return 1;
  }

  node.beginOTAA(joinEUI, devEUI, nwkKey, appKey);

  if (retrieve_internal_nonces() == 0) {
    printf("Found existing nonce\n");
    node.setBufferNonces(internal_nonce_data_buf);
  }

  if (retrieve_internal_session() == 0) {
    printf("Using existing session data\n");

    state = node.setBufferSession(internal_session_data_buf);

    if (state != 0) {
      printf("setBufferSession failed, code %d\r\n", state);
    }
  }

  state = node.activateOTAA();

  if (state == RADIOLIB_LORAWAN_SESSION_RESTORED) {
    printf("activateOTAA restored session\r\n");
  } else if (state != RADIOLIB_LORAWAN_NEW_SESSION) {
    printf("activateOTAA failed, code %d\r\n", state);
    return 1;
  }

  printf("activate success!\r\n");

  set_internal_nonces(node.getBufferNonces());
  set_internal_session(node.getBufferSession());

  hal->detachInterrupt(RADIOLIB_RADIO_DIO_1);
  hal->pinMode(RADIOLIB_RADIO_DIO_1, TOCK_RADIOLIB_PIN_INPUT);

  bool temp_exists = false;
  bool humi_exists = false;
  bool mois_exists = false;
  bool rain_exists = false;
  int temp = 0, past_temp = -1;
  int humi = 0, past_humi = -1;
  int mois = 0, past_mois = -1;
  uint32_t rain = 0, past_rain = -1;
  int loop_skip_count = 0;

  printf("Probing sensors.\r\n");

  if (libtock_temperature_exists()) {
    printf("Temperature sensor exists.\r\n");
    temp_exists = true;
  }

  if (libtock_humidity_exists()) {
    printf("Humidity sensor exists.\r\n");
    humi_exists = true;
  }

  if (libtock_moisture_exists()) {
    printf("Moisture sensor exists.\r\n");
    mois_exists = true;
  }

  if (libtock_rainfall_exists()) {
    printf("Rainfall sensor exists.\r\n");
    rain_exists = true;
  }

  libtock_alarm_ms_cancel(&wdt_alarm);

  // loop forever
  for ( ;;) {
    // Setup alarm to reset app in 10 minutes
    libtock_alarm_in_ms(10 * 60 * 1000, wdt_alarm_cb, NULL, &wdt_alarm);

    Payload.reset();

    // printf("Reading sensor data\r\n");

    // Read some sensor data from the board
    if (temp_exists) {
      if (libtocksync_temperature_read(&temp) == RETURNCODE_SUCCESS) {
        // printf("Temperature: %d\r\n", temp);
        Payload.addTemperature(0, (float) temp / 100);
      }
    }
    if (humi_exists) {
      if (libtocksync_humidity_read(&humi) == RETURNCODE_SUCCESS) {
        // printf("Humidity: %d\r\n", humi);
        Payload.addRelativeHumidity(0, (float) humi / 100);
      }
    }
    if (mois_exists) {
      if (libtocksync_moisture_read(&mois) == RETURNCODE_SUCCESS) {
        // printf("Moisture: %d\r\n", mois);
        Payload.addRelativeHumidity(1, (float) mois / 100);
      }
    }
    if (rain_exists) {
      if (libtocksync_rainfall_read(&rain, 24) == RETURNCODE_SUCCESS) {
        // printf("Rainfall in last 24 hours: %d\r\n", rain);
        Payload.addAnalogInput(1, (float) rain / 1000);
      }
      if (libtocksync_rainfall_read(&rain, 1) == RETURNCODE_SUCCESS) {
        // printf("Rainfall in last hour: %d\r\n", rain);
        Payload.addAnalogInput(0, (float) rain / 1000);
      }
    }

    if ((within_percent(temp, past_temp) ||
         within_percent(humi, past_humi) ||
         within_percent(mois, past_mois) ||
         within_percent(rain, past_rain) ||
         loop_skip_count >= 11) &&
        Payload.getSize() > 0) {
      printf("[SX1261] Transmitting\r\n");

      past_temp = temp;
      past_humi = humi;
      past_mois = mois;
      past_rain = rain;

      // Avoid transmit delays if the timer wraps around by clearing the
      // next transmit schedule
      node.scheduleTransmission(0);

      state = node.sendReceive(Payload.getBuffer(), Payload.getSize());

      if (state >= 0) {
        // the packet was successfully transmitted
      } else {
        printf("failed to transmit, code %d\r\n", state);
      }

      set_internal_nonces(node.getBufferNonces());
      set_internal_session(node.getBufferSession());
      garbage_collect();

      loop_skip_count = 0;
    } else {
      loop_skip_count++;
    }

    printf("Waiting 5 minutes before transmitting again\r\n");
    hal->delay(5 * 60 * 1000);

    libtock_alarm_ms_cancel(&wdt_alarm);
  }

  return 0;
}
