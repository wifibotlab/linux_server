//
// Created by rbeal on 5/10/17.
//

#ifndef SERVER_MODBUS_KEY_NON_BLOCK_H
#define SERVER_MODBUS_KEY_NON_BLOCK_H

void reset_terminal_mode();
void set_conio_terminal_mode();
int kbhit();
int getch();

#endif //SERVER_MODBUS_KEY_NON_BLOCK_H
