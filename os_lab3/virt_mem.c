#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE       256
#define PAGE_NUM        256
#define TLB_SIZE        16
#define FRAME_COUNT     256
#define PHYS_MEM_SIZE   (PAGE_SIZE * FRAME_COUNT)

unsigned char physical_memory[PHYS_MEM_SIZE] = {0};

typedef struct {
    int page;
    int frame;
} tlb_entry;

tlb_entry tlb[TLB_SIZE];
int tlb_next = 0;

typedef struct {
    int frame;
    int valid;
} page_table_entry;

page_table_entry page_table[PAGE_NUM];

int free_frame_counter = 0;


void load_page(int page_num, int frame_num, FILE* backing) {
    fseek(backing, page_num * PAGE_SIZE, SEEK_SET);
    fread(&physical_memory[frame_num * PAGE_SIZE], 1, PAGE_SIZE, backing);
}

void add_to_tlb(int page, int frame) {
    tlb[tlb_next].page = page;
    tlb[tlb_next].frame = frame;
    tlb_next = (tlb_next + 1) % TLB_SIZE;
}


int find_in_tlb(int page) {
    for (int i = 0; i < TLB_SIZE; ++i)
        if (tlb[i].page == page) return tlb[i].frame;
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Запуск: %s addresses.txt\n", argv[0]);
        return 1;
    }

    // всё обнуляем
    for (int i = 0; i < PAGE_NUM; ++i) {
        page_table[i].frame = -1;
        page_table[i].valid = 0;
    }
    for (int i = 0; i < TLB_SIZE; ++i) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }

    FILE *addr_file = fopen(argv[1], "r");
    if (!addr_file) { perror("Не открыть addresses.txt"); return 1; }

    FILE *backing = fopen("BACKING_STORE.bin", "rb");
    if (!backing) { perror("Не открыть BACKING_STORE.bin"); fclose(addr_file); return 1; }

    unsigned int virt_addr;
    int total = 0, page_faults = 0, tlb_hits = 0;

    while (fscanf(addr_file, "%u", &virt_addr) == 1) {
        total++;

        unsigned short addr = virt_addr & 0xFFFF;
        unsigned char page = (addr >> 8) & 0xFF;
        unsigned char offset = addr & 0xFF;

        int frame = find_in_tlb(page);
        if (frame != -1) {
            tlb_hits++;
        } else {
            if (page_table[page].valid) {
                frame = page_table[page].frame;
            } else {
                page_faults++;
                if (free_frame_counter >= FRAME_COUNT) {
                    fprintf(stderr, "Нет свободных фреймов\n");
                    exit(1);
                }
                frame = free_frame_counter++;
                load_page(page, frame, backing);
                page_table[page].frame = frame;
                page_table[page].valid = 1;
            }
            add_to_tlb(page, frame);
        }

        unsigned short phys_addr = frame * PAGE_SIZE + offset;
        signed char value = (signed char)physical_memory[phys_addr];

        printf("Virtual address: %u Physical address: %u Value: %d\n",
               virt_addr, phys_addr, value);
    }

    printf("Page fault rate: %.3f%%\n", (double)page_faults / total * 100);
    printf("TLB hit rate: %.3f%%\n", (double)tlb_hits / total * 100);

    fclose(addr_file);
    fclose(backing);
    return 0;
}