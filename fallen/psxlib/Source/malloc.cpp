/*
** Replacement memory alloc functions for
** PSX version of Urban Chaos
**
** Hey if Micro$oft can write one, then so
** can I, anyway it's gotta be easier than
** the VRam allocator I wrote before.
**
** Definately was, we now allocate the rest
** of memory in it's entirety.
*/

#include <MFStdLib.h>

typedef struct {
    void* next;
    std::int32_t size;
} DM_Header;

void* free_list;

void* dud_malloc_init(void* base, void* end) {
    DM_Header* head;

    std::int32_t size = (std::int32_t) end - (std::int32_t) base;

    head = (DM_Header*) base;
    head->next = 0;
    head->size = size;

    free_list = base;

    printf("Allocated %d bytes.\n", size - 8);
}

void* dud_malloc(std::int32_t size) {
    // Align to 4 byte boundary and add 8 bytes for header size

    std::int32_t alloc_size = (size + 11) & 0xfffffffc;
    DM_Header* p = (DM_Header*) free_list;
    while (p && (p->size < alloc_size))
        p = p->next;
    if (p) {
        // Allocate backwards from the end of the block

        p->size -= alloc_size;
        std::uint32_t addr = (std::uint32_t) p + (p->size);
        p = (DM_Header*) addr;
        p->next = 0;
        p->size = alloc_size;
        return (void*) (addr + sizeof(DM_Header));
    } else {
        return 0;
    }
}

void dud_defrag() {
    DM_Header *p, *p2;
    p = free_list;
    while (p) {
        p2 = p->next;

        if ((std::int32_t) p2 == (std::int32_t) p + (p->size)) {
            p->size += p2->size;
            p->next = p2->next;
            p = (DM_Header*) free_list;
        } else if ((std::int32_t) p == (std::int32_t) p2 + (p2->size)) {
            DM_Header* p3;

            p2->size += p->size;

            if (free_list == p) {
                free_list = p2;
            } else {
                p3 = (DM_Header*) free_list;

                while (p3 && (p3->next != p))
                    p3 = p3->next;

                if (p3) {
                    p3->next = p2;
                }
            }
            p = (DM_Header*) free_list;
        } else
            p = p->next;
    }
}

void dud_free(void* p) {
    DM_Header* head = (DM_Header*) ((std::int32_t) p - sizeof(DM_Header));

    head->next = free_list;
    free_list = head;
    dud_defrag();
}