#include <stdio.h>
#include <string.h> // strcmp사용시 필요한 해더파일
#include <unistd.h> // sleep 사용시 필요한 헤더파일

void login(const char *id, const char *result){ // 입력한 id와 로그인 결과에 대한 함수 선언
  FILE *fp = fopen("access.log", "a"); // FILE 자료형을 갖는 포인터 변수 fp를 선언하고 fp변수에서 access.log파일을 만들고 모드는 a(추가 쓰기)로 지정한다
  if(fp == NULL) return; // fp가 NULL이면 파일 열기 실패
  fprintf(fp, "%s ID : %s\n", result, id); // 향후 접근 결과와 접근한 ID를 기록
  fclose(fp);
}

int main() {
  char ID[100] = "admin", usr_id[100]; // 사용자가 ID를 많이 칠 수도 있기 때문에 크기를 100으로 설정
  int PW = 1234, usr_pw;
  int i = 0;

  while(1){
    printf("아이디를 입력하세요 : ");
    scanf("%99s", usr_id);
  
    printf("비밀번호를 입력하세요 : ");
    scanf("%d", &usr_pw);
  
    if(strcmp(ID, usr_id) == 0 && PW == usr_pw){ // 두 문자열을 사전식 비교해서 같으면 0, 다르면 0이 아닌 값을 반환
      printf("로그인 되었습니다.\n");
      login(usr_id, "로그인 성공");
      break;
    } else {
      if(strcmp(ID, usr_id) != 0){ // 만약 아스키코드가 0이 아니면 로그인 실패라는 글을 log파일에 남김
        printf("아이디가 틀렸습니다.\n");
        login(usr_id, "FAIL_ID");
      } else {
        printf("비밀번호가 틀렸습니다.\n");
        login(usr_id, "FAIL_PW");
      }
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
