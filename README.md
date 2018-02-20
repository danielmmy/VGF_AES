#######################################################
|This implemenations is basically the same avaiable at|
|https://github.com/kokke/tiny-AES-c. The changes in  |
|the code are to use the transformations instead of   |
|subtitutions tables for example during the S-box     |
|stage. This is to implement variations of the AES    |
|with Varied Galois Field.                            |              |
|Also, this project uses the gf-complete library      |
|developed by James S. Plank, Ethan L. Miller, Kevin  |
|M. Greenan, Benjamin A. Arnold, John A. Burnum, Adam |
|W. Disney and Allen C. McBride                       |
#######################################################


Note: 
 * No padding is provided so for CBC and ECB all buffers should be mutiples of 16 bytes. For padding [PKCS7](https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7) is recommendable.
 * ECB mode is considered unsafe for most uses and is not implemented in streaming mode. If you need this mode, call the function for every block of 16 bytes you need encrypted. See [wikipedia's article on ECB](https://en.wikipedia.org/wiki/Block_cipher_mode_of_operation#Electronic_Codebook_(ECB)) for more details.

There is no built-in error checking or protection from out-of-bounds memory access errors as a result of malicious input.

A heartfelt thank-you to all the nice people out there who have contributed to this project.


All material in this repository is in the public domain.
