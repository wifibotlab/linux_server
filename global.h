//
// Created by rbeal on 5/10/17.
//

#ifndef SERVER_MODBUS_GLOBAL_H
#define SERVER_MODBUS_GLOBAL_H

#define BUFFER_SIZE 50
#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define DEBUG 0

struct wifibot_read_t {

    long odom_avg;
    long odom_avd;
    long odom_arg;
    long odom_ard;

    double tension;
    double current;
    double temp;
    double hygro;

    unsigned char speed_av;
    unsigned char speed_ar;

    unsigned int wtd;
};

struct wifibot_cmd_t {
    unsigned short speed_l;
    unsigned short speed_r;

    unsigned int wtd;
};

struct shared_t {
    int rs232_filestream;
    unsigned char *buff_data_out;
    unsigned char *buff_data_in;
    struct wifibot_read_t *wifibot_read;
    struct wifibot_cmd_t *wifibot_cmd;

    unsigned char buffso_Raw_Data_Out[21];
    unsigned char buffso_Raw_Data_In[30];

    char cmd_mode;
    /*
     * For watchdog
     * 0 = Me
     * 1 = TCP
     */
};


//int debug=0;

extern struct shared_t shared;
extern struct wifibot_read_t wifibot_read;

short crc16(unsigned char *adresse_tab , unsigned char taille_max);

#endif //SERVER_MODBUS_GLOBAL_H
