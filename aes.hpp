/*
This implemenations is basically the same avaiable at https://github.com/kokke/tiny-AES-c.
The changes in the code are to use the transformations instead of subtitutions tables for example during the s-box stage.
This is to implement variations of the AES with Varied Galois Field. 
*/


#ifndef _AES_HPP_
#define _AES_HPP_

#ifndef __cplusplus
#error Do not include the hpp header in a c project!
#endif //__cplusplus

extern "C" {
#include "aes.h"
}

#endif //_AES_HPP_
