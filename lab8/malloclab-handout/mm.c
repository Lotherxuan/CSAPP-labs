// 显式空闲链表 使用按地址排序的策略，链表中每个块的地址都小于它的后继的地址
#include "mm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12) /* Extend heap by this amount (bytes) */
#define MINBLOCKSIZE 2 * DSIZE

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define PACK(size, alloc) \
  ((size) | (alloc)) /* Pack a size and allocated bit into a word */

#define GET(p) (*(unsigned int*)(p)) /* read a word at address p */
#define PUT(p, val) \
  (*(unsigned int*)(p) = (val)) /* write a word at address p */

#define GET_SIZE(p) (GET(p) & ~0x7) /* read the size field from address p */
#define GET_ALLOC(p) (GET(p) & 0x1) /* read the alloc field from address p */

/* given block ptr bp, compute address of its header */
#define HDRP(bp) ((char*)(bp)-WSIZE)
/* given block ptr bp, compute address of its footer */
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* given block ptr bp, compute address of next blocks */
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)))
/* given block ptr bp, compute address of prev blocks */
#define PREV_BLKP(bp) ((char*)(bp)-GET_SIZE((char*)(bp)-DSIZE))

/*useful macro definition related to explicit free lists*/
/* next pointer and pre pointer both take up 32 bits */
#define GET_PREV(bp) (*(unsigned int*)(bp))
#define SET_PREV(bp, addr) (*(unsigned int*)(bp) = (addr))
#define GET_NEXT(bp) (*((unsigned int*)(bp) + 1))
#define SET_NEXT(bp, addr) (*((unsigned int*)(bp) + 1) = (addr))

static char* heap_listp;
static char* free_listp;

static void* extend_heap(size_t words);
static void* coalesce(void* bp);
static void* find_fit(size_t asize);
static void place(void* bp, size_t asize);
int mm_init(void);
void* mm_malloc(size_t size);
void mm_free(void* bp);
void* mm_realloc(void* ptr, size_t size);

/* static void help_debug() {
  char* bp = free_listp;
  int count = 0;
  printf("free list:");
  for (bp = free_listp; bp; bp = GET_NEXT(bp)) {
    if (count < 10) {
      printf("%p->", bp);
      count++;
    } else {
      break;
    }
  }
  printf("\n");
  return;
} */

static void remove_from_free_list(void* bp) {
  if (bp == NULL || GET_ALLOC(HDRP(bp))) {
    return;
  }
  void* prev = GET_PREV(bp);
  void* next = GET_NEXT(bp);

  SET_PREV(bp, 0);
  SET_NEXT(bp, 0);

  if (prev == NULL && next == NULL) {
    // only one block
    free_listp = NULL;
  } else if (prev == NULL) {
    // bp is the first block pointer
    SET_PREV(next, 0);
    free_listp = next;
  } else if (next == NULL) {
    // bp is the last block pointer
    SET_NEXT(prev, 0);
  } else {
    SET_NEXT(prev, next);
    SET_PREV(next, prev);
  }
}
static void insert_to_free_list(void* bp) {
  if (bp == NULL) {
    return;
  }
  if (free_listp == NULL) {
    // free lists is empty
    free_listp = bp;
    SET_NEXT(bp, 0);
    SET_PREV(bp, 0);
    return;
  }

  char* search_ptr;
  for (search_ptr = free_listp; search_ptr; search_ptr = GET_NEXT(search_ptr)) {
    // printf("bp:%p search_ptr:%p\n", bp, search_ptr);
    if (!GET_NEXT(search_ptr)) {
      if (search_ptr < bp) {
        SET_NEXT(search_ptr, bp);
        SET_PREV(bp, search_ptr);
        SET_NEXT(bp, 0);
        break;
      }
    }
    if (search_ptr > bp) {
      if (!GET_PREV(search_ptr)) {
        SET_NEXT(bp, search_ptr);
        SET_PREV(search_ptr, bp);
        SET_PREV(bp, 0);
        free_listp = bp;
        break;
      } else {
        char* prev_bp = GET_PREV(search_ptr);
        SET_NEXT(bp, search_ptr);
        SET_PREV(search_ptr, bp);
        SET_PREV(bp, prev_bp);
        SET_NEXT(prev_bp, bp);
        break;
      }
    }
  }
}

team_t team = {
    /* Team name */
    "xuan's team",
    /* First member's full name */
    "Luo Yuxuan",
    /* First member's email address */
    "2018302120209@whu.edu.cn",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};

/*
 * extend heap by words * word(4 bytes)
 */
static void* extend_heap(size_t words) {
  char* bp;
  size_t size;

  /* Allocate an even number of words to maintain alignment */
  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
  if ((long)(bp = mem_sbrk(size)) == -1) return NULL;

  /* Initialize free block header/footer and the epilogue header */
  PUT(HDRP(bp), PACK(size, 0));  // free block header
  PUT(FTRP(bp), PACK(size, 0));  // free block footer
  SET_PREV(bp, 0);
  SET_NEXT(bp, 0);
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));  // new epilogue header

  return coalesce(bp);  // coalesce if the previous block was free
}

/*
 * coalesce
 */
static void* coalesce(void* bp) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  void* prev_bp = PREV_BLKP(bp);
  void* next_bp = NEXT_BLKP(bp);

  if (prev_alloc && next_alloc) {
  } else if (prev_alloc && !next_alloc) {
    remove_from_free_list(next_bp);
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
  } else if (!prev_alloc && next_alloc) {
    remove_from_free_list(prev_bp);
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  } else {
    remove_from_free_list(next_bp);
    remove_from_free_list(prev_bp);
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }
  insert_to_free_list(bp);
  return bp;
}

/*
 * find_fit - use first fit strategy to find an empty block.
 */
static void* find_fit(size_t asize) {
  char* bp;
  for (bp = free_listp; bp; bp = GET_NEXT(bp)) {
    if (GET_SIZE(HDRP(bp)) >= asize) {
      return bp;
    }
  }
  return NULL;
}

/*
 * place - Place the request block at the beginning of the free block,
 *         and only split if the remaining part is equal to or larger than the
 * size of the smallest block
 */
static void place(void* bp, size_t asize) {
  size_t csize = GET_SIZE(HDRP(bp));
  remove_from_free_list(bp);
  if ((csize - asize) >= MINBLOCKSIZE) {
    PUT(HDRP(bp), PACK(asize, 1));
    PUT(FTRP(bp), PACK(asize, 1));
    bp = NEXT_BLKP(bp);
    PUT(HDRP(bp), PACK(csize - asize, 0));
    PUT(FTRP(bp), PACK(csize - asize, 0));
    SET_NEXT(bp, 0);
    SET_PREV(bp, 0);
    coalesce(bp);
  } else {
    PUT(HDRP(bp), PACK(csize, 1));
    PUT(FTRP(bp), PACK(csize, 1));
  }
}

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
  if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void*)-1) return -1;
  PUT(heap_listp, 0);                             // alignment padding
  PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));  // prologue header
  PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));  // prologue footer
  PUT(heap_listp + (3 * WSIZE), PACK(0, 1));      // epilogue header
  heap_listp += (2 * WSIZE);
  free_listp = NULL;

  if (extend_heap(CHUNKSIZE / WSIZE) == NULL) return -1;
  return 0;
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void* mm_malloc(size_t size) {
  size_t asize;       // adjusted block size
  size_t extendsize;  // amount to extend heap if no fit
  char* bp;

  if (size == 0) return NULL;

  // adjusted block size to include overhead and alignment reqs
  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    return bp;
  }

  // no fit found, get more memory and place the block
  extendsize = MAX(asize, CHUNKSIZE);
  if ((bp = extend_heap(extendsize / WSIZE)) == NULL) return NULL;
  place(bp, asize);
  return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void* bp) {
  size_t size = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  SET_PREV(bp, 0);
  SET_NEXT(bp, 0);
  coalesce(bp);
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void* mm_realloc(void* ptr, size_t size) {
  void* oldptr = ptr;
  void* newptr;
  size_t copySize;

  newptr = mm_malloc(size);
  if (newptr == NULL) return NULL;
  size = GET_SIZE(HDRP(oldptr));
  copySize = GET_SIZE(HDRP(newptr));
  if (size < copySize) copySize = size;
  memcpy(newptr, oldptr, copySize - WSIZE);
  mm_free(oldptr);
  return newptr;
}