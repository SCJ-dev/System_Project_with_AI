#include <stdio.h>
#include <stdlib.h>

typedef struct{
	int id;
	char name[30];
	int status;
	float battery;
} SensorNode;

void updateStatus(SensorNode* node, int newStatus, float batteryConsume){
	node -> status = newStatus;
	node -> battery = node->battery - batteryConsume;
	printf("ID: %d | 위험등급: %d | 잔여배터리: %.2f\n", node->id, node->status, node->battery);
}

void printAlertList(const SensorNode* list, int count){
	for(int i = 0; i < count; i++){	
		if(list[i].status == 1){
			if(list[i].battery > 0){
			printf("[경계] ID: %d(%s) | 위험등급: %d | 잔여배터리: %.2f\n", list[i].id, list[i].name, list[i].status, list[i].battery);	
			} else {
				printf("[파괴] ID: %d(%s) | 위험등급: %d | 잔여배터리: %.2f\n", list[i].id, list[i].name, list[i].status, list[i].battery);	
			}
		} else if(list[i].status == 2){
			if(list[i].battery > 0){			
				printf("[위험] ID: %d(%s) | 위험등급: %d | 잔여배터리: %.2f\n", list[i].id, list[i].name, list[i].status, list[i].battery);
			} else {
				printf("[파괴] ID: %d(%s) | 위험등급: %d | 잔여배터리: %.2f\n", list[i].id, list[i].name, list[i].status, list[i].battery);	
			}
		}
	}
}

void saveLogFile(const SensorNode* list, int count, FILE *filename){
	for(int i = 0; i < count; i++){	
		if(list[i].status == 1){
			if(list[i].battery > 0){
			fprintf(filename, "[경계] ID: %d(%s) | 위험등급: %d | 잔여배터리: %f\n", list[i].id, list[i].name, list[i].status, list[i].battery);
			} else {
				fprintf(filename, "[파괴] ID: %d(%s) | 위험등급: %d | 잔여배터리: %.2f\n", list[i].id, list[i].name, list[i].status, list[i].battery);	
			}
		}	
		else if(list[i].status == 2){	
			if(list[i].battery > 0){
				fprintf(filename, "[위험] ID: %d(%s) | 위험등급: %d | 잔여배터리: %f\n", list[i].id, list[i].name, list[i].status, list[i].battery);
			} else {
				fprintf(filename, "[파괴] ID: %d(%s) | 위험등급: %d | 잔여배터리: %.2f\n", list[i].id, list[i].name, list[i].status, list[i].battery);	
			}
		}
		else {
			fprintf(filename, "[안전] ID: %d(%s) | 위험등급: %d | 잔여배터리: %f\n", list[i].id, list[i].name, list[i].status, list[i].battery);
		}
	}
	printf("[system]파일이 성공적으로 저장되었습니다.\n프로그램을 종료하고 파일을 확인해보십시요.\n");
}	

int main(){
	printf("==================================================\n     [DEFENSE SYSTEM - MONITORING LOG]     \n==================================================\n");
	printf("[system] 총 3개 센서 노드를 초기화합니다...\n");
	printf("[system] 노드 상태 업데이트 중...\n");
	SensorNode list[3] = {
    	{101, "알파초소", 0, 100.0f},
    	{102, "브라보초소", 0, 100.0f},
    	{103, "찰리초소", 0, 100.0f}
	};
	printf("[system] 업데이트 완료\n");
	while(1){
		printf("==================================================\n     [DEFENSE SYSTEM - MONITORING LOG]     \n==================================================\n");
		printf("1. 배터리 수정\n2. 경계 위험 초소\n3. 파일 저장\n4. 현재 초소 상태\n0. 종료\n");
		int cho;
		scanf("%d", &cho);

		switch (cho){
			case 1:
				printf("배터리를 수정할 초소 ID를 입력하세요> ");
				int n;
				scanf("%d", &n);
					int c = 0;
				for(int i = 0; i < 3; i++){
					if(n == list[i].id){
						c++;
						printf("현재 ID: %d(%s)의 경계태새를 어떻게 바꾸시겠습니까?> ", n, list[i].name);
						int a;
						float b;
						scanf("%d", &a);
						printf("현재 배터리가 얼마나 감소되었습니까?> ");
						scanf("%f", &b);
						updateStatus(&list[i], a, b);
					}
					
				}

					if (c == 0){
						printf("[system] 해당 ID는 존재하지 않습니다..\n");
					}
				break;
			case 2:
				printAlertList(list, 3);
				break;
			case 3:
				FILE *f;
				f = fopen("sensor_log.txt", "w");
				if(f == NULL){
					printf("[system] 파일이 열리지 않았습니다.\n");
				}
				saveLogFile(list, 3, f);
				fclose(f);
				break;
			case 4:
				printf("========== [현재 초소 관리] ==========\n");
				for(int i = 0; i < 3; i++){
					printf("ID: %d | 위험등급: %d | 잔여배터리: %.2f\n", list[i].id, list[i].status, list[i].battery);
				}
				break;
			case 0:
				printf("프로그램을 종료합니다.\n");
				return 0;
		}
	}

}
