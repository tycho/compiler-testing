#include <emmintrin.h>

__m128i fold1(__m128i x) {
    __m128i mask = _mm_set1_epi16(0x5555);
    return _mm_add_epi16(_mm_and_si128(mask, _mm_srli_epi16(x, 1)), _mm_and_si128(mask, x));
}

__m128i fold2(__m128i x) {
    __m128i mask = _mm_set1_epi16(0x5555);
    return _mm_add_epi16(_mm_and_si128(_mm_srli_epi16(x, 1), mask), _mm_and_si128(mask, x));
}
