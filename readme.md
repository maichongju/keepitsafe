# KeepItSafe
>:point_right: This tool was created as a learning experience. While the algorithms used are standardized, it is still possible vulnerabilities remain in the program.

This program can encrypt and decrypt the given content and key.

## Compile
### Makefile
Makefile is included in the project. 
```shell
make
```

# Usage
>~~:warning: If no key is provide for the cinphter that require a key. A key can be generated for some cipher, however the generated key is using ```rand()``` which is **pseudo-random**. **It is not cryptographically safe**.~~
```
keepitsafe (ENCRYPT) (CIPHER) -k "key" "input_text"
    ENCRYPT
        -enc                Run as encryption
        -dec                Run as decryption
    CIPHER (Encryption method)
        -ase256             Advanced Encryption 256 bit
        -otp                One Time Pad encryption
        -rsa                RSA encryption
        -sdes config        Simple Data Encryption Standard, config format
                            rounds,keySize,blockSize,encodeType,cipherType,p,q
                            cipherType: CBC,OFB,ECB
    OPTION
        -h, -help           Display this message
```

## Sample

- Encrypt text "ABCDEF" with key "000000" using _One time Pad_
    ```shell
    keepitsafe -enc -otp -k "000000" "ABCDEF"
    ```
    **output**
    ```shell
    qrstuv
    ```

# Cipher

- ~~[ASE256](https://en.wikipedia.org/wiki/Advanced_Encryption_Standard) ```-ase256```~~
- [One Time Pad](https://en.wikipedia.org/wiki/One-time_pad) ```-otp```
    >:loudspeaker: Length of key MUST grather or euqal the length of the plaintext/cinpher

- ~~[RSA](https://en.wikipedia.org/wiki/RSA_(cryptosystem)) ```-rsa```~~

- [Simple Data Encryption Standard](https://en.wikipedia.org/wiki/Data_Encryption_Standard) ```-sdes```
    > ```primes.txt``` is **require** and it contain the first 1,000,000 prime number

    
    

# TODO
- Add more encryption method
- Add GUI support