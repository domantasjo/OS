#include "string.h"

void reverse(char *str, int length) {
  int start = 0;
  int end = length - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}

char *uint32_to_string(uint32_t num) {
  int i = 0;
  char *str;

  if (num == 0) {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  while (num != 0) {
    int rem = num % 10;
    // Jei liekana > 9, naudojame raides (šešioliktainei sistemai)
    str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
    num = num / 10;
  }

  str[i] = '\0'; // Užbaigiame eilutę

  reverse(str, i);

  return str;
}
