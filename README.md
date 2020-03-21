# Des

This is a small command line utility for generating and encrypting messages using the provided 64bit key.
This is just a dummy implementation that I've done in order to satisfy my curiosity on how to implement top to bottom
encryption and decryption of a cypher block and because des is a popular cypher block, I've written my own implementation.

As rought reference for implementing DES I used the following document: http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm


```bash

➜  /main --help 
des encryption cipher                              
des -k des.key [-g] input                          
--help, -h             print out the help message  
--generate-key, -g     generate random 64bit key   
--key, -k              input key for enc and dec

```
