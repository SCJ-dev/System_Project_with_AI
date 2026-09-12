#include <stdio.h>

int main(){
	printf("배열의 크기 입력> ");
	int s;
	scanf("%d", &s);
	int arr[s];
	for(int i = 0; i < s; i++){
		scanf("%d", &arr[i]);
	}
	
	int max = arr[0]; // 가장 큰 요소
	int count = 0; // 연속된 것을 카운트
	int mc = 0; // 맥스 카운트
	int mi = 0;	
	for(int i = 0; i < s; i++){
		if(max < arr[i]){
			max = arr[i];
			mi = i;
		}
		if(arr[i] >= 50){
			count++;
		} else {
			count = 0;
		}
		
		if(mc < count){
			mc = count;
		}
	}
	printf("=== [레이더 신호 분석 결과] ===\n");
	for(int i = 0; i < s; i++){
		printf("index: [%d] = %d\n", i, arr[i]);
	}
	printf("최고 신호 세기: %ddsb(인덱스 %d번)\n최대 연속 위험 구간 길이: %d\n", max, mi, mc);
	return 0;
}
