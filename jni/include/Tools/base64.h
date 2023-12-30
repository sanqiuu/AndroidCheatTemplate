#ifndef BASE64_H
#define BASE64_H

#include <stdlib.h>
#include <string.h>

unsigned char *base64_encode(unsigned char *str);

unsigned char *base64_decode(unsigned char *code);

#endif //BASE64_H
