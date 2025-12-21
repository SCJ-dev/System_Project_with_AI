#include <stdio.h>
#include <string.h> // strcmp사용시 필요한 헤더파일
#include <unistd.h> // sleep 사용시 필요한 헤더파일
#include <time.h> // time함수를 사용하기 위해 사용되는 해더파일

void get_current_time(char *buf, size_t size) { // buf : 문자 배열을 가리키는 포인터, size_t : buf 크기
    time_t now = time(NULL); // 현재 시간을 초단위로 저장
    struct tm *t = localtime(&now); // time에서 받은 것을 구조체로 변환(time에서 받은 것을 구조체로 변환)
    strftime(buf, size, "%y-%m-%d %H:%M:%S", t); // 로그에 기록할 것을 문자열로 변환
}

void login(const char *id, const char *result){ // 입력한 id와 로그인 결과에 대한 함수 정의
  FILE *fp = fopen("access.log", "a"); // FILE 자료형을 갖는 포인터 변수 fp를 선언하고 fp변수에서 access.log파일을 만들고 모드는 a(없으면 새 파일 생성, 있으면 끝에 추가 기록한다)로 한다
  if(fp == NULL) return; // fp가 NULL이면 파일 열기 실패
  
  char time_buf[32]; // 충분한 여유를 두고 문자열을 지정
  get_current_time(time_buf, sizeof(time_buf)); // time_buf : 현재 시간을 문자열로 만들어 넣는 함수 호출
  
  fprintf(fp, "TIME : %s IP : LOCALHOST RESULT : %s ID : %s\n", time_buf, result, id); // 향후 접근 결과와 접근한 ID를 기록
  fclose(fp);
}

int main() {
  char ID[100] = "admin", usr_id[100]; // 사용자가 ID, PW를 많이 칠 수도 있기 때문에 크기를 100으로 설정
  char PW[100] = "1234", usr_pw[100];
  int i = 0; // 실패횟수

  while(1){
    printf("아이디를 입력하세요 : ");
    scanf("%99s", usr_id); // 오버플로우 방지
  
    printf("비밀번호를 입력하세요 : ");
    scanf("%99s", usr_pw);
  
    if(strcmp(ID, usr_id) == 0){
        if(strcmp(PW, usr_pw) == 0){ // 두 문자열을 사전식 비교해서 같으면 0, 다르면 0이 아닌 값을 반환
          printf("로그인 되었습니다.\n");
          login(usr_id, "login success");
          break;
        }
        else {
            printf("비밀번호가 틀렸습니다.\n");
            login(usr_id, "FAIL_PW");
            i++;
        }
    } else { // 만약 strcmp 결과가 0이 아니면 로그인 실패라는 글을 log파일에 남김
        printf("아이디가 틀렸습니다.\n");
        login(usr_id, "FAIL_ID");
        i++; // 로그인 실패 시 증가
      } 
      if (i >= 3) {
      printf("5초 후에 다시 시도해주세요.\n");
      sleep(5); // 5초 대기
      i = 0;
    }
  }
  return 0;
}
