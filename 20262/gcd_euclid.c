#include <stdio.h>

int abc(int a, int b){
	if(b == 0){
		return a;
	}
	return abc(b, a % b);
}

int main(){
	printf("최대공약수 구하기\n숫자를 두 개 입력해주세요> ");
	int x, y;
	scanf("%d %d", &x, &y);
	int ab = abc(x, y);
	printf("두 수의 최대공약수는 %d입니다.\n", ab);
	return 0;
}
