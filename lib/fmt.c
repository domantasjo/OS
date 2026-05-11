#include "fmt.h"
#include "vga.h"
#include <stdint.h>

void uint16_to_string(uint16_t value, char *str) {
  char temp[6];
  int i = 0;

  if (value == 0) {
    str[0] = '0';
    str[1] = '\0';
    return;
  }

  while (value > 0) {
    value /= 10;
    temp[i++] = (value % 10) + '0';
  }

  // reverse into output string
  int j = 0;
  while (i > 0) {
    str[j++] = temp[--i];
  }

  str[j] = '\0';
}
