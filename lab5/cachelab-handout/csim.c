#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cachelab.h"

static int s;
static int E;
static int b;  //从命令行传递的参数

static int hits = 0;
static int misses = 0;
static int evictions = 0;  //统计命中，未命中以及替换次数
typedef struct node {
  uint64_t mark;
  struct node* next;
} node;

typedef struct list {
  node* head_node;
  int size;
} list;

void init_list(list* list_ptr) {
  list_ptr->head_node = malloc(sizeof(node));
  list_ptr->head_node->next = NULL;
  list_ptr->head_node->mark = -1;
  list_ptr->size = 0;
}

node* find_node(uint64_t mark, list* list_ptr) {
  node* head = list_ptr->head_node;
  node* search_ptr = head;
  node* prev_ptr = head;
  while (search_ptr != NULL) {
    if ((search_ptr->mark) == mark) {
      if (prev_ptr != head) {
        prev_ptr->next = search_ptr->next;
        search_ptr->next = head->next;
        head->next = search_ptr;
      }
      break;
    } else {
      prev_ptr = search_ptr;
      search_ptr = search_ptr->next;
    }
  }
  return search_ptr;
}

//-1表示存在驱逐页面的情况发生
// 0表示没有驱逐页面的情况发生
int insert_node(uint64_t mark, list* list_ptr) {
  node* head = list_ptr->head_node;
  node* new_node = malloc(sizeof(node));
  new_node->mark = mark;
  new_node->next = head->next;
  head->next = new_node;
  (list_ptr->size)++;
  if ((list_ptr->size) == (E + 1)) {
    node* before_delete = head;
    for (int i = 0; i < E; i++) {
      before_delete = before_delete->next;
    }
    before_delete->next = NULL;
    (list_ptr->size)--;
    return -1;
  }
  return 0;
}

int main(int argc, char** argv) {
  int verbose = 0;  //作为布尔变量使用，判断是否打印出详细信息
  char* trace_dir;
  int oc;
  while ((oc = getopt(argc, argv, "s:E:b:t:v")) != -1) {
    switch (oc) {
      case 's':
        s = atoi(optarg);
        break;
      case 'E':
        E = atoi(optarg);
        break;
      case 'b':
        b = atoi(optarg);
        break;
      case 't':
        trace_dir = optarg;
        break;
      case 'v':
        verbose = 1;
        break;
      default:
        printf("This is a default case.This should not be printed");
        break;
    }
  }
  /* printf("s:%d\n", s);
  printf("E:%d\n", E);
  printf("b:%d\n", b);
  printf("trace_dir:%s\n", trace_dir); */
  int lists_amount = (int)pow(2, s);
  list* lists = malloc(sizeof(list) * lists_amount);
  for (int i = 0; i < lists_amount; i++) {
    init_list(&lists[i]);
  }

  char* op;
  char* addr;
  // char* size;
  char* str_split;

  uint64_t sign = 1L << 63;
  uint64_t mark_mask = -1L << (s + b);
  uint64_t index_mask =
      ((-1L >> b << b << (64 - s - b - 1)) ^ sign) >> (64 - s - b - 1);

  FILE* fp = NULL;
  char buff[255];

  fp = fopen(trace_dir, "r");
  while (fgets(buff, sizeof(buff), (FILE*)fp) != NULL) {
    op = strtok(buff, " ,");
    str_split = strtok(NULL, " ,");
    addr = str_split;
    str_split = strtok(NULL, " ,");
    // size = str_split;
    uint64_t address = strtol(addr, NULL, 16);
    if (strcmp(op, "I") == 0) {
      continue;
    }
    uint64_t mark = (mark_mask & address) >> (s + b);
    uint64_t index = (index_mask & address) >> b;
    if (strcmp(op, "L") == 0) {
      node* search_ptr;
      if ((search_ptr = find_node(mark, &lists[index])) == NULL) {
        int res = insert_node(mark, &lists[index]);
        if (res == -1) {
          misses++;
          evictions++;
          if (verbose) {
            printf("%lx L misses evictions\n", address);
          }
        } else {
          misses++;
          if (verbose) {
            printf("%lx L misses\n", address);
          }
        }
      } else {
        hits++;
        printf("%lx L hits\n", address);
      }
    } else if (strcmp(op, "S") == 0) {
      node* search_ptr;
      if ((search_ptr = find_node(mark, &lists[index])) == NULL) {
        int res = insert_node(mark, &lists[index]);
        if (res == -1) {
          misses++;
          evictions++;
          if (verbose) {
            printf("%lx S misses evictions\n", address);
          }
        } else {
          misses++;
          if (verbose) {
            printf("%lx S misses\n", address);
          }
        }
      } else {
        hits++;
        printf("%lx S hits\n", address);
      }
    } else if (strcmp(op, "M") == 0) {
      node* search_ptr;
      if ((search_ptr = find_node(mark, &lists[index])) == NULL) {
        int res = insert_node(mark, &lists[index]);
        if (res == -1) {
          misses++;
          evictions++;
          hits++;
          if (verbose) {
            printf("%lx M misses evictions\n", address);
          }
        } else {
          misses++;
          hits++;
          if (verbose) {
            printf("%lx M misses\n", address);
          }
        }
      } else {
        hits++;
        hits++;
        if (verbose) {
          printf("%lx M hits\n", address);
        }
      }
    }

    /* printf("%d\n", address); */
    /* printf("%s", buff); */
  }

  printSummary(hits, misses, evictions);
  return 0;
}
