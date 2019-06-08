#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void* left_sorting(void*);
void* right_sorting(void*);
void* merge(void*);

void bubble_sorting(int *arr, int left, int right);

void Display(int *arr, int start, int end);

struct range{
    int start;
    int end;
};

int arr[10] = {5, 4, 10, 6, 9, 3, 8, 2, 1, 7};

int main(int argc, char** argv){

    printf("Before sorting: ");
    Display(arr, 0, 9);

    pthread_t tid_left, tid_right, tid_merge;

    struct range range_left, range_right, range_merge;

    range_left.start = 0;
    range_left.end = 4;

    range_right.start = 5;
    range_right.end = 9;

    range_merge.start = 0;
    range_merge.end = 9;

    //create threads to sort left part and right part
    pthread_create(&tid_left, NULL, left_sorting, (void*)&range_left);
    pthread_create(&tid_right, NULL, right_sorting, (void*)&range_right);
    //the thread of merging must wait threads of sorting left and right part
    pthread_join(tid_left, NULL);
    pthread_join(tid_right, NULL);
    //create a thread to merge
    pthread_create(&tid_merge, NULL, merge, (void*)&range_merge);
    pthread_join(tid_merge, NULL);

    printf("After sorting: ");
    Display(arr, 0, 9);

    return 0;
}

void Display(int *arr, int start, int end){
    for(int i=start; i<end+1; ++i){
        printf("%d ", arr[i]);
    }
    printf("\n");
}


void *left_sorting(void *arg){
    struct range *r;

    r = (struct range *)arg;

    printf("left part before sorting: ");
    Display(arr, 0, 4);

    bubble_sorting(arr, r->start, r->end);

    printf("left part after sorting: ");
    Display(arr, 0, 4);

    pthread_exit(NULL);
}

void *right_sorting(void *arg){
    struct range *r;
    r = (struct range *)arg;

    printf("right part before sorting: ");
    Display(arr, 5, 9);

    bubble_sorting(arr, r->start, r->end);

    printf("right part after sorting: ");
    Display(arr, 5, 9);

    pthread_exit(NULL);
}

void *merge(void *arg){
    struct range *r;
    r = (struct range *)arg;

    printf("before merging: ");
    Display(arr, 0, 9);

    int *tmp = (int *)malloc(10 * sizeof(int));

    int mid = (r->start + r->end) / 2;

    int i = 0;
    int j = mid + 1;
    int k = 0;

    while(i <= mid && j <= r->end){
        if(arr[i] <= arr[j]){
            tmp[k++] = arr[i++];
        }else{
            tmp[k++] = arr[j++];
        }
    }

    while(i <= mid){
        tmp[k++] = arr[i++];
    }

    while(j <= r->end + 1){
        tmp[k++] = arr[j++];
    }

    for(k=0; k<=9; k++){
        arr[k] = tmp[k];
    }

    printf("after merging: ");
    Display(arr, 0, 9);

    pthread_exit(NULL);

}

void bubble_sorting(int *arr, int left, int right){
    int i = 0;
    int j = 0;

    int flag = 0;

    for(i=0; i<=right-left; i++){
        flag = 0;
        for(j=left; j<=right-i-1; j++){
            if(arr[j] > arr[j+1]){
                int tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
                flag = 1;
            }
        }
        if(flag == 0) break;
    }
}
