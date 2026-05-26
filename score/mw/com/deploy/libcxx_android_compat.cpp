// SPDX-License-Identifier: Apache-2.0
//
// Android libc++ compatibility shim
//
// The prebuilt Clang 21 libc++ headers reference std::__1::__hash_memory
// which may not be present in the Android GVM's libc++.so (built from an
// older version of the libc++ source). This file provides the missing symbol
// so the score_communication shared library links successfully on Android.
//
// The implementation is a standard MurmurHash2 (same as LLVM libc++).

#include <cstddef>
#include <cstdint>

namespace std {
inline namespace __1 {

// Provide __hash_memory if it's not already defined in libc++.so.
// This is a weak symbol so the real libc++.so version takes precedence
// at runtime if it exists.
__attribute__((weak))
size_t __hash_memory(const void* __ptr, size_t __size) noexcept {
    // MurmurHash2 — same algorithm used by LLVM libc++
    const uint64_t seed = 0xc70f6907UL;
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (__size * m);

    const uint64_t* data = static_cast<const uint64_t*>(__ptr);
    const uint64_t* end = data + (__size / 8);

    while (data != end) {
        uint64_t k = *data++;
        k *= m;
        k ^= k >> r;
        k *= m;
        h ^= k;
        h *= m;
    }

    const unsigned char* data2 = reinterpret_cast<const unsigned char*>(data);
    switch (__size & 7) {
    case 7: h ^= static_cast<uint64_t>(data2[6]) << 48; [[fallthrough]];
    case 6: h ^= static_cast<uint64_t>(data2[5]) << 40; [[fallthrough]];
    case 5: h ^= static_cast<uint64_t>(data2[4]) << 32; [[fallthrough]];
    case 4: h ^= static_cast<uint64_t>(data2[3]) << 24; [[fallthrough]];
    case 3: h ^= static_cast<uint64_t>(data2[2]) << 16; [[fallthrough]];
    case 2: h ^= static_cast<uint64_t>(data2[1]) << 8;  [[fallthrough]];
    case 1: h ^= static_cast<uint64_t>(data2[0]);
            h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return static_cast<size_t>(h);
}

}  // namespace __1
}  // namespace std
