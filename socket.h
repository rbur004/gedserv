#ifndef SOCKET_H
#define SOCKET_H
#include <stdio.h>

#define TIME_LENGTH 32
#define URL_LIMIT 1024

int main_loop();
void send_error(int sd, int error);
void send_url(int sd, char *url, int headeronly);
void Parse_and_send_response(int sd, char *buf);
void send_ok_header(FILE *fp, char *window);

#endif
