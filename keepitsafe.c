#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "header/utility.h"
#include "header/encrypt.h"
#include "header/decrypt.h"

/**
 * @brief Program that will encrypted the given file with 
 * the given encrypted method. If not default method will 
 * be used
 * @author Chongju Mai
 * @version 1.0
 * @date 2020/09/07
 *  
 */
static const char *HELPMSG = "Usage\n\
keepitsafe [ENCRYPT] [CIPHER] -k \"key\" \"input_text\"\n\
    ENCRYPT\n\
        -enc                Run as encryption\n\
        -dec                Run as decryption\n\
    CIPHER (Encryption method)\n\
        -otp                One Time Pad encryption\n\
        -ase256             Advanced Encryption 256 bit\n\
        -rsa                RSA encryption\n\
        -sdes config        Simple Data Encryption Standard. config format\n\
                            rounds,keySize,blockSize,encodeType,cipherType,p,q\n\
                            cipherType: CBC,OFB,ECB\n\
    OPTION\n\
        -h, -help           Display this message\n";
typedef enum
{
    enc,
    dec
} encrypt;
typedef enum
{
    cipher_otp = 8,
    cipher_ase256,
    cipher_rsa,
    cipher_sdes,
    cipher_unknown
} cipher;

typedef struct ARGUMENT
{
    encrypt encrypt;
    cipher cipher;
    SDESconfig *sdesconfig;
    bool displayhelp;
    bool haserror;
    char *error;
    char *errorargument;
    char *errorfilename;
    char *inputtext;
    char *key;
} argument;

/**
 * @brief convert sdes config string to SDESconfig 
 * 
 * @param key {char *} config string
 * @return SDESconfig* sdes configutation 
 */
SDESconfig *convert_key_to_seds(char *key)
{
    SDESconfig *config = malloc(sizeof(SDESconfig));
    config->emode = -1;
    config->cmode = -1;
    char delim[] = ",";

    /* rounds */
    char *token = strtok(key, delim);
    if (token == NULL)
    {
        free(config);
        return NULL;
    }
    config->round = atoi(token);

    /* key Size */
    token = strtok(NULL, delim);
    if (token == NULL)
    {
        free(config);
        return NULL;
    }
    config->keysize = atoi(token);

    /* Block Size */
    token = strtok(NULL, delim);
    if (token == NULL)
    {
        free(config);
        return NULL;
    }
    config->blocksize = atoi(token);

    /* Encode Type */
    token = strtok(NULL, delim);
    if (token == NULL)
    {
        free(config);
        return NULL;
    }
    if (strcmp("B6", token) == 0)
    {
        config->emode = encode_B6;
    }
    else
    {
        config->emode = encode_unknown;
    }

    /* Cipher Type CBC, ECB, OFB */
    token = strtok(NULL, delim);
    if (token == NULL)
    {
        free(config);
        return NULL;
    }
    if (strcmp("CBC", token) == 0)
    {
        config->cmode = CBC;
    }
    else if (strcmp("ECB", token) == 0)
    {
        config->cmode = ECB;
    }
    else if (strcmp("OFB", token) == 0)
    {
        config->cmode = OFB;
    }
    else
    {
        config->emode = unknown;
    }

    /* p */
    token = strtok(NULL, delim);
    if (token == NULL)
    {
        free(config);
        return NULL;
    }
    config->p = atoi(token);

    /* q */
    token = strtok(NULL, delim);
    if (token == NULL)
    {
        free(config);
        return NULL;
    }
    config->q = atoi(token);
    return config;
}

/**
 * @brief Process the user input to argument 
 * 
 * @param argc {int} number arguments
 * @param argv {char *[]} argument lists 
 * @return argument* argument object
 */
argument *processargument(int argc, char *argv[])
{
    // malloc argument
    argument *arg = malloc(sizeof(argument));
    arg->encrypt = -1;
    arg->cipher = -1;
    arg->displayhelp = false;
    arg->haserror = false;
    arg->error = NULL;
    arg->errorargument = NULL;
    arg->inputtext = NULL;
    arg->key = NULL;
    arg->sdesconfig = NULL;

    if (argc == 1)
    {
        arg->haserror = true;
        arg->error = "Missing encrytion/decryption.";
    }
    else
    {
        int current = 1;
        while (current < argc)
        {
            // Check all the input
            if (strcmp("-enc", argv[current]) == 0)
            {
                arg->encrypt = enc;
            }
            else if (strcmp("-dec", argv[current]) == 0)
            {
                arg->encrypt = dec;
            }
            else if (strcmp("-otp", argv[current]) == 0)
            {
                arg->cipher = cipher_otp;
            }
            else if (strcmp("-ase256", argv[current]) == 0)
            {
                arg->cipher = cipher_ase256;
            }
            else if (strcmp("-sdes", argv[current]) == 0)
            {
                arg->cipher = cipher_sdes;
                current++;
                if (current < argc)
                {
                    arg->sdesconfig = convert_key_to_seds(argv[current]);
                    if (arg->sdesconfig == NULL)
                    {
                        arg->haserror = true;
                        arg->error = "invalid SDES config";
                    }
                }
                else
                {
                    arg->haserror = true;
                    arg->error = "missing -sdes value";
                }
            }

            else if (strcmp("-k", argv[current]) == 0)
            {
                current++;
                if (current < argc)
                {
                    arg->key = argv[current];
                    current++;
                    if (current < argc)
                    {
                        arg->inputtext = argv[current];
                    }
                    else
                    {
                        arg->haserror = true;
                        arg->error = "missing input text";
                    }
                }
                else
                {
                    arg->haserror = true;
                    arg->error = "missing -k value";
                }
            }
            else if (strcmp("-h", argv[current]) == 0 || strcmp("-help", argv[current]) == 0)
            {
                arg->displayhelp = true;
            }
            else
            {
                arg->haserror = true;
                arg->error = "unknow argument";
                arg->errorargument = argv[current];
                break;
            }
            current++;
        }
    }

    return arg;
}

/**
 * @brief Validate generated argument. If file is given,
 * it will also check if the file is aviliable
 * 
 * @param arg {argument *} 
 */
void argumentvalid(argument *arg)
{
    if (!arg->haserror)
    {
        if (arg->encrypt == -1)
        {
            arg->haserror = true;
            arg->error = "Missing Encrytion type (-enc/-dec)";
        }
        else if (arg->cipher == -1)
        {
            arg->haserror = true;
            arg->error = "Missing cipher";
        }
        else if (arg->inputtext == NULL)
        {
            arg->haserror = true;
            arg->error = "No content given for encryption/decryption";
        }
        /* SDES config validate */
        else if (arg->cipher == cipher_sdes)
        {
            SDESconfig *config = arg->sdesconfig;
            if (config->round < 2 || config->round > 10)
            {
                arg->haserror = true;
                arg->error = "Invalid SDES round (2~10)";
            }
            else if (config->keysize < 1)
            {
                arg->haserror = true;
                arg->error = "Invalid SDES key size (10~30)";
            }
            else if (config->blocksize < 1)
            {
                arg->haserror = true;
                arg->error = "Invalid SDES block size";
            }
            else if (config->emode == encode_unknown)
            {
                arg->haserror = true;
                arg->error = "Invalid SDES encode mode";
            }
            else if (config->cmode == unknown)
            {
                arg->haserror = true;
                arg->error = "Invalid SDES cipher mode";
            }
            else if (!is_prime(config->q))
            {
                arg->haserror = true;
                arg->error = "Invalid SDES p require prime number";
            }
            else if (!is_prime(config->q))
            {
                arg->haserror = true;
                arg->error = "Invalid SDES p require prime number";
            }
        }
        else if (arg->key == NULL)
        {
            arg->haserror = true;
            arg->error = "Key is require";
        }
    }
}

/**
 * @brief Free arguemnt object
 * 
 * @param argu 
 */
void argu_free(argument *argu)
{
    if (argu->sdesconfig != NULL)
    {
        free(argu->sdesconfig);
    }
    free(argu);
}

int main(int arg, char *argv[])
{
    argument *argu = processargument(arg, argv);
    if (argu->displayhelp)
    {
        printf("\n%s\n", HELPMSG);
        return 0;
    }
    argumentvalid(argu);

    // Error detected
    if (argu->haserror)
    {
        printf("Error: %s", argu->error);
        // Unknown argument
        if (argu->errorargument != NULL)
        {
            printf("%s", argu->errorargument);
        }
        // File not found
        else if (argu->errorfilename != NULL)
        {
            printf("%s", argu->errorfilename);
        }
        printf("\n\n%s\n", HELPMSG);
    }
    else
    {
        char *output = NULL;
        switch (argu->encrypt)
        {
        case enc:
            switch (argu->cipher)
            {
            case cipher_otp:
            {
                output = one_time_pad_enc(argu->inputtext, argu->key, BASESTR);
                break;
            }
            case cipher_sdes:
            {
                /* SDES configuration */
                SDESconfig *config = argu->sdesconfig;
                /* encrypt from command line */
                if (argu->inputtext != NULL)
                {
                    output = sdes_enc(argu->inputtext, argu->key, config);
                }
                break;
            }
            default:
                break;
            }
            break;
        case dec:
            switch (argu->cipher)
            {
            case cipher_otp:
            {
                output = one_time_pad_dec(argu->inputtext, argu->key, BASESTR);
                break;
            }
            case cipher_sdes:
            {
                SDESconfig *config = argu->sdesconfig;
                if (argu->inputtext != NULL)
                {
                    output = sdes_dec(argu->inputtext, argu->key, config);
                }
                break;
            }
            default:
                break;
            }
            break;
        default:
            break;
        }
        if (output != NULL)
        {
            printf("%s\n", output);
            free(output);
        }
    }
    argu_free(argu);

    return 0;
}