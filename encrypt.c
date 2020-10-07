/**
 * @file encrypt.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-09-22
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "header/utility.h"
#include "header/hashmap.h"
#include "header/encrypt.h"

/**
 * @brief One time pad encryption, if the given char does 
 * not include in the base. It will be ingone. #plaintext <= 
 * #key. cipher = (index of plaintext in basestring + ASCII of 
 * key) % basestring size
 * 
 * @param plaintext {const char*} plaintext 
 * @param key {const char*} One time Pad key
 * @param base {const char*} Base string
 * @return char* cipher
 * 
 */
char *one_time_pad_enc(char *plaintext, char *key, const char *base)
{
    size_t n = strlen(plaintext);
    char *desc = calloc(n + 1, sizeof(char));
    size_t basesize = strlen(base);
    size_t index = 0;

    // While not reach the end and string did not end
    while (index < n && plaintext[index] != '\0')
    {
        int plaintextindex = find_index(base, plaintext[index]);
        char cipherchar;
        // Current char is not in basestring, do nothing
        if (plaintextindex == -1)
        {
            cipherchar = plaintext[index];
        }
        else
        {
            int cipherindex = (plaintextindex + (int)key[index]) % basesize;
            cipherchar = base[cipherindex];
        }
        strncat(desc, &cipherchar, 1);
        index++;
    }
    return desc;
}

void ase256enc() {}

/**
 * @brief SDES encryption 
 * 
 * @param plaintext {char *} plaintext
 * @param key {char *} key for encryption, binary
 * @param config {SDESconfig *} sdes config 
 * @return char* cipher
 */
char *sdes_enc(char *plaintext, char *key, SDESconfig *config)
{
    char *cipher = NULL;
    switch (config->cmode)
    {
    case ECB:
        cipher = sdes_enc_ECB(plaintext, key, config);
        break;
    case CBC:
        cipher = sdes_enc_CBC(plaintext, key, config);
        break;
    case OFB:
        cipher = sdes_enc_OFB(plaintext, key, config);
        break;
    default:
        break;
    }
    return cipher;
}

/**
 * @brief SDES encryption using ECB
 * 
 * @param plaintext {char *} plaintext
 * @param key {char *} key for encryption, binary
 * @param config {SDESconfig *} sdes config 
 * @return char* cipher
 */
char *sdes_enc_ECB(char *plaintext, char *key, SDESconfig *config)
{
    if (strlen(plaintext) == 0)
    {
        perror("\nERROR (sdes_enc): invalid input\n");
        exit(EXIT_FAILURE);
    }
    else if (strlen(key) != config->keysize)
    {
        perror("\nERROR (sdes_enc): invalid key\n");
        exit(EXIT_FAILURE);
    }

    char *cipher = NULL;
    char *oldtemp = NULL;
    char *ciphercode = calloc(1, sizeof(char));
    char *tempcipher = NULL;
    if (config->emode == encode_B6)
    {
        int blockChar = config->blocksize / 6;
        hashmap *undefinelist = get_undefined(plaintext, BASESTRB6);
        plaintext = remove_undefined(plaintext, undefinelist);

        /** Add padding **/
        if (strlen(plaintext) % blockChar != 0)
        {
            oldtemp = plaintext;
            size_t paddingsize = blockChar - strlen(plaintext) % blockChar;
            plaintext = calloc(strlen(plaintext) + paddingsize + 1, sizeof(char));
            strcpy(plaintext, oldtemp);
            free(oldtemp);
            char paddingChar = 'Q';
            for (size_t i = 0; i < paddingsize; i++)
            {
                strncat(plaintext, &paddingChar, 1);
            }
        }

        /** Encode plaintext **/
        char *plaincode = encode(plaintext, encode_B6);
        free(plaintext);

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
            for (size_t j = 0; j < config->round; j++)
            {
                oldtemp = tempcipher;
                char *subkey = get_subkey(key, j + 1, config);
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
        cipher = decode(ciphercode, encode_B6);
        oldtemp = cipher;
        cipher = insert_undefined_list(oldtemp, undefinelist);
        free(oldtemp);
        free(ciphercode);
        hashmap_free(undefinelist);
    }
    return cipher;
}

/**
 * @brief SDES encryption using CBC
 * 
 * @param plaintext {char *} plaintext
 * @param key {char *} key for encryption, binary
 * @param config {SDESconfig *} sdes config 
 * @return char* cipher
 */
char *sdes_enc_CBC(char *plaintext, char *key, SDESconfig *config)
{
    if (strlen(plaintext) == 0)
    {
        perror("\nERROR (sdes_enc): invalid input\n");
        exit(EXIT_FAILURE);
    }
    else if (strlen(key) != config->keysize)
    {
        perror("\nERROR (sdes_enc): invalid key\n");
        exit(EXIT_FAILURE);
    }

    char *cipher = NULL;
    char *oldtemp = NULL;
    char *ciphercode = calloc(1, sizeof(char));
    char *tempcipher = NULL;
    if (config->emode == encode_B6)
    {
        int blockChar = config->blocksize / 6;
        hashmap *undefinelist = get_undefined(plaintext, BASESTRB6);
        plaintext = remove_undefined(plaintext, undefinelist);

        /** Add padding **/
        if (strlen(plaintext) % blockChar != 0)
        {
            oldtemp = plaintext;
            size_t paddingsize = blockChar - strlen(plaintext) % blockChar;
            plaintext = calloc(strlen(plaintext) + paddingsize + 1, sizeof(char));
            strcpy(plaintext, oldtemp);
            free(oldtemp);
            char paddingChar = 'Q';
            for (size_t i = 0; i < paddingsize; i++)
            {
                strncat(plaintext, &paddingChar, 1);
            }
        }

        /** Encode plaintext **/
        char *plaincode = encode(plaintext, encode_B6);
        free(plaintext);

        size_t plaintcodesize = strlen(plaincode);
        char *codeBlock = calloc(config->blocksize + 1, sizeof(char));
        char *vector = get_IV(config);
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
            oldtemp = tempcipher;
            tempcipher = xor(oldtemp, vector);
            free(oldtemp);
            for (size_t j = 0; j < config->round; j++)
            {
                oldtemp = tempcipher;
                char *subkey = get_subkey(key, j + 1, config);
                tempcipher = feistel(oldtemp, subkey, config);
                free(subkey);
                free(oldtemp);
            }
            int half = config->blocksize / 2;
            oldtemp = ciphercode;
            free(vector);
            vector = calloc(strlen(tempcipher) + 1, sizeof(char));
            strcat(vector, tempcipher + half);
            strncat(vector, tempcipher, half);
            ciphercode = calloc(strlen(ciphercode) + strlen(tempcipher) + 1, sizeof(char));
            strcpy(ciphercode, oldtemp);
            free(oldtemp);
            strcat(ciphercode, vector);
            free(tempcipher);
        }
        free(codeBlock);
        cipher = decode(ciphercode, encode_B6);
        oldtemp = cipher;
        cipher = insert_undefined_list(oldtemp, undefinelist);
        free(oldtemp);
        free(ciphercode);
        hashmap_free(undefinelist);
    }
    return cipher;
}

/**
 * @brief SDES encryption using OFB
 * 
 * @param plaintext {char *} plaintext
 * @param key {char *} key for encryption, binary
 * @param config {SDESconfig *} sdes config 
 * @return char* cipher
 */
char *sdes_enc_OFB(char *plaintext, char *key, SDESconfig *config)
{
    if (strlen(plaintext) == 0)
    {
        perror("\nERROR (sdes_enc): invalid input\n");
        exit(EXIT_FAILURE);
    }
    else if (strlen(key) != config->keysize)
    {
        perror("\nERROR (sdes_enc): invalid key\n");
        exit(EXIT_FAILURE);
    }

    char *cipher = NULL;
    char *oldtemp = NULL;
    char *ciphercode = calloc(1, sizeof(char));
    if (config->emode == encode_B6)
    {
        // int blockChar = config->blocksize / 6;
        hashmap *undefinelist = get_undefined(plaintext, BASESTRB6);
        plaintext = remove_undefined(plaintext, undefinelist);

        /** Encode plaintext **/
        char *plaincode = encode(plaintext, encode_B6);
        free(plaintext);

        size_t plaintcodesize = strlen(plaincode);
        char *codeBlock = calloc(config->blocksize + 1, sizeof(char));
        char *vector = get_IV(config);
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

            for (size_t j = 0; j < config->round; j++)
            {
                oldtemp = vector;
                char *subkey = get_subkey(key, j + 1, config);
                vector = feistel(oldtemp, subkey, config);
                free(subkey);
                free(oldtemp);
            }
            int half = config->blocksize / 2;

            oldtemp = vector;
            vector = calloc(strlen(vector) + 1, sizeof(char));
            strcpy(vector, oldtemp + half);
            strncat(vector, oldtemp, half);
            free(oldtemp);

            oldtemp = ciphercode;
            ciphercode = calloc(strlen(ciphercode) + strlen(codeBlock) + 1, sizeof(char));
            strcpy(ciphercode, oldtemp);
            free(oldtemp);
            if (strlen(codeBlock) == config->blocksize)
            {
                char *ciphertemp = xor(codeBlock, vector);
                strcat(ciphercode, ciphertemp);
                free(ciphertemp);
            }
            else
            {
                char *tempvector = calloc(strlen(codeBlock) + 1, sizeof(char));
                strncpy(tempvector, vector, strlen(codeBlock));
                char *ciphertemp = xor(codeBlock, tempvector);
                strcat(ciphercode, ciphertemp);
                free(tempvector);
                free(ciphertemp);
            }
        }
        free(vector);
        free(codeBlock);
        cipher = decode(ciphercode, encode_B6);
        oldtemp = cipher;
        cipher = insert_undefined_list(oldtemp, undefinelist);
        free(oldtemp);
        free(ciphercode);
        hashmap_free(undefinelist);
    }
    return cipher;
}
