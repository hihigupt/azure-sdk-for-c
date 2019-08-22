#include <stdio.h>

#define TEST_ASSERT(c) \
  do { \
    if(c) { printf("- `%s`: succeeded\n", #c); } else { fprintf(stderr, "- `%s`: failed\n", #c); result = 1; } \
  } while(false);

int main() {
  return 0;
}
