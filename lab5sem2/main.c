#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"


int main() {
    Cache cache;
    init_cache(&cache);
    int choice;
    do {
        print_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1: {
                char domain[MAX_LENGTH];
                printf( "\nEnter domain name: " );
                scanf("%s", domain);
                char original_domain[MAX_LENGTH];
                strcpy(original_domain, domain);

                FILE* file = fopen("dns1.txt", "r");
                if (file == NULL) {
                    continue;
                }
                char* ip = find_ip_address(file, &cache, domain);
                if (ip == NULL) {
                    printf( "Domain not found\n" );
                }
                else {
                    add_to_cache(&cache, original_domain, ip);
                    free(ip);
                }
                fclose(file);
                break;
            }
            case 2:
                show_cache(&cache);
                break;
            case 3:
                add_record();
                break;
            default:
                printf("Try again");
                break;
        }
    } while (choice != 3);

    free_cache(&cache);

    return 0;
}