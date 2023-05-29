#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#ifndef LAB5SEM2_FUNCTIONS_H
#define LAB5SEM2_FUNCTIONS_H
#define MAX_LENGTH 256
#define CACHE_SIZE 3

typedef struct CacheEntry {
    char domain[MAX_LENGTH];
    char ip[MAX_LENGTH];
    struct CacheEntry* prev;
    struct CacheEntry* next;
} CacheEntry;
typedef struct Cache {
    CacheEntry* entries[CACHE_SIZE];
    CacheEntry* head;
    CacheEntry* tail;
    int count;
} Cache;

unsigned int hash(const char* str);
void init_cache(Cache* cache);
int is_valid_cname(const char* cname);
char* find_in_cache(Cache* cache, const char* domain);
void remove_entry_from_cache(Cache* cache, CacheEntry* entry_to_remove);
void add_to_cache(Cache* cache, const char* domain, const char* ip);
char* find_ip_address(FILE* file, Cache* cache, char* domain);
void show_cache(Cache* cache);
int is_valid_ip(const char* ip);
int is_duplicate_record(const char* domain, const char* type, const char* value);
int is_valid_domain(const char* domain);
void add_record();
void free_cache(Cache* cache);
void print_menu();
#endif //LAB5SEM2_FUNCTIONS_H
