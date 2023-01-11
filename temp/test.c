#include <stdio.h>

int main(){
    int xlen = 3;
    int ylen = 3;
    
    puts("COLUMN MAJOR");
    for (int x = 0; x < xlen; ++x){
        for (int y = 0; y < ylen; ++y){
            printf("(%d %d)", x, y);
        }
        putchar('\n');
    }
    
    puts("ROW MAJOR");
    for (int y = 0; y < ylen; ++y){
        for (int x = 0; x < xlen; ++x){
            printf("(%d %d)", x, y);
        }
        putchar('\n');
    }

    // [00][10][20]
    // [01][11][21]
    // [02][12][22]

    return 0;
}