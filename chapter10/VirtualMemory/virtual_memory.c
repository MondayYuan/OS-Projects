#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>

#define TLB_SIZE 16
#define PageTableSize 256
#define PhysicalMemorySize 128
#define FRAME_SIZE 256

int TLBTable[TLB_SIZE][2]; //[pageNumber, frameNumber]
int TLBPointer;
int PageTable[PageTableSize];
unsigned char PhysicalMemory[PhysicalMemorySize][FRAME_SIZE];
int PhysicalMemoryPointer;
int UsedPhysicalMemory = 0;

FILE *f_disk;
int n_addrs, n_pageFaults, n_TLBHits;

void init(void);
void getPage(int addr);
int readFromDisk(int pageNumber);
void updateTLBTable(int pageNumber, int frameNumber);

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Wrong command!\n");
        return -1;
    }

    init();

    FILE *fp = fopen(argv[1], "r");
    int addr;
    while(fscanf(fp, "%d", &addr) != EOF){
        getPage(addr);
        n_addrs ++;
    }

    fclose(fp);
    fclose(f_disk);

    // calculate and print out the stats
    printf("Number of translated addresses = %d\n", n_addrs);
    double pfRate = n_pageFaults / (double)n_addrs;
    double TLBRate = n_TLBHits / (double)n_addrs;
    
    printf("Page Faults = %d\n", n_pageFaults);
    printf("Page Fault Rate = %.3f\n",pfRate);
    printf("TLB Hits = %d\n", n_TLBHits);
    printf("TLB Hit Rate = %.3f\n", TLBRate);

    return 0;
}

void init(void){
    n_addrs = 0;
    n_pageFaults = 0;
    n_TLBHits = 0;

    for(int i=0; i<TLB_SIZE; ++i){
        TLBTable[i][0] = -1;
    }
    TLBPointer = 0;

    for(int i=0; i<PageTableSize; ++i){
        PageTable[i] = -1;
    }
    UsedPhysicalMemory = 0;
    PhysicalMemoryPointer = 0;

    f_disk = fopen("BACKING_STORE.bin", "rb");
    if(f_disk == NULL){
        printf("Fail to find the BACKING_STORE.bin\n");
        exit(-1);
    }
}

void getPage(int logical_addr){
    int pageNumber = (logical_addr & 0xFFFF) >> 8;
    int offset = logical_addr & 0xFF;

    int frameNumber;
    int i;
    for(i=0; i<TLB_SIZE; ++i){
        //TLB Hit
        if(TLBTable[i][0] == pageNumber){
            frameNumber = TLBTable[i][1];
            n_TLBHits ++;
            break;
        }
    }

    //TLB miss
    if(i == TLB_SIZE){
        frameNumber = PageTable[pageNumber];
    }

    //page faults
    if(frameNumber < 0){
        frameNumber = readFromDisk(pageNumber);
        n_pageFaults ++;
    }

    //TLB miss
    if(i == TLB_SIZE){
        updateTLBTable(pageNumber, frameNumber);
    }

    signed char value = PhysicalMemory[frameNumber][offset];
    printf("frame number=%d, offset = %d, value=%d\n", frameNumber, offset,value);
    printf("Virtual address: %d Physical address: %d Value: %d\n", logical_addr, (frameNumber << 8) | offset, value);
}

int readFromDisk(int pageNumber){
    if(fseek(f_disk, pageNumber * FRAME_SIZE, SEEK_SET) != 0){
        printf("Error seeking in backing store\n");
        exit(-1);
    }

    signed char buffer[FRAME_SIZE];
    if(fread(buffer, sizeof(signed char), FRAME_SIZE, f_disk) == 0){
        printf("Error seeking in backing store\n");
        exit(-1);
    }

    PhysicalMemoryPointer = (PhysicalMemoryPointer + 1) % PhysicalMemorySize;
    for(int i=0; i<FRAME_SIZE; ++i){
        PhysicalMemory[PhysicalMemoryPointer][i] = buffer[i];
    }
    
    if(UsedPhysicalMemory < PhysicalMemorySize){
        UsedPhysicalMemory ++;
    }else{ //page replace
        //change TLB Table
        for(int i=0; i<TLB_SIZE; ++i){
            if(TLBTable[i][1] == PhysicalMemoryPointer){
                TLBTable[i][0] = -1;
                break;
            }
        }
        //change page table
        for(int i=0; i<PageTableSize; ++i){
            if(PageTable[i] == PhysicalMemoryPointer){
                PageTable[i] = -1;
                break;
            }
        }
    }

    PageTable[pageNumber] = PhysicalMemoryPointer;

    return PhysicalMemoryPointer;
}

void updateTLBTable(int pageNumber, int frameNumber){
    TLBTable[TLBPointer][0] = pageNumber;
    TLBTable[TLBPointer][1] = frameNumber;
    TLBPointer = (TLBPointer + 1) % TLB_SIZE;
}