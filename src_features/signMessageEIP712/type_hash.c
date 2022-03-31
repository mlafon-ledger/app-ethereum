#include <stdlib.h>
#include <stdio.h>
#include "eip712.h"
#include "mem.h"
#include "encode_type.h"
#include "type_hash.h"
#include "hash_wrap.h"

const uint8_t *type_hash(const void *const structs_array,
                         const char *const struct_name,
                         const uint8_t struct_name_length)
{
    const void *const mem_loc_bak = mem_alloc(0); // backup the memory location
    const char  *typestr;
    uint16_t    length;
    uint8_t *hash_ptr;

    typestr = encode_type(structs_array, struct_name, struct_name_length, &length);
    if (typestr == NULL)
    {
        return NULL;
    }
    cx_keccak_init((cx_hash_t*)&global_sha3, 256);
    cx_hash((cx_hash_t*)&global_sha3,
            0,
            (uint8_t*)typestr,
            length,
            NULL,
            0);

#ifdef DEBUG
    // Print type string
    fwrite(typestr, sizeof(char), length, stdout);
    printf("\n");
#endif

    // restore the memory location
    mem_dealloc(mem_alloc(0) - mem_loc_bak);

    // copy hash into memory
    if ((hash_ptr = mem_alloc(KECCAK256_HASH_LENGTH)) == NULL)
    {
        return NULL;
    }
    cx_hash((cx_hash_t*)&global_sha3,
            CX_LAST,
            NULL,
            0,
            hash_ptr,
            KECCAK256_HASH_LENGTH);
#ifdef DEBUG
    // print computed hash
    printf("-> 0x");
    for (int idx = 0; idx < KECCAK256_HASH_LENGTH; ++idx)
    {
        printf("%.02x", hash_ptr[idx]);
    }
    printf("\n");
#endif
    return hash_ptr;
}
