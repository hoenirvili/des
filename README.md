# des

This is a small command line utility for generating and encrypting messages using the provided 64bit key.
This is just a simplified implementation of DES(ECB mode) that I've done in order to satisfy my curiosity on how to implement a crypto cipher top to bottom. The padding strategy we use is PKCS5.

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
[*] Generate 64 random key
➜  des master ✗ echo "This is just a simple plaintext that we want to encrypt" | ./main --key des.key > encrypted.txt
➜  des master ✗ hexdump -C encrypted.txt 
00000000  c9 f6 1e 97 93 92 b1 c1  07 14 f3 8b 78 56 70 3d  |............xVp=|
00000010  1e 99 9d 34 77 c7 60 fb  ad b2 94 3f 4c f7 eb 0f  |...4w.`....?L...|
00000020  47 45 82 1b 79 9a a1 65  cb ac 95 be b0 0e 13 c8  |GE..y..e........|
00000030  d9 97 77 df 5c a2 af a5  61 d8 d0 fa 3a aa db 12  |..w.\...a...:...|
00000040
```

### Decrypt encrypted plaintext using the same key

```bash
➜  des master ✗ cat encrypted.txt | ./main --key des.key --decrypt > decrypted.txt
➜  des master ✗ hexdump -C decrypted.txt 
00000000  54 68 69 73 20 69 73 20  6a 75 73 74 20 61 20 73  |This is just a s|
00000010  69 6d 70 6c 65 20 70 6c  61 69 6e 74 65 78 74 20  |imple plaintext |
00000020  74 68 61 74 20 77 65 20  77 61 6e 74 20 74 6f 20  |that we want to |
00000030  65 6e 63 72 79 70 74 0a  00 00 00 00 00 00 00 00  |encrypt.........|
00000040
```
