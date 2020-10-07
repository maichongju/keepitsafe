#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "header/utility.h"
#include "header/decrypt.h"
#include "header/encrypt.h"

/**
 * @brief 
 * 
 * @param cipher {char *} cipher
 * @param key {const char*} One time Pad key
 * @param base {const char*} Base string
 * @param desc {char *} Destionation array
 * @param n {size_t} size of the plaintext
 */
char *one_time_pad_dec(char *cipher, char *key, const char *base)
{
    size_t n = strlen(cipher);
    char *desc = calloc(n + 1, sizeof(char));
    size_t basesize = strlen(base);
    size_t index = 0;
    // Clean the desc string.
    desc[0] = '\0';

    // While not reach the end and string did not end
    while (index < n && cipher[index] != '\0')
    {
        int plaintextindex = find_index(base, cipher[index]);
        char plainchar;
        // Current char is not in basestring, do nothing
        if (plaintextindex == -1)
        {
            plainchar = cipher[index];
        }
        else
        {
            int cipherindex = mod((plaintextindex - (int)key[index]), basesize);
            plainchar = base[cipherindex];
        }
        strncat(desc, &plainchar, 1);
        index++;
    }
    return desc;
}

/**
 * @brief Decrypt the cipher using SDES
 * 
 * @param cipher {char*} cipher
 * @param key {char *} key for the decrypt 
 * @param config {SDESconfig*} config for decrypt
 * @return char* plaintext
 */
char *sdes_dec(char *cipher, char *key, SDESconfig *config)
{
    char *plaintext = NULL;
    switch (config->cmode)
    {
    case CBC:
        plaintext = sdes_dec_CBC(cipher, key, config);
        break;
    case ECB:
        plaintext = sdes_dec_ECB(cipher, key, config);
        break;
    case OFB:
        plaintext = sdes_dec_OFB(cipher, key, config);
        break;
    default:
        break;
    }
    return plaintext;
}

char *sdes_dec_ECB(char *cipher, char *key, SDESconfig *config)
{
    if (strlen(cipher) == 0)
    {
        perror("\nERROR (sdes_enc): invalid input\n");
        exit(EXIT_FAILURE);
    }
    else if (strlen(key) != config->keysize)
    {
        perror("\nERROR (sdes_enc): invalid key\n");
        exit(EXIT_FAILURE);
    }

    char *plaintext = NULL;
    char *oldtemp = NULL;
    char *ciphercode = calloc(1, sizeof(char));
    char *tempcipher = NULL;
    if (config->emode == encode_B6)
    {
        hashmap *undefinelist = get_undefined(cipher, BASESTRB6);
        cipher = remove_undefined(cipher, undefinelist);

        /** Encode cipher **/
        char *plaincode = encode(cipher, encode_B6);
        free(cipher);

        size_t plaintcodesize = strlen(plaincode);
        char *codeBlock = calloc(config->blocksize + 1, sizeof(char));
        for (size_t i = 0; i < plaintcodesize; i += config->blocksize)
        {
            if (plaintcodesize < i + config->blocksize)
            {
                size_t codeBlockSize = strlen(plaincode) - i;
                strncpy(codeBlock, plaincode + i, codeBlockSize);
                *(codeBlock + codeBlockSize) = '\0';
            }
            else
            {
                strncpy(codeBlock, plaincode + i, config->blocksize);
            }
            tempcipher = calloc(strlen(codeBlock) + 1, sizeof(char));
            strcpy(tempcipher, codeBlock);
            for (size_t j = config->round; j > 0; j--)
            {
                oldtemp = tempcipher;
                char *subkey = get_subkey(key, j, config);
                tempcipher = feistel(oldtemp, subkey, config);
                free(subkey);
                free(oldtemp);
            }
            int half = config->blocksize / 2;
            oldtemp = ciphercode;
            ciphercode = calloc(strlen(ciphercode) + strlen(tempcipher) + 1, sizeof(char));
            strcpy(ciphercode, oldtemp);
            free(oldtemp);
            strcat(ciphercode, tempcipher + half);
            strncat(ciphercode, tempcipher, half);
            free(tempcipher);
        }
        free(codeBlock);
        plaintext = decode(ciphercode, encode_B6);
        oldtemp = plaintext;
        plaintext = remove_padding(oldtemp, 'Q');
        oldtemp = plaintext;
        plaintext = insert_undefined_list(oldtemp, undefinelist);
        free(oldtemp);
        free(ciphercode);
        hashmap_free(undefinelist);
    }
    return plaintext;
}

char *sdes_dec_CBC(char *cipher, char *key, SDESconfig *config)
{
    if (strlen(cipher) == 0)
    {
        perror("\nERROR (sdes_enc): invalid input\n");
        exit(EXIT_FAILURE);
    }
    else if (strlen(key) != config->keysize)
    {
        perror("\nERROR (sdes_enc): invalid key\n");
        exit(EXIT_FAILURE);
    }

    char *plaintext = NULL;
    char *oldtemp = NULL;
    char *plaincode = calloc(1, sizeof(char));
    char *tempplain = NULL;
    if (config->emode == encode_B6)
    {
        hashmap *undefinelist = get_undefined(cipher, BASESTRB6);
        cipher = remove_undefined(cipher, undefinelist);

        /** Encode cipher **/
        char *ciphercode = encode(cipher, encode_B6);
        free(cipher);

        size_t plaintcodesize = strlen(ciphercode);
        char *codeBlock = calloc(config->blocksize + 1, sizeof(char));
        char *vector = get_IV(config);
        for (size_t i = 0; i < plaintcodesize; i += config->blocksize)
        {
            if (plaintcodesize < i + config->blocksize)
            {
                size_t codeBlockSize = strlen(ciphercode) - i;
                strncpy(codeBlock, ciphercode + i, codeBlockSize);
                *(codeBlock + codeBlockSize) = '\0';
            }
            else
            {
                strncpy(codeBlock, ciphercode + i, config->blocksize);
            }
            tempplain = calloc(strlen(codeBlock) + 1, sizeof(char));
            strcpy(tempplain, codeBlock);
            char *tempvector = calloc(strlen(tempplain) + 1, sizeof(char));
            strcpy(tempvector, tempplain);
            for (size_t j = config->round; j > 0; j--)
            {
                oldtemp = tempplain;
                char *subkey = get_subkey(key, j, config);
                tempplain = feistel(oldtemp, subkey, config);
                free(subkey);
                free(oldtemp);
            }
            int half = config->blocksize / 2;
            oldtemp = tempplain;
            tempplain = calloc(strlen(tempplain) + 1, sizeof(char));
            strcpy(tempplain, oldtemp + half);
            strncat(tempplain, oldtemp, half);
            free(oldtemp);
            oldtemp = tempplain;
            tempplain = xor(oldtemp, vector);
            free(oldtemp);
            oldtemp = plaincode;
            plaincode = calloc(strlen(plaincode) + strlen(tempplain) + 1, sizeof(char));
            strcpy(plaincode, oldtemp);
            strcat(plaincode, tempplain);
            free(tempplain);
            free(vector);
            vector = tempvector;
        }
        free(vector);
        free(codeBlock);
        plaintext = decode(plaincode, encode_B6);
        oldtemp = plaintext;
        plaintext = remove_padding(oldtemp, 'Q');
        oldtemp = plaintext;
        plaintext = insert_undefined_list(oldtemp, undefinelist);
        free(oldtemp);
        free(plaincode);
        hashmap_free(undefinelist);
    }
    return plaintext;
}

char *sdes_dec_OFB(char *cipher, char *key, SDESconfig *config)
{
    return sdes_enc_OFB(cipher, key, config);
}
