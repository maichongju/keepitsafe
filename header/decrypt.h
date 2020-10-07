#ifndef DECRYPT_H
#define DECRYPT_H

char *one_time_pad_dec(char *cipher, char *key, const char *base);
char *sdes_dec(char *cipher, char *key, SDESconfig *config);
char *sdes_dec_CBC(char *cipher, char *key, SDESconfig *config);
char *sdes_dec_ECB(char *cipher, char *key, SDESconfig *config);
char *sdes_dec_OFB(char *cipher, char *key, SDESconfig *config);
#endif