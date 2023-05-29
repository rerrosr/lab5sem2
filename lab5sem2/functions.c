#include "functions.h"

unsigned int hash(const char* key) {
    unsigned int hashValue = 0;
    while (*key != '\0') {
        hashValue = *key + 31 * hashValue;
        key++;
    }
    return hashValue % CACHE_SIZE;
}

void init_cache(Cache* cache) {
    for (int i = 0; i < CACHE_SIZE; i++) {
        cache->entries[i] = NULL;
    }
    cache->head = NULL;
    cache->tail = NULL;
    cache->count = 0;
}

int is_valid_cname(const char* cname) {
    FILE* file = fopen("dns1.txt", "r");
    if (file == NULL) {
        return 0;
    }

    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, file) != NULL) {
        char name[MAX_LENGTH];
        char type[MAX_LENGTH];
        if (sscanf(line, "%s %*s %s", name, type) == 2 && strcasecmp(name, cname) == 0 && strcasecmp(type, "A") == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}


char* find_in_cache(Cache* cache, const char* domain) {
    unsigned int index = hash(domain);
    CacheEntry* entry = cache->entries[index];
    while (entry != NULL) {
        if (strcmp(entry->domain, domain) == 0) {
            if (entry != cache->head) {
                // Перемещаем запись в начало кеша
                if (entry == cache->tail) {
                    cache->tail = entry->prev;
                    cache->tail->next = NULL;
                } else {
                    entry->prev->next = entry->next;
                    entry->next->prev = entry->prev;
                }

                entry->prev = NULL;
                entry->next = cache->head;
                cache->head->prev = entry;
                cache->head = entry;
            }

            return entry->ip;
        }
        entry = entry->next;
    }

    return NULL;
}
void show_cache(Cache* cache) {
    printf("\nCache:\n");
    CacheEntry* entry = cache->head;
    while (entry != NULL) {
        printf( "%s %s\n", entry->domain, entry->ip);
        entry = entry->next;
    }
}

void add_to_cache(Cache* cache, const char* domain, const char* ip) {
    if (find_in_cache(cache, domain) != NULL) {
        return;
    }
    unsigned int index = hash(domain);
    CacheEntry* new_entry = (CacheEntry*)malloc(sizeof(CacheEntry));
    strcpy(new_entry->domain, domain);
    strcpy(new_entry->ip, ip);
    new_entry->prev = NULL;
    new_entry->next = cache->head;

    if (cache->head != NULL) {
        cache->head->prev = new_entry;
    }
    cache->head = new_entry;
    if (cache->tail == NULL) {
        cache->tail = new_entry;
    }
    if (cache->count == CACHE_SIZE) {
        CacheEntry* entry_to_remove = cache->tail;
        cache->tail = entry_to_remove->prev;
        if (cache->tail != NULL) {
            cache->tail->next = NULL;
        }
        remove_entry_from_cache(cache, entry_to_remove);
    } else {
        cache->count++;
    }
    new_entry->next = cache->entries[index];
    cache->entries[index] = new_entry;
}

char* find_ip_address(FILE* file, Cache* cache, char* domain) {
    char* cached_ip = find_in_cache(cache, domain);
    if (cached_ip != NULL) {
        printf("IP address: %s\n", cached_ip);
        return strdup(cached_ip);
    }
    fseek(file, 0, SEEK_SET);
    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, file) != NULL) {
        char name[MAX_LENGTH];
        char type[MAX_LENGTH];
        char value[MAX_LENGTH];
        sscanf(line, "%s %*s %s %s", name, type, value);
        if (strcmp(name, domain) == 0) {
            if (strcmp(type, "A") == 0) {
                printf("IP address: %s\n", value);
                add_to_cache(cache, domain, value);
                return strdup(value);
            } else if (strcmp(type, "CNAME") == 0) {
                strcpy(domain, value);
                fseek(file, 0, SEEK_SET);
            }
        }
    }

    return NULL;
}

int is_valid_ip(const char* ip) {
    int num = 0;
    int dots = 0;

    while (*ip) {
        if (*ip == '.') {
            dots++;
            if (num < 0 || num > 255 || dots > 3) {
                return false;
            }
            num = 0;
        } else if (*ip >= '0' && *ip <= '9') {
            num = num * 10 + (*ip - '0');
            if (num < 0 || num > 255) {
                return false;
            }
        } else {
            return false;
        }
        ip++;
    }

    if (dots != 3 || num < 0 || num > 255) {
        return false;
    }

    return true;
}
int is_duplicate_record(const char* domain, const char* type, const char* value) {
    FILE* file = fopen("dns1.txt", "r");
    if (file == NULL) {
        return 0;
    }
    char line[MAX_LENGTH];
    while (fgets(line, MAX_LENGTH, file) != NULL) {
        char name[MAX_LENGTH];
        char record_type[MAX_LENGTH];
        char record_value[MAX_LENGTH];
        sscanf(line, "%s %*s %s %s", name, record_type, record_value);
        if (strcmp(name, domain) == 0 && strcmp(record_type, type) == 0 && strcmp(record_value, value) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int is_valid_domain(const char* domain) {
    if (domain[0] == '\0') {
        return 0;
    }
    for (int i = 0; domain[i] != '\0'; i++) {
        if (!isalnum(domain[i]) && domain[i] != '.' && domain[i] != '-') {
            return 0;
        }
    }
    int dot_count = 0;
    for (int i = 0; domain[i] != '\0'; i++) {
        if (domain[i] == '.') {
            dot_count++;
            if (i == 0 || domain[i - 1] == '.' || domain[i + 1] == '\0') {
                return 0;
            }
        }
    }
    if (dot_count < 1) {
        return 0;
    }
    return 1;
}
void add_record() {
    char domain[MAX_LENGTH];
    do {
        printf( "\nEnter domain name: ");
        scanf("%s", domain);
        if (!is_valid_domain(domain)) {
            printf( "Invalid domain name\n");
        }
    } while (!is_valid_domain(domain));

    char type[MAX_LENGTH];
    do {
        printf( "Enter record type (A or CNAME): ");
        scanf("%s", type);
        if (strcmp(type, "A") != 0 && strcmp(type, "CNAME") != 0) {
            printf( "Invalid record type\n");
        }
    } while (strcmp(type, "A") != 0 && strcmp(type, "CNAME") != 0);

    char value[MAX_LENGTH];
    if (strcmp(type, "A") == 0) {
        do {
            printf( "Enter record value: ");
            scanf("%s", value);
            if (!is_valid_ip(value)) {
                printf( "Invalid IP address\n");
            }
        } while (!is_valid_ip(value));
    }
    else {
        do {
            printf( "Enter record value: ");
            scanf("%s", value);
            if (!is_valid_cname(value)) {
                printf( "Invalid CNAME value\n");
            }
        } while (!is_valid_cname(value));
    }

    if (is_duplicate_record(domain, type, value)) {
        printf( "Duplicate record\n");
        return;
    }

    FILE* file = fopen("dns1.txt", "a");
    if (file == NULL) {
        printf( "Could not open file\n\n"
        );
        return;
    }
    fprintf(file, "%s IN %s %s\n", domain, type, value);
    fclose(file);

    printf( "Record added\n"
    );
}
void remove_entry_from_cache(Cache* cache, CacheEntry* entry_to_remove) {
    unsigned int index_to_remove = hash(entry_to_remove->domain);
    if (entry_to_remove == cache->entries[index_to_remove]) {
        cache->entries[index_to_remove] = entry_to_remove->next;
    }
    else {
        CacheEntry* current_entry = cache->entries[index_to_remove];
        while (current_entry != NULL) {
            if (current_entry->next == entry_to_remove) {
                current_entry->next = entry_to_remove->next;
                break;
            }
            current_entry = current_entry->next;
        }
    }
}
void free_cache(Cache* cache) {
    CacheEntry* entry = cache->head;
    while (entry != NULL) {
        CacheEntry* next_entry = entry->next;
        free(entry);
        entry = next_entry;
    }
}

void print_menu() {
    printf("\n-----Menu-----\n");
    printf("1. Find IP by domain\n");
    printf("2. Display cache\n");
    printf("3. Add record\n");
    printf("\nEnter your choice: ");
}
