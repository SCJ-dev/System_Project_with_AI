#include <stdio.h>
#include <unistd.h> // sleep 함수 사용시 필요

int main(){

  // 숫자를 4개를 담을 변수 (리눅스가 주는 숫자들)
  long a, b, c, d; // 누적시간을 다루는 것이라서 수가 엄청 큼
  
  // 계산을 위해 '과거, 현재' 데이터를 저장할 변수
  long prev_total = 0, prev_idle = 0;
  long current_total, current_idle; // total 전체시간, idle 논 시간
  
  while (1) {
    //1. 파일 열기 
    FILE *fp = fopen("/proc/stat", "r"); // FILE(구조체 : char *ptr, int _cnt, char *_base가 들어있음)
    // fopen = c언어에서 파일을 열어 읽거나 쓰기 위해 준비하는 함수("열고 싶은 파일 선택", "모드작성"(현재 코드는 r이기 때문에 읽기 모드(w=쓰기, a=추가)

    //2. 읽기 (fscanf 사용)
    // 맨 앞 "cpu" 글자는 버리고 (%*s), 숫자 4개만 읽어와서 a,b,c,d에 넣음)
    fscanf(fp, "%*s %ld %ld %ld %ld", &a, &b, &c, &d);

    //3. 파일 닫기(fclose 사용), fclose = c언어에서 fopen함수로 열었던 파일을 닫고 관련 자원을 해제
    fclose(fp);

    //4. 계산 준비
    // a,b,c는 일한 시간, d는 논 시간(idle) total에는 논 시간까지 더해줘야 함
    current_total = a+b+c+d;
    current_idle = d;

    //5. 변화량 구하기 (현재 - 과거)
    long total = current_total - prev_total;
    long idle = current_idle - prev_idle;

    //6. 퍼센트 계산(공식)
    //(전체 변화량 - 논 시간 변화량) / 전체 변화량 * 100
    // total_diff가 0이면 나눗셈 에러 발생하기 때문에 if문으로 보호    
    if(total > 0){
      long t = total - idle;
      float tot = (float)t / total * 100;
      //화면 지우고 출력
      printf("\033[H\033[J"); 
      printf("======================\n");
      printf(" CPU 사용률: %.1f %%\n", tot);
      printf("======================\n");
      if(tot >= 1.0){
          printf("[경고] PC가 뜨겁습니다 🔥\n");
      } else {
          printf("[안정] 온도가 안정적입니다 🍀\n");
      }
      
      
    // 7. 현재 기록을 과거로 저장 (다음 바퀴를 위해)
    prev_total = current_total;
    prev_idle = current_idle;

    // 8. 1초 쉬기
    sleep(1);
  }
  return 0;
}
