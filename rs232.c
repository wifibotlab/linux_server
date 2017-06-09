//
// Created by rbeal on 5/10/17.
//

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "string.h"

#include "global.h"
#include "rs232.h"
#include "modbus_wifibot.h"

int rs232_init(char *dev) {

    shared.rs232_filestream = -1;


    shared.rs232_filestream = open(dev, O_RDWR | O_NOCTTY /* | O_NDELAY*/);		//Open in non blocking read/write mode

    //CONFIGURE THE UART
    //The flags (defined in /usr/include/termios.h - see http://pubs.opengroup.org/onlinepubs/007908799/xsh/termios.h.html):
    //	Baud rate:- B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200, B230400, B460800, B500000, B576000, B921600, B1000000, B1152000, B1500000, B2000000, B2500000, B3000000, B3500000, B4000000
    //	CSIZE:- CS5, CS6, CS7, CS8
    //	CLOCAL - Ignore modem status lines
    //	CREAD - Enable receiver
    //	IGNPAR = Ignore characters with parity errors
    //	ICRNL - Map CR to NL on input (Use for ASCII comms where you want to auto correct end of line characters - don't use for bianry comms!)
    //	PARENB - Parity enable
    //	PARODD - Odd parity (else even)
    struct termios options;
    tcgetattr(shared.rs232_filestream, &options);
    options.c_cflag = B19200 | CS8 | CLOCAL | CREAD;		//<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(shared.rs232_filestream, TCIFLUSH);
    tcsetattr(shared.rs232_filestream, TCSANOW, &options);

    return shared.rs232_filestream;
}

void rs232_deinit(void) {

    close(shared.rs232_filestream);
}

void rs232_read (struct shared_t *shared) {

    int size;
    unsigned char buff_read;
    unsigned char buff[30];
    int nbuff=0;

    short crc_r;

    int state=0;
    /*
     * state:
     * 0- waiting for...
     * 1- Start byte ok
     */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        size = (int) read(shared->rs232_filestream, &buff_read, 1);


        if (size > 0) {
            //printf("buff: %u\n\r", (unsigned int)buff_read);
            if (buff_read == MODBUS_SYNC_BYTE && state == 0) {

                state = 1;
                nbuff = 1;
                buff[0] = buff_read;
            }
            else if (state == 1) {
                buff[nbuff] = buff_read;

                if (nbuff == (buff[1]+1)) { // buff[1]: packet size
                    //printf("Packet size: %d\n", buff[1]);
                    crc_r = crc16(buff+1, (unsigned char) (nbuff-2) );

                    if ( (buff[nbuff-1] == (crc_r&0xFF) ) && (buff[nbuff] == ((crc_r>>8) & 0xFF)) ) {
                        if (DEBUG) printf("UART CRC " COLOR_GREEN "[OK]\n" COLOR_RESET);
                        state = 0;

                        memcpy(shared->buff_data_in, buff, (size_t )nbuff);

                    }
                    else {
                        printf("CRCNOPE\n\r");
                    }
                }

                if (nbuff > (buff[1]+1) && state == 1) {

                    state = 0;
                    printf("UART RECEP " COLOR_RED "[ERROR]\n" COLOR_RESET);
                }

                nbuff++;
            }
        }
    }
#pragma clang diagnostic pop
}

int rs232_write(unsigned char *data, unsigned int size) {

    int error;

    if (DEBUG) printf("UART WRITE ");
    error = (int) write(shared.rs232_filestream, data, size);

    if (error != size) {

        if (DEBUG) printf(COLOR_RED "[ERROR]\n" COLOR_RESET);
        return -1;
    }
    else {

        if (DEBUG) printf(COLOR_GREEN "[OK]\n" COLOR_RESET);
        return 0;
    }
}

