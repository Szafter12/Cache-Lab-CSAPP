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

typedef struct {
    int hit_num;
    int miss_num;
    int eviction_num;
} cache_stat_info_t;

cache_t initialize_cache(int, int);
void free_cache(cache_t*, int);
void proccess_cache(cache_t*, cache_stat_info_t*, int, int, int, char*);

cache_line_t *get_lru_line(cache_set_t *, int);

int main(int argc, char *argv[])
{
    int opt;
    int s = 0, E = 0, b = 0;
    char *trace_file = NULL;

    cache_stat_info_t cache_stat = {0,0,0};

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

    if (E < 1 || (s+b) > 64 || trace_file == NULL || s < 0 || b < 0) {
        printf("Usage ./csim -s <s> -E <E> -b <b> -t <tracefile>\n");
        printf("Sum of <s> and <b> must be less than 64\n");
        printf("<E> must be at least set to 1\n");
        printf("<s>, <b> and <E> must be positive numbers\n");
        exit(1);
    }

    int S = pow(2,s);

    cache_t cache = initialize_cache(S, E);

    proccess_cache(&cache, &cache_stat, s, b, E,trace_file);

    free_cache(&cache, S);

    printSummary(cache_stat.hit_num, cache_stat.miss_num, cache_stat.eviction_num);
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
            free(cache.sets);
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

void proccess_cache(cache_t *cache, cache_stat_info_t *cache_info, int s, int b, int E,char *tracefile) {
    FILE *fp = fopen(tracefile, "r");

    if (fp == NULL) {
        printf("cache tracefile open failed\n");
        exit(1);
    }

    char operation;
    uint64_t address;
    int size;

    uint64_t timer = 0;

    while (fscanf(fp, " %c %lx,%d", &operation, &address, &size) > 0) {
        if (operation == 'I') continue;
        ++timer;

        uint64_t set_index = (address >> b) & ((1 << s) - 1);
        uint64_t tag_index = address >> (s+b);

        cache_set_t *set = &cache->sets[set_index];
        int is_hit = 0;

        for (size_t i = 0; i < E; i++) {
            cache_line_t *line = &set->lines[i];

            if (line->valid && line->tag == tag_index) {
                is_hit = 1;

                if (operation == 'M') {
                    cache_info->hit_num += 2;
                } else {
                    cache_info->hit_num++;
                }

                line->lru = timer;

                break;
            }
        }

        if (!is_hit) {
            cache_info->miss_num++;

            if (operation == 'M') cache_info->hit_num++;

            int is_cache_stored = 0;
            for (size_t i = 0; i < E; i++) {
                cache_line_t *line = &set->lines[i];

                if (!line->valid) {
                    line->valid = 1;
                    line->tag = tag_index;
                    line->lru = timer;

                    is_cache_stored = 1;

                    break;
                }
            }

            if (!is_cache_stored) {
                cache_info->eviction_num++;

                cache_line_t *lru_line = get_lru_line(set, E);

                lru_line->valid = 1;
                lru_line->tag = tag_index;
                lru_line->lru = timer;
            }
        }
    }

    fclose(fp);
}

cache_line_t *get_lru_line(cache_set_t *set, int line_nums) {
    size_t min_idx = 0;
    cache_line_t *min_line = &set->lines[min_idx];

    for (size_t i = 1; i < line_nums; i++) {
        cache_line_t *line = &set->lines[i];

        if (line->lru < min_line->lru) {
            min_line = line;
            min_idx = i;
        }
    }

    return min_line;
}