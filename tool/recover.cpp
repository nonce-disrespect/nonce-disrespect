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
    struct slice a1, c1, t1, a2, c2, t2;
    struct slice *sp[] = {&a1, &c1, &t1, &a2, &c2, &t2};
    int i;
    GF2EX p, p1, p2;
    vec_pair_GF2EX_long factors;
    char out[33];

    argc -= 1;
    argv += 1;

    assert(argc == 6);

    for (i = 0; i < argc; i += 1) {
        hex2bytes(sp[i], argv[i]);
    }

    initfield();
    buildpoly(p1, &a1, &c1, &t1);
    buildpoly(p2, &a2, &c2, &t2);
    p = p1 + p2;
    MakeMonic(p);
    CanZass(factors, p);

    for (i = 0; i < factors.length(); i += 1) {
        if (deg(factors[i].a) == 1) {
            felem2hex(out, factors[i].a[0]);
            printf("%s\n", out);
        }
    }

    return 0;
}
