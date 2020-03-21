# Des

This is a small command line utility for generating and encrypting messages using the provided 64bit key.
This is just a simplified implementation of DES(ECB mode) that I've done in order to satisfy my curiosity on how to implement a crypto cipher top to bottom.

I used as a reference implementing DES the following [document](http://page.math.tu-berlin.de/~kant/teaching/hess/krypto-ws2006/des.htm).


```bash

➜  /main --help 
des encryption cipher                              
des -k des.key [-g] input                          
--help, -h             print out the help message  
--generate-key, -g     generate random 64bit key   
--key, -k              input key for enc and dec

```
