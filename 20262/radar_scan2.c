#include <stdio.h>

int main(){
	int s, d, j = 0;
	int sum = 0, sum1 = 0;
	printf("전체 데이터 개수 입력 > ");
	scanf("%d", &s);
	int arr[s];

	printf("연속 탐색 구간 입력 > ");
	
	scanf("%d", &d);
	printf("%d개의 데이터 입력 > ", s);
	
	for(int i = 0; i < s; i++){
		scanf("%d", &arr[i]);
	}
	
	for(int i = 0; i < d; i++){
		sum1 += arr[i];
	}

	sum = sum1;
	
	for(int i = d; i < s; i++){
		sum = sum+arr[i]-arr[i-d];
		if (sum1 < sum){
			sum1 = sum;
		}
	}
	printf("=== [데이터 연속 신호 구간 분석] ===\n%d분간 최대 신호합: %d\n", d, sum1);
	return 0;
}
