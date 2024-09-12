#include <mysql/mysql.h>
#include <iostream>

int main() {
    std::cout << "Hello World!" << std::endl;

    int d = 2;
    scanf("%d", d);
    printf("You gave me: %d", d);

    int *ptr = (int *) malloc(sizeof(int));  
    *ptr = 10;  
    printf("Value of ptr: %d\n", *ptr);  

    return 0;
}
