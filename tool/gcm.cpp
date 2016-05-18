#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>

#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/GF2E.h>
#include <NTL/GF2EX.h>
#include <NTL/GF2EXFactoring.h>

#include "gcm.h"

using namespace std;
using namespace NTL;

#define NELEMS(arr) ((sizeof arr) / (sizeof arr[0]))

void initfield(void)
{
    GF2X gf2e128mod;

    // builds the right poly for GMAC's GF(2^128) modulus
    BuildIrred(gf2e128mod, 128);
    GF2E::init(gf2e128mod);
}

void asserthex(const char *s)
{
    size_t i, len;

    len = strlen(s);
    assert(len % 2 == 0);
    for (i = 0; i < len; i += 1) {
        char c;

        c = s[i];
        assert((c >= '0' && c <= '9') ||
               (c >= 'a' && c <= 'f') ||
               (c >= 'A' && c <= 'F'));
    }
}

void hex2bytes(struct slice *s, const char *hex)
{
    size_t i;
    unsigned tmp;

    asserthex(hex);

    s->len = strlen(hex) / 2;
    s->data = (uint8_t *)malloc(s->len);

    for (i = 0; i < s->len; i += 1) {
        sscanf(hex + (2*i), "%02x", &tmp);
        s->data[i] = tmp;
    }
}

void block2felem(GF2E &x, const struct slice *block)
{
    GF2X p;
    size_t i, j;

    for (i = 0; i < block->len; i += 1) {
        uint8_t b = block->data[i];
        for (j = 0; j < 8; j += 1) {
            SetCoeff(p, 8*i + j, b >> (7-j));
        }
    }

    conv(x, p);
}

void packuint64(uint8_t *buf, uint64_t x)
{
    size_t i;

    for (i = 0; i < 8; i += 1) {
        buf[i] = x >> (8*(7-i));
    }
}

void buildpoly(GF2EX &p, const struct slice *a,
               const struct slice *c, const struct slice *t)
{
    GF2E x;
    const struct slice *sp[] = {a, c};
    struct slice s, block;
    size_t i, j;
    uint8_t lenblock[blocklen];

    i = 0;
    for (j = 0; j < NELEMS(sp); j += 1) {
        s = *sp[j];
        while (s.len > 0) {
            block.data = s.data;
            block.len = min(blocklen, s.len);

            block2felem(x, &block);
            SetCoeff(p, i, x);

            i += 1;
            s.data += block.len;
            s.len -= block.len;
        }
    }

    block.data = &lenblock[0];
    block.len = blocklen;
    packuint64(block.data, a->len << 3);
    packuint64(block.data + 8, c->len << 3);

    block2felem(x, &block);
    SetCoeff(p, i, x);

    i += 1;

    assert(t->len == blocklen);

    block2felem(x, t);
    SetCoeff(p, i, x);

    reverse(p, p);
}

void felem2hex(char *out, const GF2E &x)
{
    GF2X p;
    size_t i;
    uint b;
    const char *hex = "0123456789abcdef";

    p = rep(x);

    for (i = 0; i < 32; i += 1) {
        size_t j;

        j = 4 * i;
        b = ((conv<uint>(coeff(p, j+0)) << 3) |
             (conv<uint>(coeff(p, j+1)) << 2) |
             (conv<uint>(coeff(p, j+2)) << 1) |
             (conv<uint>(coeff(p, j+3))));
        out[i] = hex[b];
    }

    out[32] = '\0';
}
