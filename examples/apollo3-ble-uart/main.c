#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <console.h>
#include <tock.h>

#include "wsf_types.h"
#include "wsf_trace.h"
#include "wsf_buf.h"

#include "hci_handler.h"
#include "dm_handler.h"
#include "l2c_handler.h"
#include "att_handler.h"
#include "smp_handler.h"
#include "l2c_api.h"
#include "att_api.h"
#include "smp_api.h"
#include "app_api.h"
#include "hci_core.h"
#include "hci_drv.h"
#include "hci_drv_apollo.h"
#include "hci_drv_apollo3.h"

#include "am_mcu_apollo.h"
#include "am_util.h"

#include "tag_api.h"
#include "app_ui.h"

#include "wsf_msg.h"

#include <timer.h>

#define WSF_BUF_POOLS               4

static uint32_t g_pui32BufMem[(WSF_BUF_POOLS*16
         + 16*8 + 32*4 + 64*6 + 280*8) / sizeof(uint32_t)];

// Default pool descriptor.
static wsfBufPoolDesc_t g_psPoolDescriptors[WSF_BUF_POOLS] =
{
    {  16,  8 },
    {  32,  4 },
    {  64,  6 },
    { 280,  8 }
};

static void timer_cb (__attribute__ ((unused)) int arg0,
                      __attribute__ ((unused)) int arg1,
                      __attribute__ ((unused)) int arg2,
                      __attribute__ ((unused)) void* userdata) {
  printf("Timer Fired in app\n");
  WsfTaskSetReady(0, WSF_TIMER_EVENT);
}

void
scheduler_timer_init(void)
{
  static bool resume = 0;
  static tock_timer_t timer;
  printf("Setting Timer in app\n");
  timer_every(100, timer_cb, &resume, &timer);

}

void exactle_stack_init(void){
    wsfHandlerId_t handlerId;

    //
    // Set up timers for the WSF scheduler.
    //
    scheduler_timer_init();
    WsfOsInit();
    WsfTimerInit();

    //
    // Initialize a buffer pool for WSF dynamic memory needs.
    //
    WsfBufInit(sizeof(g_pui32BufMem), (uint8_t*)g_pui32BufMem, WSF_BUF_POOLS, g_psPoolDescriptors);

    //
    // Initialize security.
    //
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    //
    // Set up callback functions for the various layers of the ExactLE stack.
    //
    handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmAdvInit();
    DmConnInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    L2cSlaveHandlerInit(handlerId);
    L2cInit();
    L2cSlaveInit();

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();
    AttcInit();

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(251);

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(TagHandler);
    TagHandlerInit(handlerId);

    handlerId = WsfOsSetNextHandler(HciDrvHandler);
    HciDrvHandlerInit(handlerId);
}

/*******************************************************************************
 * MAIN
 ******************************************************************************/

int main (void) {
  printf("Apollo3 BLE Example\n");

  //
  // Configure the peripheral's advertised name: (tag_main.c)
  set_adv_name("TockOS BLE");

  //
  // Boot the radio.
  //
  HciDrvRadioBoot(1);

  printf("Radio booted\n");

  for (int i = 0; i < 1000000; i++) {
    asm("nop");
  }

  //
  // Initialize the main ExactLE stack.
  //
  exactle_stack_init();

  printf("Stack init\n");

  // printf("Setting callback\n");

  // subscribe(0x30000, 0, am_ble_isr, NULL);

  //
  // Start the "Tag" profile.
  //
  TagStart();

  printf("Finished Setup\n");

  yield();

  while (1)
    {

        //
        // Calculate the elapsed time from our free-running timer, and update
        // the software timers in the WSF scheduler.
        //
        wsfOsDispatcher();
    }
}
