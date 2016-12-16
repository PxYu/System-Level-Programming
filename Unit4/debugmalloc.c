/**
2014302580341
余璞轩
December 4th, 2016
*/

#include <stdlib.h>
#include <string.h>
#include "debugmalloc.h"
#include "dmhelper.h"
#include <stdio.h>

#define MAXNUM 1024
#define WORDSIZE 4
#define LINESIZE 4
#define BLOCKSIZE 4
#define FENCESIZE 4
#define CHECKSUMSIZE 4
#define FILENAMESIZE 32

struct block {
    int adr;
    int flg;
};

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p,val) (*(unsigned int *)(p)=(val))

const size_t headerSize = CHECKSUMSIZE + FILENAMESIZE + LINESIZE + BLOCKSIZE + FENCESIZE;
const size_t footerSize = FENCESIZE;
struct block blk[MAXNUM];

int checksum(void *p, size_t s) {
    int *pp = (int *)p;
    int sum = 0;
    while (s > 0) {
        int x = *pp;
        int val = 0;
        for (int i = 0; i != 8; i++) {
            val += x & 0x01010101;
            x = x >> 1;
        }
        val += (val >> 16);
        val += (val >> 8);
        sum += val & 0xff;
        s -= 4;
        pp++;
    }
    return sum;
}

/* Wrappers for malloc and free */

void *MyMalloc(size_t size, char *filename, int linenumber) {
	size_t tsize, payload;
	char *bp;
	if (size == 0) {
        return NULL;
    }
	payload = size;
    /* total size to allocate */
	tsize = headerSize + payload + footerSize;
	bp = (char *)malloc(tsize);
	/* clear the whole part by 0 */
    memset(bp, 0, tsize);
	bp += CHECKSUMSIZE;
	strcpy(bp, filename);
	bp += FILENAMESIZE;
	PUT(bp, linenumber);
	bp += LINESIZE;
	PUT(bp, size);
	bp += BLOCKSIZE;
	PUT(bp, 0xCCDEADCC);
	bp += (FENCESIZE + payload);
	PUT(bp, 0xCCDEADCC);
    /* Roll back and fill in the checksum. */
	bp -= (payload + headerSize);
	PUT(bp, checksum(bp + CHECKSUMSIZE, headerSize - CHECKSUMSIZE));
	blk[0].flg++;
	for (int i = 1; i < MAXNUM; i++) {
		if (blk[i].flg == 0) {
			blk[i].flg = 1;
			blk[i].adr = (int)bp;
			break;
		}
	}
	return (void *)(bp + headerSize);
}

void MyFree(void *ptr, char *filename, int linenumber) {
	for (int i = 1; i <= MAXNUM; i++) {
		if (i == MAXNUM) {
			error(4, filename, linenumber);
			break;
		}
		char *p = (char *)(ptr - headerSize);
		if (blk[i].flg == 1 && blk[i].adr == (int)p) {
			int check = GET(p);
			if(check ^ checksum(p + CHECKSUMSIZE, headerSize - CHECKSUMSIZE)) {
				error(3, filename, linenumber);
			}
			char m_filename[FILENAMESIZE];
			strcpy(m_filename, p + CHECKSUMSIZE);
			int m_linenumber = GET(p + CHECKSUMSIZE + FILENAMESIZE);
			int head = GET(ptr - FENCESIZE);
			if(head ^ 0xCCDEADCC) {
				errorfl(1, m_filename, m_linenumber, filename, linenumber);
			}
			unsigned int payload = GET(ptr - FENCESIZE - BLOCKSIZE);
			int tail = GET(ptr + payload);
			if(tail ^ 0xCCDEADCC) {
				errorfl(2, m_filename, m_linenumber, filename, linenumber);
			}
			blk[i].flg = 0;
			blk[0].flg--;
            free(p);
			break;
		}
	}
}

/* returns number of bytes allocated using MyMalloc/MyFree:
	used as a debugging tool to test for memory leaks */
int AllocatedSize() {
	int num = blk[0].flg;
	int sum = 0;
	for (int i = 1; i < MAXNUM && num; i++) {
		if (blk[i].flg == 0) {
            continue;
        }
		char *p = (char *)(blk[i].adr);
        /* sum += block size */
		sum += GET(p + CHECKSUMSIZE + FILENAMESIZE + LINESIZE);
		num--;
	}
	return sum;
}



/* Optional functions */

/* Prints a list of all allocated blocks with the
	filename/line number when they were MALLOC'd */
void PrintAllocatedBlocks() {
	int num = blk[0].flg;
	if(num) {
        printf("Allocated blocks are:\n");
    }
	for (int i = 1; i < MAXNUM && num; i++) {
		if (blk[i].flg == 1) {
			char *p = (char *)(blk[i].adr);
			int payload = GET(p + CHECKSUMSIZE + FILENAMESIZE + LINESIZE);
			char m_filename[FILENAMESIZE];
			strcpy(m_filename, p + CHECKSUMSIZE);
			unsigned int m_linenumber = GET(p + CHECKSUMSIZE + FILENAMESIZE);
			PRINTBLOCK(payload, m_filename, m_linenumber);
			num--;
		}
	}
	return;
}

/* Goes through the currently allocated blocks and checks
	to see if they are all valid.
	Returns -1 if it receives an error, 0 if all blocks are
	okay.
*/
int HeapCheck() {
	int num = blk[0].flg;
	for (int i = 1; i < MAXNUM && num; i++) {
		if (blk[i].flg == 1) {
			char *p = (char *)(blk[i].adr);
			char m_filename[FILENAMESIZE];
			strcpy(m_filename, p + CHECKSUMSIZE);
			int m_linenumber = GET(p + CHECKSUMSIZE + FILENAMESIZE);
			int head = GET(p + headerSize - FENCESIZE);
			if(head ^ 0xCCDEADCC) {
				PRINTERROR(1, m_filename, m_linenumber);
				return -1;
			}
			unsigned int payload = GET(p + headerSize - FENCESIZE - BLOCKSIZE);
			int tail = GET(p + headerSize + payload);
			if(tail ^ 0xCCDEADCC) {
				PRINTERROR(2, m_filename, m_linenumber);
				return -1;
			}
		}
	}
	return 0;
}
