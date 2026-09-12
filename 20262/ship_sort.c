#include <stdio.h>

typedef struct {
	int id;
	int import;
	int min;
} Sheep;

int main(){
	printf("===== [수송 관리 프로그램] ===== \n");
	int n;
	printf("배의 숫자를 입력해주세요> ");
	scanf("%d", &n);
	Sheep shp[n];
	for(int i = 0; i < n; i++){
		printf("%d번째 배의 ID를 숫자로 입력해주세요> ", i+1);
		scanf("%d", &shp[i].id);
		printf("%d번째 배의 물자 중요도 입력바랍니다0~5> ", i+1);
		scanf("%d", &shp[i].import);
		printf("%d번째 배의 출항 준비 시간을 분단위로 입력바랍니다> ", i+1);
		scanf("%d", &shp[i].min);
	}
	
	Sheep swap;
	for(int i = 0; i < n; i++){
		for(int j = i+1; j < n; j++){
			if(shp[i].import < shp[j].import){
				swap = shp[i];
				shp[i] = shp[j];
				shp[j] = swap;
			} else if (shp[i].import == shp[j].import){
				if(shp[i].min > shp[j].min){
					swap = shp[i];
					shp[i] = shp[j];
					shp[j] = swap;
				}
			}
		}
	}
			

	printf("\n===== [물자 이동 순위] =====\n");

	for(int i = 0; i < n; i++){
		printf("%d순위 | ID: %d | 중요도: %d | 준비시간: %d\n", i+1, shp[i].id, shp[i].import, shp[i].min);
	}
	return 0;
}
		
	
