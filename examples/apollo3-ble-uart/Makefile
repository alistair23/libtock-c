# Makefile for user application

# Specify this directory relative to the current application.
TOCK_USERLAND_BASE_DIR = ../..

# Which files to compile.
C_SRCS := $(wildcard *.c)

# External libraries used
# EXTERN_LIBS += exactle/uecc
# EXTERN_LIBS += exactle/ble-host
# EXTERN_LIBS += exactle/ble-profiles

CPPFLAGS+=  -DPART_$(PART)
CPPFLAGS+= -DAM_FREERTOS
CPPFLAGS+= -DAM_CUSTOM_BDADDR
CPPFLAGS+= -DAM_PACKAGE_BGA
CPPFLAGS+= -DWSF_TRACE_ENABLED
CPPFLAGS+= -DAM_DEBUG_PRINTF
CPPFLAGS+= -DAM_PART_APOLLO3
CPPFLAGS+= -DSEC_ECC_CFG=SEC_ECC_CFG_UECC
CPPFLAGS+= -DWSF_TRACE_ENABLED
CPPFLAGS+= -DHCI_TRACE_ENABLED
CPPFLAGS+= -Dgcc
CPPFLAGS+= -DNOFPU

CPPFLAGS += -Iexactle/wsf/include
CPPFLAGS += -Iexactle/wsf/sources/
CPPFLAGS += -Iexactle/wsf/sources/util/
CPPFLAGS += -Iexactle/wsf/sources/port/freertos/
CPPFLAGS += -Iexactle/ble-host/include
CPPFLAGS += -Iexactle/ble-host/sources/stack/cfg/
CPPFLAGS += -Iexactle/ble-host/sources/sec/common
CPPFLAGS += -Iexactle/ble-host/sources/hci/ambiq/
CPPFLAGS += -Iexactle/ble-host/sources/stack/smp/
CPPFLAGS += -Iexactle/ble-host/sources/stack/dm/
CPPFLAGS += -Iexactle/ble-host/sources/stack/hci/
CPPFLAGS += -Iexactle/ble-host/sources/hci/ambiq/apollo3
CPPFLAGS += -Iexactle/ble-profiles/include
CPPFLAGS += -Iexactle/ble-profiles/include/app
CPPFLAGS += -Iexactle/ble-profiles/sources/apps/app/
CPPFLAGS += -Iexactle/ble-profiles/sources/services
CPPFLAGS += -Iexactle/ble-profiles/sources/profiles/
CPPFLAGS += -Iexactle/ble-profiles/sources/profiles/gatt/
CPPFLAGS += -Iexactle/ble-profiles/sources/profiles/gap
CPPFLAGS += -Iexactle/ble-profiles/sources/profiles/fmpl
CPPFLAGS += -Iexactle/ble-profiles/sources/profiles/include/
CPPFLAGS += -Iexactle/ble-profiles/sources/apps/
CPPFLAGS += -Iuecc

CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/mcu/apollo3/
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/utils/
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/mcu/apollo3/hal/
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/CMSIS/AmbiqMicro/Include/
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/CMSIS/ARM/Include/
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/boards_sfe/common/examples/ble_freertos_tag/src
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/boards_sfe/redboard_artemis_nano/bsp/
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/devices

CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/third_party/FreeRTOSv10.1.1/Source/include
CPPFLAGS += -I/scratch/alistair/software/mbed/AmbiqSuiteSDK/third_party/FreeRTOSv10.1.1/Source/portable/GCC/ARM_CM4F/

VPATH+= exactle/wsf/sources/port/freertos
VPATH+=:exactle/ble-profiles/sources/profiles/hid
VPATH+=:exactle/ble-profiles/sources/profiles/fmpl
VPATH+=:exactle/ble-host/sources/stack/att
VPATH+=:exactle/ble-host/sources/sec/uecc
VPATH+=:exactle/ble-profiles/sources/apps/app/common
VPATH+=:exactle/ble-host/sources/hci/ambiq/apollo3
VPATH+=:exactle/wsf/sources/util
VPATH+=:exactle/ble-host/sources/stack/dm
VPATH+=:exactle/ble-profiles/sources/profiles/wdxs
VPATH+=:exactle/ble-profiles/sources/profiles/gatt
VPATH+=:exactle/ble-profiles/sources/profiles/gap
VPATH+=:exactle/ble-profiles/sources/services
VPATH+=:exactle/ble-profiles/sources/apps/tag
VPATH+=:exactle/ble-host/sources/sec/common
VPATH+=:exactle/ble-profiles/sources/profiles/bas
VPATH+=:exactle/ble-host/sources/stack/smp
VPATH+=:exactle/ble-profiles/sources/apps/app
VPATH+=:exactle/ble-host/sources/stack/hci
VPATH+=:exactle/ble-host/sources/stack/cfg
VPATH+=:exactle/ble-host/sources/hci/ambiq
VPATH+=:exactle/ble-profiles/sources/apps/hidapp
VPATH+=:exactle/ble-host/sources/stack/l2c
VPATH+=:uecc

VPATH+=:/scratch/alistair/software/mbed/AmbiqSuiteSDK/utils/
VPATH+=:/scratch/alistair/software/mbed/AmbiqSuiteSDK/devices
VPATH+=:/scratch/alistair/software/mbed/AmbiqSuiteSDK/mcu/apollo3/hal/
VPATH+=:exactle/wsf/sources/port/freertos/
VPATH+=:/scratch/alistair/software/mbed/AmbiqSuiteSDK/third_party/FreeRTOSv10.1.1/Source/
VPATH+=:/scratch/alistair/software/mbed/AmbiqSuiteSDK/third_party/FreeRTOSv10.1.1/Source/portable/GCC/ARM_CM4F/
VPATH+=:/scratch/alistair/software/mbed/AmbiqSuiteSDK/third_party/FreeRTOSv10.1.1/Source/portable/MemMang/
VPATH+=:/scratch/alistair/software/mbed/AmbiqSuiteSDK/boards_sfe/common/examples/ble_freertos_tag/src/

C_SRCS += l2c_coc.c
C_SRCS += l2c_main.c
C_SRCS += l2c_master.c
C_SRCS += l2c_slave.c

C_SRCS += wsf_assert.c
C_SRCS += wsf_timer.c
C_SRCS += wsf_trace.c
C_SRCS += wsf_os.c
C_SRCS += wsf_buf.c
C_SRCS += wsf_efs.c
C_SRCS += wsf_math.c
C_SRCS += wsf_msg.c
C_SRCS += wsf_queue.c
C_SRCS += hid_main.c
C_SRCS += fmpl_main.c

C_SRCS += attc_disc.c
C_SRCS += attc_main.c
C_SRCS += attc_proc.c
C_SRCS += attc_read.c
C_SRCS += attc_sign.c
C_SRCS += attc_write.c

C_SRCS += atts_ccc.c
C_SRCS += atts_csf.c
C_SRCS += atts_dyn.c
C_SRCS += atts_ind.c
C_SRCS += atts_main.c
C_SRCS += atts_proc.c
C_SRCS += atts_read.c
C_SRCS += atts_sign.c
C_SRCS += atts_write.c

C_SRCS += att_main.c
C_SRCS += att_uuid.c

C_SRCS += sec_ecc.c
C_SRCS += app_db.c
C_SRCS += app_hw.c
C_SRCS += app_ui.c

C_SRCS += hci_drv_apollo3.c
C_SRCS += bda.c
C_SRCS += bstream.c
C_SRCS += calc128.c
C_SRCS += crc32.c
C_SRCS += print.c
C_SRCS += terminal.c

C_SRCS += wstr.c

C_SRCS += dm_adv.c
C_SRCS += dm_adv_ae.c
C_SRCS += dm_adv_leg.c
C_SRCS += dm_conn.c
C_SRCS += dm_conn_cte.c
C_SRCS += dm_conn_master.c
C_SRCS += dm_conn_master_ae.c
C_SRCS += dm_conn_master_leg.c
C_SRCS += dm_conn_slave.c
C_SRCS += dm_conn_slave_ae.c
C_SRCS += dm_conn_slave_leg.c
C_SRCS += dm_conn_sm.c
C_SRCS += dm_dev.c
C_SRCS += dm_dev_priv.c
C_SRCS += dm_main.c
C_SRCS += dm_past.c
C_SRCS += dm_phy.c
C_SRCS += dm_priv.c
C_SRCS += dm_scan.c
C_SRCS += dm_scan_ae.c
C_SRCS += dm_scan_leg.c
C_SRCS += dm_sec.c
C_SRCS += dm_sec_lesc.c
C_SRCS += dm_sec_master.c
C_SRCS += dm_sec_slave.c
C_SRCS += dm_sync_ae.c

C_SRCS += gatt_main.c
C_SRCS += gap_main.c

C_SRCS += uECC.c
C_SRCS += uECC_ll.c

C_SRCS += port.c
C_SRCS += sec_aes.c
C_SRCS += sec_aes_rev.c
C_SRCS += sec_ccm_hci.c
C_SRCS += sec_cmac_hci.c
C_SRCS += sec_ecc_debug.c
C_SRCS += sec_ecc_hci.c
C_SRCS += sec_main.c

C_SRCS += bas_main.c

C_SRCS += smpi_act.c
C_SRCS += smpi_sc_act.c
C_SRCS += smpi_sc_sm.c
C_SRCS += smpi_sm.c

C_SRCS += smpr_act.c
C_SRCS += smpr_sc_act.c
C_SRCS += smpr_sc_sm.c
C_SRCS += smpr_sm.c
C_SRCS += smp_act.c
C_SRCS += smp_db.c
C_SRCS += smp_main.c
C_SRCS += smp_non.c
C_SRCS += smp_sc_act.c
C_SRCS += smp_sc_main.c

C_SRCS += hci_main.c

C_SRCS += cfg_stack.c

C_SRCS += hci_cmd.c
C_SRCS += hci_cmd_ae.c
C_SRCS += hci_cmd_cte.c
C_SRCS += hci_cmd_past.c
C_SRCS += hci_cmd_phy.c
C_SRCS += hci_core.c
C_SRCS += hci_core_ps.c
C_SRCS += hci_evt.c
C_SRCS += hci_tr.c
C_SRCS += hci_vs.c
C_SRCS += hci_vs_ae.c

C_SRCS += am_util_ble.c
C_SRCS += am_util_debug.c
C_SRCS += am_util_delay.c
C_SRCS += am_util_id.c
C_SRCS += am_util_stdio.c

C_SRCS += event_groups.c
C_SRCS += list.c
C_SRCS += queue.c
C_SRCS += tasks.c
C_SRCS += timers.c
C_SRCS += heap_2.c
C_SRCS += rtos.c
C_SRCS += svc_core.c
C_SRCS += svc_temp.c
C_SRCS += svc_px.c
C_SRCS += svc_batt.c

C_SRCS += am_devices_button.c
C_SRCS += am_devices_led.c
C_SRCS += app_disc.c
C_SRCS += app_main.c
C_SRCS += app_master.c
C_SRCS += app_master_leg.c
C_SRCS += app_server.c
C_SRCS += app_slave.c
C_SRCS += app_slave_leg.c
C_SRCS += app_terminal.c

C_SRCS += am_hal_ble.c
C_SRCS += am_hal_ble_patch.c
C_SRCS += am_hal_ble_patch_b0.c
C_SRCS += am_hal_cachectrl.c
C_SRCS += am_hal_clkgen.c
C_SRCS += am_hal_cmdq.c
C_SRCS += am_hal_ctimer.c
C_SRCS += am_hal_debug.c
C_SRCS += am_hal_flash.c
C_SRCS += am_hal_global.c
C_SRCS += am_hal_interrupt.c
C_SRCS += am_hal_mcuctrl.c
C_SRCS += am_hal_pwrctrl.c
C_SRCS += am_hal_queue.c

# Include userland master makefile. Contains rules and flags for actually
# building the application.
include $(TOCK_USERLAND_BASE_DIR)/AppMakefile.mk
