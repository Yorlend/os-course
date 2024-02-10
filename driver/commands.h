#pragma once

#include <linux/types.h>

#define FLAGS_MESSAGE_PROTOCOL 0xa0
#define FLAGS_TRANSPORT_LAYER_SECURITY 0xb0
#define FLAGS_TRANSPORT_LAYER_SECURITY_DATA 0xb2

#define COMMAND_NOP 0x00
#define COMMAND_MCU_GET_IMAGE 0x20
#define COMMAND_WRITE_SENSOR_REGISTER 0x80
#define COMMAND_READ_SENSOR_REGISTER 0x82
#define COMMAND_REQUEST_TLS_CONNECTION 0xd0
#define COMMAND_ENABLE_CHIP 0x96
#define COMMAND_RESET 0xa2
#define COMMAND_MCU_GET_POV_IMAGE 0xd2
#define COMMAND_TLS_SUCCESSFULLY_ESTABLISHED 0xd4

int encode_message_pack(void* out_buffer, const void* payload, size_t length, uint8_t flags);
