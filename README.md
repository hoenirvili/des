# des

This is a small command line utility for generating and encrypting messages using the provided 64bit key.
This is just a simplified implementation of DES(ECB mode) that I've done in order to satisfy my curiosity on how to implement a crypto cipher top to bottom. The padding strategy we use is PKCS5. We don't use like in openssl a password salt combination and also we don't verify if the imput key
or that one we generate is weak or not.

[![asciicast](https://asciinema.org/a/HTJde9ZwiQfVFQaLVb6FTz0oP.svg)](https://asciinema.org/a/HTJde9ZwiQfVFQaLVb6FTz0oP)



I used as a reference implementing DES the following [document](http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm).

```bash
➜  des master ✗ make
➜  des master ✗ ./main --help
des encryption cipher in ECB mode and using PKCS5.0 padding
des -k des.key [-g] input
--help, -h             print out the help message
--decrypt, -d          decrypt the input
--generate-key, -g     generate random 64bit key
--key, -k              input key for enc and dec
```


### Generate 64 bit key and encrypt the message using the key
```bash
➜  des master ✗ ./main --generate-key des.key 
➜  des master ✗ echo -n "This is just a simple plaintext that we want to encrypt" | ./main --key des.key > encrypted.txt
➜  des master ✗ hexdump -C encrypted.txt 
00000000  6e 44 22 a0 ab b0 dd 69  f0 53 20 6c 47 c4 35 d9  |nD"....i.S lG.5.|
00000010  e1 46 9f f8 8d 0c 70 84  30 da 7c 64 d2 fb de 10  |.F....p.0.|d....|
00000020  a3 20 60 31 fd 01 88 d3  cb f6 2d 32 48 50 f0 08  |. `1......-2HP..|
00000030  b9 9b 15 f8 5b 03 6c 59                           |....[.lY|
00000038
```

### Decrypt encrypted plaintext using the same key

```bash
➜  des master ✗ cat encrypted.txt | ./main --key des.key --decrypt > decrypted.txt
➜  des master ✗ hexdump -C decrypted.txt 
00000000  54 68 69 73 20 69 73 20  6a 75 73 74 20 61 20 73  |This is just a s|
00000010  69 6d 70 6c 65 20 70 6c  61 69 6e 74 65 78 74 20  |imple plaintext |
00000020  74 68 61 74 20 77 65 20  77 61 6e 74 20 74 6f 20  |that we want to |
00000030  65 6e 63 72 79 70 74                              |encrypt|
00000037
```
