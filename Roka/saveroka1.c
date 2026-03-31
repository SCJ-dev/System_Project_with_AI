#include <stdio.h>
#include <time.h>

int main()
{
    int target; 
    int target_man = 0; // 미상 물체 감지 횟수
    FILE *fp; // 파일 포인터

    // 시간 정보 담을 변수
    time_t rawtime;
    struct tm *timeinfo;

    printf("=== ooGP 감시 시스템 가동 ===\n");

    while (1) {
        // target_man이 3이 되기 전까지 반복
        while (target_man <= 2) {
            printf("전방 타겟 (1:감지, 0:무) : ");
            
            // 입력 오류 방지 (숫자가 아닌 문자 입력 시 처리)
            if (scanf("%d", &target) != 1) {
                while (getchar() != '\n'); 
                continue;
            }

            if (target == 1) {
                target_man++;
                if (target_man == 3) {
                    printf("[경고] ooGP 미상물체 출현, 시간과 위치를 기록합니다.\n");
                    
                    // 1. 현재 시스템의 시간 가져오기
                    time(&rawtime);
                    // 2. 지역 시간 구조체로 변환
                    timeinfo = localtime(&rawtime);
                    
                    // 파일명을 EMERGENCYLOG.txt로 수정 (Java 서버 연동용)
                    fp = fopen("EMERGENCYLOG.txt", "a");
                    
                    if (fp != NULL) {
                        // asctime은 자동으로 줄바꿈을 포함하므로 포맷을 맞췄습니다.
                        fprintf(fp, "현재 시간 : %s ooGP 전방 미상물체 출현\n", asctime(timeinfo));
                        fclose(fp);
                        printf("로그 기록 완료.\n");
                    }
                    else {
                        // 세미콜론(;) 누락 수정 완료
                        printf("[오류] 로그 파일을 열 수 없습니다.\n");
                    }
        
                    target_man = 0; // 카운트 초기화
                }
            }
            else if (target == 0) {
                target_man = 0; // 연속 감지 실패 시 카운트 초기화
            }
        }
    }
    return 0;
}
