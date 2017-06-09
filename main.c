#include <stdio.h>
#include <stdlib.h>
#include "pthread.h"
#include <unistd.h>

#include "global.h"
#include "rs232.h"
#include "key_non_block.h"
#include "modbus_wifibot.h"
#include "tcp_server.h"

struct shared_t shared;
void * thread_rs232_read(void *p_data);
void * thread_wifibot_read_stream(void *p_data);
void * thread_tcp_socket(void *p_data);

void wifibot_process(struct wifibot_cmd_t *wifibot);

int main() {

    // threads
    pthread_t rs232_read;
    pthread_t wifibot_read_wtream;
    pthread_t tcp_socket;

    // var
    struct wifibot_cmd_t wifibot_cmd;
    struct wifibot_read_t wifibot_read;
    short crc;
    unsigned char buff[20];
    unsigned char key;

    shared.wifibot_read = &wifibot_read;

    // INIT BUFFER ##############################
    printf("Buff in  ");
    shared.buff_data_in  = malloc(BUFFER_SIZE);
    if (shared.buff_data_in == NULL) {
        printf(COLOR_RED "[ERROR]\n" COLOR_RESET);
        return -1;
    }
    else
    {
        printf(COLOR_GREEN "[OK]\n" COLOR_RESET);
    }

    printf("Buff out ");
    shared.buff_data_out = malloc(BUFFER_SIZE);
    if (shared.buff_data_out == NULL) {
        printf(COLOR_RED "[ERROR]\n" COLOR_RESET);
        return -1;
    }
    else
    {
        printf(COLOR_GREEN "[OK]\n" COLOR_RESET);
    }
    // ##########################################


    // INIT SERIAL ##############################
    printf("Opening serial ");
    if ( rs232_init("/dev/ttyUSB0") == -1)
    {
        printf(COLOR_RED "[ERROR]\n" COLOR_RESET);
        return -1;
    }
    else
    {
        printf(COLOR_GREEN "[OK]\n" COLOR_RESET);
    }
    // ##########################################

    // THREAD READ RS232 ########################
    printf("Init thread : serial_read ");
    if ( pthread_create(&rs232_read, NULL, thread_rs232_read, (void *)&shared) ) {
        printf(COLOR_RED "[ERROR]\n" COLOR_RESET);
        return -1;
    }
    else
    {
        printf(COLOR_GREEN "[OK]\n" COLOR_RESET);
    }
    // ##########################################

    // THREAD WIFIBOT READ STREAM ###############
    printf("Init thread : wifibot_read_stream ");
    if ( pthread_create(&wifibot_read_wtream, NULL, thread_wifibot_read_stream, (void *)&shared) ) {
        printf(COLOR_RED "[ERROR]\n" COLOR_RESET);
        return -1;
    }
    else
    {
        printf(COLOR_GREEN "[OK]\n" COLOR_RESET);
    }
    // ##########################################
    
    // THREAD TCP SOCKET ########################
    printf("Init thread : tcp_socket ");
    if ( pthread_create(&tcp_socket, NULL, thread_tcp_socket, (void *)&shared) ) {
        printf(COLOR_RED "[ERROR]\n" COLOR_RESET);
        return -1;
    }
    else
    {
        printf(COLOR_GREEN "[OK]\n" COLOR_RESET);
    }
    // ##########################################

    // SETUP WIFIBOT ############################
    wifibot_set_wtd(100); // 100 milliseconds
    // ##########################################

    set_conio_terminal_mode();

    int run = 1;

    wifibot_cmd.speed_l = 0;
    wifibot_cmd.speed_r = 0;
    shared.cmd_mode = 0;

    while(run) {

        while (!kbhit()) {

            if (shared.cmd_mode == 0)
		wifibot_process(&wifibot_cmd);

            usleep(100000);
        }

        key = (unsigned char) getch();
        if (key == 'c') // exit
            run = 0;

        if (key == 'z' && wifibot_cmd.speed_l < 65500) {
	    
	    shared.cmd_mode = 0;
            wifibot_cmd.speed_l += 500;
            wifibot_cmd.speed_r += 500;

            usleep(50);
        }

        if (key == 's' && wifibot_cmd.speed_l > 0) {

	    shared.cmd_mode = 0;
            wifibot_cmd.speed_l -= 500;
            wifibot_cmd.speed_r -= 500;

            usleep(50);
        }

        // printf("speed: %u\n\r",  wifibot_cmd.speed_l);
        printf("temp:  %f°C\n\r", shared.wifibot_read->temp);
        printf("hygro: %f%%\n\r", shared.wifibot_read->hygro);
    }

    // DEINIT ###################################
    pthread_cancel(rs232_read);
    pthread_cancel(wifibot_read_wtream);
    rs232_deinit();
    free(shared.buff_data_in);
    free(shared.buff_data_out);
    // ##########################################
    return 0;
}

void * thread_rs232_read(void *p_data) {

    printf("Thread rs232_read" COLOR_GREEN " [OK]\n" COLOR_RESET );

    struct shared_t *shared;
    shared = (struct shared_t *) p_data;

    rs232_read(shared);
}

void * thread_wifibot_read_stream(void *p_data) {

    printf("Thread wifibot_read_stream" COLOR_GREEN " [OK]\n" COLOR_RESET );

    struct shared_t *shared;
    shared = (struct shared_t *) p_data;

    wifibot_read_stream(shared);
}

void * thread_tcp_socket(void *p_data) {
	
    printf("Thread tcp_socket" COLOR_GREEN " [OK]\n" COLOR_RESET );

    struct shared_t *shared;
    shared = (struct shared_t *) p_data;
    
    tcp_server(shared);

}

short crc16(unsigned char *adresse_tab , unsigned char taille_max)
{
    unsigned int Crc = 0xFFFF;
    unsigned int Polynome = 0xA001;
    unsigned int CptOctet = 0;
    unsigned int CptBit = 0;
    unsigned int Parity= 0;

    Crc = 0xFFFF;
    Polynome = 0xA001; // Polynôme = 2^15 + 2^13 + 2^0 = 0xA001.

    for ( CptOctet= 0 ; CptOctet < taille_max ; CptOctet++)
    {
        Crc ^= *( adresse_tab + CptOctet); //Ou exculsif entre octet message et CRC

        for ( CptBit = 0; CptBit <= 7 ; CptBit++) /* Mise a 0 du compteur nombre de bits */
        {
            Parity= Crc;
            Crc >>= 1; // Décalage a droite du crc
            if (Parity%2 == 1) Crc ^= Polynome; // Test si nombre impair -> Apres decalage à droite il y aura une retenue
        } // "ou exclusif" entre le CRC et le polynome generateur.
    }
    return(Crc);
}

void wifibot_process(struct wifibot_cmd_t *wifibot) {

    unsigned char buff[30];
    short crc;

// SPEED
    buff[0] = MODBUS_SYNC_BYTE;
    buff[1] = 8;
    buff[2] = MODBUS_CMD_SPEED;
    buff[3] = (unsigned char) (wifibot->speed_l & 0xFF);
    buff[4] = (unsigned char) (wifibot->speed_l>>8 & 0xFF);
    buff[5] = (unsigned char) (wifibot->speed_r & 0xFF);
    buff[6] = (unsigned char) (wifibot->speed_r>>8 & 0xFF);
    buff[7] = 0 + 64 + 0 + 16;

    crc = crc16(buff+1, 7);
    buff[8] = (unsigned char)(crc & 0xFF);
    buff[9] = (unsigned char)((crc>>8) & 0xFF);

    rs232_write(buff, 10);
}
