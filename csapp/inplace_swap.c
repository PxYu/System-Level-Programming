#include <stdio.h>

void inplace_swap(int *x, int *y) {
    *y = *x ^ *y;
    *x = *x ^ *y;
    *y = *x ^ *y;
}

void reverse_array(int a[], int cnt) {
    int first, last;
    for (first = 0, last = cnt - 1;
        first < last;
        first++, last--) {
            inplace_swap(&a[first], &a[last]);
    }
}

int main() {
    int a = 1, b = 2;
    inplace_swap(&a, &b);
    printf("a:%d, b:%d\n", a, b);
    
    int arr[] = {1,2,3,4,5};
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    reverse_array(arr, 5);
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}