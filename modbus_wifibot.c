//
// Created by rbeal on 5/12/17.
//

#include <stdio.h>
#include <unistd.h>
#include "global.h"
#include "modbus_wifibot.h"
#include "rs232.h"

int wifibot_set_wtd(unsigned int milliseconds) {

    unsigned char buff[20];
    short crc;

    milliseconds /= 10;

    buff[0] = MODBUS_SYNC_BYTE;
    buff[1] = 5;
    buff[2] = MODBUS_CMD_SET_WTD;
    buff[3] = (unsigned char) (milliseconds&0xFF);
    buff[4] = (unsigned char) ((milliseconds>>8) & 0xFF);
    crc = crc16(buff+1, 4);
    buff[5] = (unsigned char)(crc & 0xFF);
    buff[6] = (unsigned char)((crc>>8) & 0xFF);

    rs232_write(buff, 6);

    shared.wifibot_read->wtd = milliseconds;
}


void wifibot_read_stream(struct shared_t *shared) {

    struct wifibot_read_t *wifibot;
    wifibot = shared->wifibot_read;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {

        if (shared->buff_data_in[0] == MODBUS_SYNC_BYTE) {

            shared->buff_data_in[0] = 0;

            switch (shared->buff_data_in[2]) {

                case MODBUS_STREAM:

                    wifibot->odom_avg =  shared->buff_data_in[3];
                    wifibot->odom_avg += shared->buff_data_in[4]<<8;
                    wifibot->odom_avg += shared->buff_data_in[5]<<16;
                    wifibot->odom_avg += shared->buff_data_in[6]<<24;

                    wifibot->odom_avd =  shared->buff_data_in[7];
                    wifibot->odom_avd += shared->buff_data_in[8]<<8;
                    wifibot->odom_avd += shared->buff_data_in[9]<<16;
                    wifibot->odom_avd+= shared->buff_data_in[10]<<24;

                    wifibot->odom_arg =  shared->buff_data_in[11];
                    wifibot->odom_arg += shared->buff_data_in[12]<<8;
                    wifibot->odom_arg += shared->buff_data_in[13]<<16;
                    wifibot->odom_arg += shared->buff_data_in[14]<<24;

                    wifibot->odom_ard =  shared->buff_data_in[15];
                    wifibot->odom_ard += shared->buff_data_in[16]<<8;
                    wifibot->odom_ard += shared->buff_data_in[17]<<16;
                    wifibot->odom_ard += shared->buff_data_in[18]<<24;


                    wifibot->tension = shared->buff_data_in[19] / 10;
                    wifibot->current = shared->buff_data_in[20] / 10;

                    wifibot->temp = (double) (shared->buff_data_in[21]) / 2;
                    wifibot->hygro = (double) (shared->buff_data_in[22]) / 2;

                    wifibot->speed_av = shared->buff_data_in[23];
                    wifibot->speed_ar = shared->buff_data_in[24];

                    break;

                default:
                    break;
            }
        }

        usleep(100);
    }
#pragma clang diagnostic pop
}