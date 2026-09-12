#include <stdio.h>

int main(){
    printf("버퍼 크기: ");
    int count = 0, s, p, a, b, size = 0, result = 0;
    scanf("%d", &s);
    printf("패킷 수 입력: ");
    scanf("%d", &p);
    
    int arr[s];
    for(int i = 0; i < p; i++){
        scanf("%d %d", &a, &b);
        result = a % 7;
        if(result == b){
            count++;
            arr[size] = a;
            size++;
            if(size == s){
                size = 0;
            }
        }
    }
    
    printf("===== [시스템 순환 버퍼 최종 로그] =====\n");
    for(int i = 0; i < s; i++){
        printf("[Log %d] %d\n", i, arr[i]);
    }
    printf("총 수신된 정상 패킷 수: %d개\n", count);
    
    return 0;
}
