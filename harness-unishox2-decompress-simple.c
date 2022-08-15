#include <stdint.h>
#include <stdio.h>
#include <malloc.h>
#include "unishox2.h"

int LLVMFuzzerTestOneInput(const uint8_t *buf, size_t len)
{
  static char out[16 * 65536];
  unishox2_decompress_simple((const char*)buf, len, out);
  return 0;
}

int main(int argc, const char **argv)
{
  FILE *f;
  long pos;
  size_t len;
  char *buf;

  if (argc < 2)
  {
    printf("usage: %s <filename>\n", argv[0]);
    return 1;
  }

  f = fopen(argv[1], "r");
  if (!f) return 1;
  if (fseek(f, 0, SEEK_END) < 0) return 1;
  pos = ftell(f);
  if (pos < 0) return 1;
  len = pos;
  if (fseek(f, 0, SEEK_SET) < 0) return 1;

  buf = malloc(len);
  if (!buf) return 1;
  if (fread(buf, 1, len, f) != (size_t)len) return 1;

  return LLVMFuzzerTestOneInput((const uint8_t*)buf, len);
}