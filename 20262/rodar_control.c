#include <stdio.h>

int main(){
	int target[100];
	int front = 0, rear = 0;
	while(1){
		printf("===== [GOP 레이더 통제 시스템] =====\n1: 표적 수신 | 2: 일반 처리(Queue) | 3: 긴급 격추(Stack) | 0: 종료\n명령 입력> ");
		int n;
		scanf("%d", &n);
		int tar;
		switch (n){
			case 1:
				printf("타겟 id를 입력바랍니다> ");
				scanf("%d", &tar);
				target[rear] = tar;
				rear++;
				break;
			case 2:
				if(front == rear){
					printf("현재 관리 중인 표적이 없습니다..\n");
				} else {
					printf("[일반 처리 완료] 표적 ID: %d\n", target[front]);
					front++;
				} break;
			case 3:
				if(front == rear){
					printf("현재 관리 중인 표적이 없습니다..\n");
				} else {
					rear--;
					printf("[긴급 격추] 표적 ID: %d\n", target[rear]);
				} break;
			case 0:
				printf("프로그램 종료\n");
				return 0;
		}
	}
}
