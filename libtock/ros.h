#pragma once

#include <stdlib.h>
#include <string.h>

#include "tock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ROS_DRIVER_NUM 0x10001

// We currently support ROS version 1
// Version 1:
//   |-------------------------|
//   |       Count (u32)       |
//   |-------------------------|
//   |   Pending Tasks (u32)   |
//   |-------------------------|
//   |                         |
//   |     Time Ticks (u64)    |
//   |-------------------------|
#define ROS_BUFFER_LEN (4 * 4 + 4 * 4 + 8 * 4)

// Get the latest version of the read only syscalls supported by the kernel.
int ros_get_version(void);

// Share a buffer with the kernel to use for read only syscalls
//
// `base` the buffer to use
// `len` should be ROS_BUFFER_LEN
int ros_allocate_region(void* base, int len);

// Use the read only syscall buffer provided by `base`
// to get the number of pending tasks.
uint32_t ros_get_pending_tasks(void* base);

// Use the read only syscall buffer provided by `base`
// to get the current time returned from the kernel.
uint64_t ros_get_ticks(void* base);

#ifdef __cplusplus
}
#endif
