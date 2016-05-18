#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <NTL/GF2X.h>
#include <NTL/GF2XFactoring.h>
#include <NTL/GF2E.h>
#include <NTL/GF2EX.h>
#include <NTL/GF2EXFactoring.h>

#include "gcm.h"

using namespace std;
using namespace NTL;

int main(int argc, char **argv)
{
    struct slice a1, c1, t1, a2, c2, h;
    struct slice *sp[] = {&a1, &c1, &t1, &a2, &c2, &h};
    int i;
    GF2E x, y, z;
    GF2EX p1, p2;
    char out[2*blocklen + 1];

    argc -= 1;
    argv += 1;

    assert(argc == 6);

    for (i = 0; i < argc; i += 1) {
        hex2bytes(sp[i], argv[i]);
    }

    initfield();

    block2felem(x, &h);
    block2felem(y, &t1);
    buildpoly(p1, &a1, &c1, &t1);
    eval(z, p1, x);

    buildpoly(p2, &a2, &c2, &t1);
    add(p2, p2, y);
    add(p2, p2, z);
    eval(z, p2, x);

    felem2hex(out, z);
    printf("%s\n", out);

    return 0;
}
