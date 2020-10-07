#ifndef UTILITY_H
#define UTILITY_H
#include <stdbool.h>
#include "hashmap.h"

static const char BASESTR[] = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
static const char BASESTRB6[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \n";
static const int SBOX1SEED = 12138;
static const int SBOX2SEED = 12580;
static const char PRIMEFILE[] = "primes.txt";
typedef enum CIPHERMODE
{
    ECB = 16,
    CBC,
    OFB,
    unknown
} cipher_mode;

typedef enum ENCODEMODE
{
    encode_B6 = 4,
    encode_unknown
} encode_mode;

typedef struct SDESCONFIG
{
    size_t blocksize;
    size_t round;
    size_t keysize;
    encode_mode emode;
    cipher_mode cmode;
    int p;
    int q;
} SDESconfig;

bool file_exist(const char *filepath);

int find_index(const char target[], char element);

char *key_generate(const char base[], size_t size);

bool isbinary(char *text);
char *decimal_to_binary(int decimal, size_t size);
int binary_to_decimal(char *binary);
int mod(int a,int b);
char *encode(char *text, encode_mode emode);
char *decode(char *text, encode_mode emode);
char *encodeB6(char text);
char decodeB6(char *text);
hashmap *get_undefined(char *src, const char *base);
char *remove_undefined(char *src, hashmap *map);
char *insert_undefined_list(char *src, hashmap *ulist);
char *remove_front_back(char *input);

char *shift_string(char *str, int n, char d);
char *xor(char *a, char *b);
char *remove_padding(char *text, char padding);
bool is_prime(int num);

// SDES
char *blum(int p, int q, int m);
char *get_subkey(char *key, int i, SDESconfig *config);
char *expand(char *binary);
char *sdesf(char *ri, char *ki, SDESconfig *config);
char **getsbox(int sboxnum, size_t size);
char **newsbox(int sboxnum);
char *getsbox1(char *r, SDESconfig *config);
char *getsbox2(char *r, SDESconfig *config);
void get_randomnum_list(int desc[], size_t size, int seed);
void free_sbox(char **sbox, size_t size);
char *feistel(char *bi, char *ki, SDESconfig *config);
char *get_IV(SDESconfig *config);
#endif