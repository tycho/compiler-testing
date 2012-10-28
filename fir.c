#include <string.h>

/* finite impulse response (FIR) filter */
void fir_filter(float *dst, const float *src, size_t n, const float *kernel) {
    const float k0 = kernel[0];
    const float k1 = kernel[1];
    const float k2 = kernel[2];
    const float k3 = kernel[3];
    const float k4 = kernel[4];
    const float k5 = kernel[5];
    const float k6 = kernel[6];
    const float k7 = kernel[7];
    do {
        float v = src[7] * k0
                + (src[ 6] + src[ 8]) * k1
                + (src[ 5] + src[ 9]) * k2
                + (src[ 4] + src[10]) * k3
                + (src[ 3] + src[11]) * k4
                + (src[ 2] + src[12]) * k5
                + (src[ 1] + src[13]) * k6
                + (src[ 0] + src[14]) * k7;
        ++src;
        *dst++ = v;
    } while(--n);
}
