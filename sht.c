#include "blake3.h"
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gmp.h>

#define BUF_SIZE (16 * 1024 * 1024) // 16MB for stdin
#define pdie(msg) do { perror(msg); return 1; } while (0)

static const char base64_lookup[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

int hash_fd(int fd) {
    struct stat statbuf;
    if (fstat(fd, &statbuf) == -1)
        pdie("fstat");

    blake3_hasher hasher;
    blake3_hasher_init(&hasher);

    if (S_ISREG(statbuf.st_mode)) {
        void *mapped = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped == MAP_FAILED)
            pdie("mmap");
        blake3_hasher_update_tbb(&hasher, mapped, statbuf.st_size);
        if (munmap(mapped, statbuf.st_size) == -1)
            pdie("munmap");
    } else {
        uint8_t *buf = (uint8_t*)malloc(BUF_SIZE);
        if (!buf) pdie("malloc");
        ssize_t n;
        while ((n = read(fd, buf, BUF_SIZE)) > 0)
            blake3_hasher_update_tbb(&hasher, buf, n);
        if (n < 0) pdie("read");
        free(buf);
    }

    uint8_t output[33];
    blake3_hasher_finalize(&hasher, output, 32);
    output[32] = 0;
    for (size_t i = 0; i < 33; i+=3)
    {
	uint32_t triplet = (output[i] << 16) | (output[i+1] << 8) | output[i+2];
        putchar(base64_lookup[(triplet >> 18) & 0x3F]);
        putchar(base64_lookup[(triplet >> 12) & 0x3F]);
        putchar(base64_lookup[(triplet >> 6 ) & 0x3F]);
        putchar(base64_lookup[ triplet        & 0x3F]);
    }
    printf("\n");

    return 0;
}

int main(int argc, char **argv) {
    if (argc == 1) {
        return hash_fd(STDIN_FILENO);
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            return 1;
        }
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1)
            pdie("open");
        int res = hash_fd(fd);
        close(fd);
        if (res != 0)
            return res;
    }
    return 0;
}
