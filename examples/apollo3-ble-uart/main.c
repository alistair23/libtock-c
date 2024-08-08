#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libtock/interface/console.h>
#include <libtock/services/alarm.h>
#include "libtock-sync/services/alarm.h"
#include <libtock/tock.h>

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

#include "am_util.h"

#include "tag_api.h"
#include "app_ui.h"

#include "wsf_msg.h"

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

static void timer_cb(__attribute__ ((unused)) uint32_t now,
                     __attribute__ ((unused)) uint32_t scheduled,
                     __attribute__ ((unused)) void*    opaque) {
  WsfTaskSetReady(0, WSF_TIMER_EVENT);
}

void
scheduler_timer_init(void)
{
  static libtock_alarm_t timer;
  // printf("Setting Timer in app\n");
  libtock_alarm_repeating_every_ms(100, timer_cb, NULL, &timer);

}

// Delays for a desired amount of loops.
// This re-implemented the HAL delay without
// calling into the ROM
// We can't use `libtocksync_alarm_delay_ms()` as the syscall
// overhead is too large.
void am_hal_flash_delay(uint32_t ui32Iterations)
{
    for (int i = 0; i < ui32Iterations; i++) {
        asm("nop");
    }
}

void exactle_stack_init(void){
    wsfHandlerId_t handlerId;

    printf("Set up timers for the WSF scheduler.\n");

    //
    // Set up timers for the WSF scheduler.
    //
    WsfOsInit();
    WsfTimerInit();

    printf("Initialize a buffer pool for WSF dynamic memory needs.\n");

    //
    // Initialize a buffer pool for WSF dynamic memory needs.
    //
    WsfBufInit(sizeof(g_pui32BufMem), (uint8_t*)g_pui32BufMem, WSF_BUF_POOLS, g_psPoolDescriptors);

    printf("Initialize security.\n");

    //
    // Initialize security.
    //
    SecInit();
    SecAesInit();
    SecCmacInit();
    SecEccInit();

    printf("Set up callback functions for the various layers of the ExactLE stack.\n");

    //
    // Set up callback functions for the various layers of the ExactLE stack.
    //
    handlerId = WsfOsSetNextHandler(HciHandler);
    HciHandlerInit(handlerId);

    printf("DmHandler\n");

    handlerId = WsfOsSetNextHandler(DmHandler);
    DmDevVsInit(0);
    DmAdvInit();
    DmConnInit();
    DmConnSlaveInit();
    DmSecInit();
    DmSecLescInit();
    DmPrivInit();
    DmHandlerInit(handlerId);

    printf("L2cSlaveHandler\n");

    handlerId = WsfOsSetNextHandler(L2cSlaveHandler);
    printf("L2cSlaveHandlerInit\n");
    L2cSlaveHandlerInit(handlerId);
    printf("L2cInit\n");
    L2cInit();
    printf("L2cSlaveInit\n");
    L2cSlaveInit();

    printf("AttHandler\n");

    handlerId = WsfOsSetNextHandler(AttHandler);
    AttHandlerInit(handlerId);
    AttsInit();
    AttsIndInit();
    AttcInit();

    printf("SmpHandler\n");

    handlerId = WsfOsSetNextHandler(SmpHandler);
    SmpHandlerInit(handlerId);
    SmprInit();
    SmprScInit();
    HciSetMaxRxAclLen(251);

    printf("AppHandler\n");

    handlerId = WsfOsSetNextHandler(AppHandler);
    AppHandlerInit(handlerId);

    printf("TagHandler\n");

    handlerId = WsfOsSetNextHandler(TagHandler);
    TagHandlerInit(handlerId);

    printf("HciDrvHandler\n");

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
  // set_adv_name("TockOS BLE");

  command(0x10001, 2, 0x5000C000, 0xD000);

  //
  // Boot the radio.
  //
  HciDrvRadioBoot(1);
  printf("Radio booted\n");

  //
  // Initialize the main ExactLE stack.
  //
  exactle_stack_init();
  printf("Stack init complete\n");

  scheduler_timer_init();
  printf("Set up timer\n");

  //
  // Start the "Tag" profile.
  //
  TagStart();
  printf("Finished Setup\n");

  while (1)
    {

        //
        // Calculate the elapsed time from our free-running timer, and update
        // the software timers in the WSF scheduler.
        //
        wsfOsDispatcher();
    }
}
