#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "header/utility.h"
#include "header/hashmap.h"

/**
 * @brief Check if the given file path exist
 * 
 * @param filepath {const char *} filepath for the file 
 * @return true file exist
 * @return false file does not exist
 */
bool file_exist(const char *filepath)
{
    if (access(filepath, F_OK) == -1)
        return false;

    return true;
}

/**
 * @brief Find index of the given element, if not found,
 * -1 will return 
 * 
 * @param target {const char[]} target array
 * @param element target element
 * @return int index of element, -1 if element is not found
 */
int find_index(const char target[], char element)
{
    size_t n = strlen(target);
    size_t index = 0;

    while (index < n && target[index] != element)
        index++;

    return index == n ? -1 : index;
}

/**
 * @brief generate a random key. This function use rand() to 
 * generate key, it is pseudo random. 
 * 
 * @param base {const *char} base string 
 * @param desc {const *char} destionation char array
 * @param size {size_t} size of the generated key
 */
char *key_generate(const char base[], size_t size)
{
    char *desc = calloc(size + 1, sizeof(char));
    size_t basesize = strlen(base);
    srand(time(NULL));
    size_t i = 0;
    while (i < size)
    {
        int ran = rand() % basesize;
        char ch = base[ran];
        strncat(desc, &ch, 1);
        i++;
    }
    return desc;
}

/**
 * @brief Remove the first and last char from the list
 * 
 * @param input {char *} input text
 * @return char* 
 */
char *remove_front_back(char *input)
{
    if (strlen(input) < 3)
    {
        return NULL;
    }
    char *result = calloc(strlen(input) - 1, sizeof(char));
    strncpy(result, input + 1, strlen(input) - 2);
    return result;
}

/**
 * @brief modole for a % b
 * 
 * @param a {int} int a
 * @param b {int} int b
 * @return int model result
 */
int mod(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

/**
 * @brief Determind if the given string is binary
 * 
 * @param text text need to convert
 * @return true text is binary
 * @return false text is not binary
 */
bool isbinary(char *text)
{
    char *current = text;
    if (text == NULL || *text == '\0')
    {
        return false;
    }
    while (current != NULL && *current != '\0')
    {
        if (*current != '1' && *current != '0')
        {
            return false;
        }
        current++;
    }
    return true;
}

/**
 * @brief Convert the given decimal to given size of binary,
 * If overflow, no output to binary.
 * 
 * @param decimal {int} input decimal 
 * @param size {size_t} size of the binary
 */
char *decimal_to_binary(int decimal, size_t size)
{
    char *binary = calloc(size + 1, sizeof(char));
    int i = 0;
    int q = 1;
    int r = 0;
    char temp[size + 1];
    char tchar;
    temp[0] = '\0';
    bool overflow = false;
    while (q != 0)
    {
        q = decimal / 2;
        r = decimal % 2;
        decimal = q;
        if (i == size)
        {
            overflow = true;
            break;
        }
        tchar = r + '0';
        strncat(temp, &tchar, 1);
        i++;
    }
    if (!overflow)
    {
        // Add place holder 0
        while (i != size)
        {
            strcat(temp, "0");
            i++;
        }

        // Reverse to the correct
        i = size - 1;
        binary[0] = '\0';
        while (i >= 0)
        {
            strncat(binary, &temp[i], 1);
            i--;
        }
    }
    return binary;
}

/**
 * @brief Convert binary to decimal (unsign)
 * 
 * @param binary {char *} source binary
 * @return int converted integer
 */
int binary_to_decimal(char *binary)
{
    if (isbinary(binary))
    {
        int value = 0;
        int exponent = strlen(binary) - 1;
        char *current = binary;
        while (current != NULL && *current != '\0')
        {
            if (*current == '1')
            {
                value += pow(2, exponent);
            }
            exponent--;
            current++;
        }

        return value;
    }
    return -1;
}

/**
 * @brief encode the given text using given encode mode
 * 
 * @param text {char *} text need to encode
 * @param emode {enum encodemode} encode mode
 * @return desc {char *} encoded string, if unknow char detected
 * it will return NULL
 */
char *encode(char *text, encode_mode emode)
{
    char *desc = NULL;
    switch (emode)
    {
    case encode_B6:
    {
        desc = calloc(6 * strlen(text) + 1, sizeof(char));
        while (text != NULL && *text != '\0')
        {
            if (find_index(BASESTRB6, *text) == -1)
            {
                free(desc);
                desc = NULL;
                break;
            }
            char *bin = encodeB6(*text);
            strcat(desc, bin);
            free(bin);
            text++;
        }

        break;
    }

    default:
        perror("\nError: Unknow encode method \n");
        exit(EXIT_FAILURE);
        break;
    }
    return desc;
}

/**
 * @brief decode the given char using given encode mode
 * 
 * @param text {char *} text need to decode
 * @param emode {enum encodemode} encode mode
 * @return desc {char *} decoded string, if non-binary 
 * number appear it will return NULL
 */
char *decode(char *text, encode_mode emode)
{
    char *desc = NULL;
    switch (emode)
    {
    case encode_B6:
        if (strlen(text) % 6 == 0 && isbinary(text))
        {
            desc = calloc(strlen(text) / 6 + 1, sizeof(char));
            char *current = text;
            char binary[7];
            binary[6] = '\0';
            while (*current != '\0')
            {
                strncpy(binary, current, 6);
                char decchar = decodeB6(binary);
                strncat(desc, &decchar, 1);
                current += 6;
            }
        }

        break;

    default:
        printf("\nError: Unknow encode method \n");
        break;
    }
    return desc;
}

/**
 * @brief Encode the given text using B6 encode.
 * 
 * @param text {char *} text need to encode
 * @return {char *} binary of the text
 */
char *encodeB6(char text)
{
    int index = find_index(BASESTRB6, text);
    if (index != -1)
    {
        char *binary = decimal_to_binary(index, 6);
        return binary;
    }
    return NULL;
}

/**
 * @brief Decode the given binary usiung B6 encode. desc must
 * have tnought space to store
 * 
 * @param text {char *} binary need to decode
 * @param desc {char *} desctionation of the decode
 */
char decodeB6(char *text)
{
    if (isbinary(text))
    {
        int value = binary_to_decimal(text);
        char dchar = BASESTRB6[value];
        return dchar;
    }
    else
    {
        perror("\n ERROR (B6decode) invalid input");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Get the undefined object
 * 
 * @param src {char*} source string
 * @param base {char*} base string
 * @return hashmap* 
 */
hashmap *get_undefined(char *src, const char *base)
{
    char *current = src;
    size_t index = 0;
    hashmap *map = hashmap_create(type_int, type_char);
    while (*current != '\0')
    {
        if (find_index(base, *current) == -1)
        {
            hashmap_insert(map, (void *)&index, (void *)current);
        }
        index++;
        current++;
    }
    return map;
}

/**
 * @brief remove undefined from the src
 * 
 * @param src {char*} source string
 * @param map 
 * @return char* 
 */
char *remove_undefined(char *src, hashmap *map)
{
    char *newstr;
    // Nothing need to change
    if (map->size == 0)
    {
        newstr = calloc(strlen(src) + 1, sizeof(char));
        strcpy(newstr, src);
    }
    else
    {
        size_t newstrlen = strlen(src) - map->size;
        newstr = calloc(newstrlen + 1, sizeof(char));
        hashmapnode *undefinecurrent = map->head;
        for (size_t i = 0; i < strlen(src); i++)
        {
            if (undefinecurrent == NULL)
            {
                strcat(newstr, src + i);
                break;
            }
            if (i == undefinecurrent->key->i)
            {
                undefinecurrent = undefinecurrent->next;
            }
            else
            {
                strncat(newstr, src + i, 1);
            }
        }
    }

    return newstr;
}

/**
 * @brief Insert the undefined character to the source 
 * 
 * @param src {char *} source string
 * @param ulist {hashmap*} contain all the undefinded char
 * @return char* complete string 
 */
char *insert_undefined_list(char *src, hashmap *ulist)
{
    if (ulist->type_val != type_char || ulist->type_key != type_int)
    {
        perror("invalid hashmap\n");
        return NULL;
    }
    else if (src == NULL)
    {
        perror("invalid input");
        return NULL;
    }

    char *newstr;
    if (ulist->size == 0)
    {
        newstr = calloc(strlen(src) + 1, sizeof(char));
        strcpy(newstr, src);
    }
    else
    {
        size_t newstrsize = strlen(src) + ulist->size;
        size_t srcindex = 0;
        newstr = calloc(newstrsize + 1, sizeof(char));
        hashmapnode *undefindedcurrent = ulist->head;
        for (size_t i = 0; i < newstrsize; i++)
        {
            if (undefindedcurrent == NULL)
            {

                strcat(newstr, src + srcindex);
                break;
            }
            if (i == undefindedcurrent->key->i)
            {
                strncat(newstr, &undefindedcurrent->val->c, 1);
                undefindedcurrent = undefindedcurrent->next;
            }
            else
            {
                strncat(newstr, src + srcindex, 1);
                srcindex++;
            }
        }
    }
    return newstr;
}

/**
 * @brief Shift the given string by n shfits (circular shift)
 * where d is the direction of shift
 * 
 * @param str {char *}input string
 * @param n {size_t} number of shifts
 * @param d {char} direction of shift
 */
char *shift_string(char *str, int n, char d)
{
    if (d != 'r' && d != 'l')
    {
        perror("\n ERROR (shiftstring) invalid direction \n");
        exit(EXIT_FAILURE);
    }
    if (n < 0)
    {
        n *= -1;
        d = d == 'r' ? 'l' : 'r';
    }
    size_t len = strlen(str);
    // char tempstr1[len + 1], tempstr2[len + 1];
    // tempstr1[0] = '\0';
    // tempstr2[0] = '\0';
    n = n % len;
    char *result = calloc(strlen(str) + 1, sizeof(char));

    if (*str != '\0' || n > 0)
    {
        if (d == 'l')
        {
            strncpy(result, str + n, len - n);
            strncpy(result + len - n, str, n);
            result[len] = '\0';
        }
        else
        {
            strncpy(result, str - n + len, n);
            strncpy(result + n, str, len - n);

            result[len] = '\0';
        }
    }
    else
    {
        strcpy(result, str);
    }

    return result;
}

/**
 * @brief Used Blum Blum Shub Random Generation to generates
 * a random stream of bits of size m. The seed is the nth
 * primt number, where n = p*q
 * 
 * @param p {int}
 * @param q {int}
 * @param m {int}
 * @return char* 
 */
char *blum(int p, int q, int m)
{
    // Error handling
    if (p % 4 != 3 || q % 4 != 3)
    {
        perror("\nERROR (blum): p, q not valid\n");
        exit(EXIT_FAILURE);
    }
    else if (m <= 0)
    {
        perror("\nERROR (blum): invalid m\n");
        exit(EXIT_FAILURE);
    }

    // get the nth prime number
    int n = p * q;
    // Read from file (refer man getline())
    FILE *primestream = fopen(PRIMEFILE, "r");

    if (primestream == NULL)
    {
        perror("\n ERROR (blum): prime.txt not found \n");
        exit(EXIT_FAILURE);
    }
    size_t i = 0, bufsize = 10;
    char buffer[10];
    while (i < n - 1 && fgets(buffer, bufsize, primestream) != NULL)
    {
        i++;
    }

    if (fgets(buffer, bufsize, primestream) == NULL)
    {
        perror("\n ERROR: invalid p,q \n");
        exit(EXIT_FAILURE);
    }

    int seed = atoi(buffer);

    // Can not convert to number
    if (seed == 0)
    {
        perror("\n ERROR: invalid prime.txt \n");
        exit(EXIT_FAILURE);
    }

    fclose(primestream);

    char *bitstream = calloc(m + 1, sizeof(char));
    // Need to use long, otherwise it will overflow
    long x = (long)pow(seed, 2) % n;

    for (size_t i = 0; i < m; i++)
    {
        x = (long)pow(x, 2) % n;
        // Convert the bit to char;
        char bit = x % 2 + '0';
        strncat(bitstream, &bit, 1);
    }

    return bitstream;
}

/**
 * @brief Generates a subkey for ith round in SDES.
 * The sub-key is one character shorter than original key size
 * Sub-key is generated by circular shift of key with value 1
 * The last bit will be drop
 * 
 * @param key {char *} key for generation
 * @param i {size_t} ith round
 * @return char* 
 */
char *get_subkey(char *key, int i, SDESconfig *config)
{
    if (!isbinary(key) || strlen(key) != config->keysize)
    {
        fprintf(stderr, "\n ERROR (getsubkey): invalid key (%s)\n", key);
        exit(EXIT_FAILURE);
    }
    else if (i <= 0)
    {
        perror("\nERROR (getsubkey): invalid round \n");
        exit(EXIT_FAILURE);
    }
    char *subKey = shift_string(key, i - 1, 'l');
    subKey[strlen(subKey) - 1] = '\0';
    return subKey;
}

/**
 * @brief Remove the padding from the text. Return a new string
 * 
 * @param text {char*} input text
 * @param padding {char} padding remove
 * @return char* new string without padding 
 */
char *remove_padding(char *text, char padding)
{
    size_t textsize = strlen(text);
    int paddingsize = 0;
    for (size_t i = textsize - 1; i >= 0; i--)
    {
        if (*(text + i) == padding)
        {
            paddingsize++;
        }
        else
        {
            break;
        }
    }
    char *newtext = calloc(textsize - paddingsize + 1, sizeof(char));
    strncpy(newtext, text, textsize - paddingsize);
    return newtext;
}

/**
 * @brief Check if the given number is a prime number
 * 
 * @param num {int} input number
 * @return true input is a prime number
 * @return false input is not a prime number 
 */
bool is_prime(int num)
{
    if (num <= 1)
    {
        return false;
    }
    bool result = true;
    for (size_t i = 2; i <= num / 2; i++)
    {
        if (num % i == 0)
        {
            result = false;
            break;
        }
    }

    return result;
}

/**
 * @brief Expand the input binary number by adding two digits
 * The input binary number should be an even number >= 6
 * Expansion works as the following:
 * If the index of the two middle elements is i and i+1
 * From indices 0 up to i-1: same order
 * middle becomes: R(i+1)R(i)R(i+1)R(i)
 * From incides R(i+2) to the end: same order
 * 
 * @param binary {char *} bin
 * @return desc {char *} desctionaltion 
 */
char *expand(char *binary)
{
    if (isbinary(binary) && strlen(binary) > 5 && strlen(binary) % 2 == 0)
    {
        char *desc = calloc(strlen(binary) + 2 + 1, sizeof(char));
        int m = strlen(binary) / 2 - 1;
        char *b = binary;
        strncat(desc, b, m);
        strncat(desc, b + m + 1, 1);
        strncat(desc, b + m, 1);
        strncat(desc, b + m + 1, 1);
        strncat(desc, b + m, 1);
        strcat(desc, b + m + 2);
        return desc;
    }
    else
    {
        perror("\nERROR (expand): invalid input \n");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Performs the following tasks: 1. expand(Ri)
 * 2. xor step 1 with ki
 * 3. divide output of 2 into two equal block
 * 4. pass the most significant bits of 3 to sbox1
 * and lest significant bit to sbox2
 * 5. conactenate the output of 4 to [sbox1][sbox2]
 * 
 * @param ri {char *}block of binary numbers
 * @param ki {char *} binary number representing subkey
 * @return {char *} block of binary numbers 
 */
char *sdesf(char *ri, char *ki, SDESconfig *config)
{
    if (!isbinary(ki) || strlen(ki) != config->keysize - 1)
    {
        perror("\nERROR (F): invalid key\n");
        exit(EXIT_FAILURE);
    }
    else if (!isbinary(ri) || strlen(ri) != config->blocksize / 2)
    {
        perror("\nERROR (F): invalid input\n");
        exit(EXIT_FAILURE);
    }
    char *Ri = expand(ri);
    char *nRi = xor(Ri, ki);
    free(Ri);
    int half = strlen(nRi) / 2;
    char sbox1ri[half + 1], sbox2ri[strlen(nRi) - half + 1];
    sbox1ri[half] = '\0';
    sbox2ri[half] = '\0';
    strncpy(sbox1ri, nRi, half);
    strcpy(sbox2ri, nRi + half);
    free(nRi);
    char *sBox1 = getsbox1(sbox1ri, config);
    char *sBox2 = getsbox2(sbox2ri, config);
    char *result = calloc(strlen(sBox1) + strlen(sBox2) + 1, sizeof(char));
    strcpy(result, sBox1);
    strcat(result, sBox2);
    free(sBox1);
    free(sBox2);
    return result;
}

/**
 * @brief get the sbox
 * 
 * @param sboxnum {int} sbox number
 * @param size {size_t} size of the sbox
 * @return char** generated sbox
 */
char **getsbox(int sboxnum, size_t size)
{

    int sizebasic = pow(2, size - 1);
    char basic[sizebasic][size];

    int index = 0;
    for (index = 0; index < sizebasic; index++)
    {
        char *bin = decimal_to_binary(index, size - 1);
        strcpy(basic[index], bin);
        free(bin);
    }

    char **sbox = calloc(sizebasic * 2, sizeof(char *));
    for (size_t i = 0; i < sizebasic * 2; i++)
    {
        sbox[i] = calloc(size + 1, sizeof(char));
    }

    // Get a random number
    int randnum[sizebasic];
    get_randomnum_list(randnum, sizebasic, 12138 + size + sboxnum);
    // sbox first part
    for (size_t i = 0; i < sizebasic; i++)
    {
        strcpy(sbox[i], basic[randnum[i]]);
    }

    // sbox second part
    get_randomnum_list(randnum, sizebasic, 12138 * 3 + size + sboxnum);
    for (size_t i = 0; i < sizebasic; i++)
    {
        strcpy(sbox[i + sizebasic], basic[randnum[i]]);
    }
    return sbox;
}

/**
 * @brief Retrieves relevant structure of sbox1 from getsbox
 * 
 * @param r {char *} binary number of size
 * @return {char *}  binary number
 */
char *getsbox1(char *r, SDESconfig *config)
{
    if (strlen(r) != config->blocksize / 4 + 1)
    {
        perror("\nERROR (sbox1): invalid input\n");
        exit(EXIT_FAILURE);
    }
    size_t l = strlen(r);
    char **sbox = getsbox(1, l);
    int index = binary_to_decimal(r);

    char *result = calloc(strlen(sbox[index]), sizeof(char));
    strcpy(result, sbox[index]);
    free_sbox(sbox, l);
    return result;
}

/**
 * @brief Retrieves relevant structure of sbox2 from getsbox
 * 
 * @param r {char *} binary number of size
 * @return {char *}  binary number
 */
char *getsbox2(char *r, SDESconfig *config)
{
    if (strlen(r) != config->blocksize / 4 + 1)
    {
        perror("\nERROR (sbox2): invalid input\n");
        exit(EXIT_FAILURE);
    }
    size_t l = strlen(r);
    char **sbox = getsbox(2, l);
    int index = binary_to_decimal(r);

    char *result = calloc(strlen(sbox[index]), sizeof(char));
    strcpy(result, sbox[index]);
    free_sbox(sbox, l);
    return result;
}

/**
 * @brief Generate a number list that contain random
 * number. Since this use rand(), it is pseudo random
 * 
 * @param desc {int[]} target list
 * @param size {int} size of list
 * @param seed {int} seed for the generation
 */
void get_randomnum_list(int desc[], size_t size, int seed)
{
    srand(seed);
    size_t index = 0, x = 0;
    while (index < size)
    {
        int r = rand() % size;
        // Try to find
        for (x = 0; x < index; x++)
        {
            if (desc[x] == r)
            {
                break;
            }
        }

        if (x == index)
        {
            desc[index++] = r;
        }
    }
}

/**
 * @brief Free sbox
 * 
 * @param sbox {char **} sbox need to free
 * @param size {size_t} size of the sbox
 */
void free_sbox(char **sbox, size_t size)
{
    size_t s = pow(2, size - 1) * 2;
    for (size_t i = 0; i < s; i++)
    {
        free(sbox[i]);
    }
    free(sbox);
}

/**
 * @brief Fiestel Cipher on block of binary number
 * 
 * @param bi {char *} block of  binary number
 * @param ki {char *} subkey (binary)
 * @return {char *} 
 */
char *feistel(char *bi, char *ki, SDESconfig *config)
{
    if (!isbinary(ki) || strlen(ki) != config->keysize - 1)
    {
        perror("\n ERROR (feistel): invalid key\n");
        exit(EXIT_FAILURE);
    }
    else if (!isbinary(bi) || strlen(bi) != config->blocksize)
    {
        perror("\n ERROR (feistel): invalid input\n");
        exit(EXIT_FAILURE);
    }
    size_t half = strlen(bi) / 2;
    char L[half + 1], R[half + 1];
    strncpy(L, bi, half);
    strcpy(R, bi + half);
    L[half] = '\0';
    R[half] = '\0';
    char *result = calloc(strlen(bi) + 1, sizeof(char));
    strcpy(result, R);
    char *temp = sdesf(R, ki, config);
    char *temp1 = xor(temp, L);
    strcat(result, temp1);
    free(temp);
    free(temp1);
    return result;
}

/**
 * @brief Generate the initial vector 
 * 
 * @param config {SDESconfig*} config 
 * @return char* generated initial vector
 */
char *get_IV(SDESconfig *config)
{
    return blum(643, 131, config->blocksize);
}

/**
 * @brief Apply xor operation on a and b
 * 
 * @param a {char *} binary number
 * @param b {char *} binary number
 * @return out {char *} output binary number
 */
char * xor (char *a, char *b) {
    if ((isbinary(a) && isbinary(b)) && (strlen(a) == strlen(b)))
    {
        char *out = calloc(strlen(a) + 1, sizeof(char));
        char *acurrent = a;
        char *bcurrent = b;
        char xorchar;
        while (*acurrent != '\0')
        {
            if (*acurrent == *bcurrent)
            {
                xorchar = '0';
            }
            else
            {
                xorchar = '1';
            }
            acurrent++;
            bcurrent++;
            strncat(out, &xorchar, 1);
        }
        return out;
    }
    else
    {
        perror("\n ERROR (xor) invalid input\n");
        exit(EXIT_FAILURE);
    }
}
