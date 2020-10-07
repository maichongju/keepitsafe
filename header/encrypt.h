#ifndef ENCRYPT_H
#define ENCRYPT_H

char *one_time_pad_enc(char *plaintext, char *key, const char *base );
void ase256enc();
char *sdes_enc(char *plaintext, char *key, SDESconfig *config);
char *sdes_enc_ECB(char *plaintext, char *key, SDESconfig *config);
char *sdes_enc_CBC(char *plaintext, char *key, SDESconfig *config);
char *sdes_enc_OFB(char *plaintext, char *key, SDESconfig *config);

#endif