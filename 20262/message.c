#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
	printf("수신할 전문 개수를 입력하세요> ");
	int n, a = 0;
	scanf("%d", &n);

	char **st = (char**)malloc(sizeof(char*) * n);
	int *len = (int*)malloc(sizeof(int) * n);
	
	for(int i = 0; i < n; i++){
		st[i] = (char*)malloc(sizeof(char)*200);
	}
	
	for(int i = 0; i < n; i++){
		printf("[전문 %d] 입력> ", i+1);
		scanf(" %[^\n]s", st[i]);
		len[i] = strlen(st[i]);
	}

	int max = len[0];
	for(int i = 0; i < n; i++){
		if(max < len[i]){
			max = len[i];
			a = i;
		}
	}
	printf("🚨 가장 긴 긴급 전문> %s", st[a]);
	for(int i = 0; i < n; i++){
		free(st[i]);
	}
	printf("\n");
	free(st);
	free(len);
	return 0;
}


