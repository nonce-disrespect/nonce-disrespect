#ifndef GCM_GCM_H
#define GCM_GCM_H

#include <stdint.h>
#include <stdlib.h>

#include <NTL/GF2X.h>
#include <NTL/GF2E.h>
#include <NTL/GF2EX.h>

#define NELEMS(arr) ((sizeof arr) / (sizeof arr[0]))

void initfield(void);

void asserthex(const char *s);

struct slice {
    uint8_t *data;
    size_t len;
};

void hex2bytes(struct slice *s, const char *hex);

void block2felem(NTL::GF2E &x, const struct slice *block);

void packuint64(uint8_t *buf, uint64_t x);

const size_t blocklen = 16;

void buildpoly(NTL::GF2EX &p, const struct slice *a,
               const struct slice *c, const struct slice *t);

void felem2hex(char *out, const NTL::GF2E &x);

#endif
