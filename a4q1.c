#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_PAGES 16       // Total number of pages
#define NUM_FRAMES 4       // Total number of frames
#define PAGE_SIZE 256      // Size of each page in bytes
#define MEMORY_SIZE (NUM_PAGES * PAGE_SIZE) // Total logical memory size
#define NUM_ADDRESSES 20   // Number of random memory accesses

typedef struct {
    int frameNumber;  // Frame number
    int presentBit;   // Indicates if the page is in memory
    int useBit;       // Indicates if the page has been recently used
} PageTableEntry;

// Global variables
PageTableEntry pageTable[NUM_PAGES];
int frames[NUM_FRAMES];
int clockHand = 0;

// Function to initialize the page table and frames
void initializePageTable() {
    for (int i = 0; i < NUM_PAGES; i++) {
        pageTable[i].frameNumber = -1;
        pageTable[i].presentBit = 0;
        pageTable[i].useBit = 0;
    }
    for (int i = 0; i < NUM_FRAMES; i++) {
        frames[i] = -1; // -1 indicates an empty frame
    }
}

// Function to handle page faults using the clock algorithm
void handlePageFault(int pageNumber) {
    printf("Page fault occurred for page %d!\n", pageNumber);

    while (1) {
        int currentPage = frames[clockHand];

        if (currentPage == -1 || pageTable[currentPage].useBit == 0) {
            // Replace the page
            if (currentPage != -1) {
                pageTable[currentPage].presentBit = 0; // Mark the current page as not present
                printf("Replacing page %d from frame %d.\n", currentPage, clockHand);
            }

            frames[clockHand] = pageNumber;
            pageTable[pageNumber].frameNumber = clockHand;
            pageTable[pageNumber].presentBit = 1;
            pageTable[pageNumber].useBit = 1;

            clockHand = (clockHand + 1) % NUM_FRAMES;
            return;
        }

        // Reset the use bit and move the clock hand
        pageTable[currentPage].useBit = 0;
        clockHand = (clockHand + 1) % NUM_FRAMES;
    }
}

// Function to translate a logical address to a physical address
int translateAddress(int logicalAddress) {
    int pageNumber = logicalAddress / PAGE_SIZE;
    int offset = logicalAddress % PAGE_SIZE;

    if (pageNumber >= NUM_PAGES) {
        printf("Error: Page number out of bounds for logical address %d\n", logicalAddress);
        return -1;
    }

    if (!pageTable[pageNumber].presentBit) {
        handlePageFault(pageNumber);
    }

    int physicalAddress = pageTable[pageNumber].frameNumber * PAGE_SIZE + offset;
    pageTable[pageNumber].useBit = 1; // Mark the page as recently used
    return physicalAddress;
}

// Function to generate random memory access patterns
void generateRandomAccessPatterns(int *addresses, int numAddresses) {
    srand(time(NULL));
    for (int i = 0; i < numAddresses; i++) {
        addresses[i] = rand() % MEMORY_SIZE;
    }
}

// Main function
int main() {
    int addresses[NUM_ADDRESSES];

    // Initialize the page table and frames
    initializePageTable();

    // Generate random memory access patterns
    generateRandomAccessPatterns(addresses, NUM_ADDRESSES);

    // Translate each logical address and handle page faults
    printf("Randomly generated virtual addresses and their translations:\n");
    for (int i = 0; i < NUM_ADDRESSES; i++) {
        int virtualAddress = addresses[i];
        printf("Virtual Address: %5d -> ", virtualAddress);

        int physicalAddress = translateAddress(virtualAddress);
        if (physicalAddress != -1) {
            printf("Physical Address: %5d\n", physicalAddress);
        }
    }

    return 0;
}
