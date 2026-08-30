#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

typedef struct {
    int valid;
    uint64_t tag;
    uint64_t lru;
} cache_line_t;

typedef struct {
    cache_line_t *lines;
} cache_set_t;

typedef struct {
    cache_set_t *sets;
} cache_t;

cache_t initialize_cache(int, int);
void free_cache(cache_t*, int);
void proccess_cache(cache_t*, int, int, char*);

int main(int argc, char *argv[])
{
    int opt;
    int s = 0, E = 0, b = 0;
    char *trace_file = NULL;

    int hits_num = 0;
    int misses_num = 0;
    int evictions_num = 0;

    while ((opt = getopt(argc, argv, "s:E:b:t:v")) != -1) {
        switch (opt) {
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
                trace_file = optarg;
                break;
            default:
                printf("Usage ./csim -s <s> -E <E> -b <b> -t <tracefile>\n");
                exit(1);
        }
    }

    int S = pow(2,s);

    cache_t cache = initialize_cache(S, E);

    proccess_cache(&cache, s, b, trace_file);

    free_cache(&cache, S);
    //printSummary(0, 0, 0);
    return 0;
}

cache_t initialize_cache(int num_sets, int num_lines) {
    cache_t cache;

    cache.sets = (cache_set_t *) malloc(sizeof(cache_set_t) * num_sets);
    if (cache.sets == NULL) {
        printf("cache sets memory allocation failed\n");
        exit(1);
    }

    for (int i = 0; i < num_sets; i++) {
        cache.sets[i].lines = (cache_line_t *) calloc(sizeof(cache_line_t), num_lines);
        if (cache.sets[i].lines == NULL) {
            printf("cache lines memory allocation failed\n");
            exit(1);
        }
    }

    return cache;
}

void free_cache(cache_t *cache, int num_sets) {
    for (size_t i = 0; i < num_sets; i++) {
        free(cache->sets[i].lines);
        cache->sets[i].lines = NULL;
    }
    free(cache->sets);
}

void proccess_cache(cache_t *cache, int s, int b, char *tracefile) {
    FILE *fp = fopen(tracefile, "r");

    char operation;
    uint64_t address;
    int size;

    while (fscanf(fp, " %c %lx,%d", &operation, &address, &size) > 0) {

    }

    fclose(fp);
}