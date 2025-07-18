#include "blake3.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define pdie(msg) do { \
    perror(msg); \
    return 1; \
} while (0)

int main(int argc, char **argv) {
  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    if (fd == -1)
        pdie("open");

    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1)
        pdie("fstat");

    void *mapped = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED)
        pdie("mmap");

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);

    blake3_hasher_update_tbb(&hasher, mapped, statbuf.st_size);

    if (munmap(mapped, statbuf.st_size) == -1)
        pdie("munmap");

    if (close(fd) == -1)
        pdie("close");

    uint8_t output[BLAKE3_OUT_LEN];
    blake3_hasher_finalize(&hasher, output, BLAKE3_OUT_LEN);

    for (size_t i = 0; i < BLAKE3_OUT_LEN; i++)
      printf("%02x", output[i]);
    printf("\n");
  }
}
