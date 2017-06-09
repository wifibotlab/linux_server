//
// Created by rbeal on 5/11/17.
//

#ifndef SERVER_MODBUS_MODBUS_WIFIBOT_H
#define SERVER_MODBUS_MODBUS_WIFIBOT_H

#define MODBUS_SYNC_BYTE 254

// ############ CMD LIST ###############

#define MODBUS_CMD_SET_WTD    0x2
/*
 * DATA OCT (1): wtd_h
 * DATA OCT (2): wtd_l
 */

#define MODBUS_CMD_SPEED 0x3
/*
 * DATA OCT (1): Left_L
 * DATA OCT (2): Left_H
 * DATA OCT (3): Right_L
 * DATA OCT (4): Right_H
 * DATA OCT (5): SPEED FLAG
 */

#define MODBUS_CMD_WTD 0x4
/*
 * DATA OCT (1): WTD_L
 * DATA OCT (2): WTD_H
 */

#define MODBUS_DEBUG      0xF0
#define MODBUS_STREAM     0xF1

int wifibot_set_wtd(unsigned int milliseconds);
void wifibot_read_stream(struct shared_t *shared); // thread

#endif //SERVER_MODBUS_MODBUS_WIFIBOT_H
