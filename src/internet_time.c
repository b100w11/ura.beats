#include "internet_time.h"
#include "string.h"
#include <stdio.h>


void internet_time(int hours, int minutes, char* words, size_t length) {
  int it = (int) (((hours/24.0) + (minutes/60.0/24.0))*1000) + 0.5;  /*+ seconds/3600.0/24.0;*/

  memset(words, 0, length);
  snprintf(words, length, "%i.", it);
}