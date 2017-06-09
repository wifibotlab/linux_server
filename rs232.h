//
// Created by rbeal on 5/10/17.
//

#ifndef SERVER_MODBUS_RS232_H
#define SERVER_MODBUS_RS232_H

int rs232_init(char *dev);
void rs232_deinit(void);
void rs232_read(struct shared_t *shared);
int rs232_write(unsigned char *data, unsigned int size);

#endif //SERVER_MODBUS_RS232_H
