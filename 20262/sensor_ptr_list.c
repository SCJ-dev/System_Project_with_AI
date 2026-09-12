#include <stdio.h>
#include <stdlib.h>

void abc(int *arr, int size, int threshold){
	for(int i = 0; i < size; i++){
		if(arr[i] <= threshold){
			arr[i] = 0;
		} else {
			arr[i] *= 2;
		}
	}
	int max = arr[0];
	for(int i = 0; i < size; i++){
		printf("%d ", arr[i]);
	}
	for(int i = 0; i < size; i++){
		if(arr[i] > max){
			max = arr[i];
		}
	}
	printf("최대값> %d\n", max);
	
}

int main(){
	printf("신호 개수 입력> ");
	int n;
	scanf("%d", &n);
	int *ptr = (int*)malloc(sizeof(int) * n);
	printf("%d개의 신호 세기 입력> ", n);
	for(int i = 0; i < n; i++){
		scanf("%d", &ptr[i]);
	}

	abc(ptr, n, 20);
	free(ptr);
	return 0;
}

