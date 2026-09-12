#include <stdio.h>
#include <stdlib.h>

int n, z = 0;

typedef struct {
	int id;
	char code[30];
	int battery;
} Drone;

int main(){
	printf("현재 남은 드론 제고를 입력하세요> ");
	scanf("%d", &n);
	Drone *ptr = (Drone*)malloc(sizeof(Drone) * n);
	Drone *dan = (Drone*)malloc(sizeof(Drone) * n);
	for(int i = 0; i < n; i++){
		printf("%d번째 드론 ID> ", i+1);
		scanf("%d", &ptr[i].id);
		printf("%d번째 드론 정찰 구역> ", i+1);
		scanf("%s", ptr[i].code);
		printf("%d번째 현재 남은 배터리> ", i+1);
		scanf("%d", &ptr[i].battery);
	}

	printf("=====[GOP 정상 임무 수행 드론]=====\n");	
	for(int i = 0; i < n; i++){
		if(ptr[i].battery <= 20){
			dan[z] = ptr[i];
			z++;
		} else {	
			printf("ID: %d | 구역: %s | 배터리: %d%%\n", ptr[i].id, ptr[i].code, ptr[i].battery);
		}
	}
	printf("\n=====[🚨 긴급 복귀 명령 드론 🚨]=====\n");
	for(int i = 0; i < z; i++){	
			printf("ID: %d | 구역: %s | 배터리: %d%%\n", dan[i].id, dan[i].code, dan[i].battery);
	}
	
	free(ptr);
	free(dan);
	return 0;
}
