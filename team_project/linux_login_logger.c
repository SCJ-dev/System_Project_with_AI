#include <stdio.h>
#include <string.h> // strcmp사용시 필요한 헤더파일
#include <unistd.h> // sleep 사용시 필요한 헤더파일
#include <time.h> // time, localtime, strftime함수를 사용하기 위해 사용되는 해더파일
//----------------------------DB 관련 헤더----------------------------
#ifdef USE_DB
#include <mariadb/mysql.h>
#endif
//--------------------------시간 관련 로그-----------------------------

void get_current_time(char *buf, size_t size) { // buf : 문자 배열을 가리키는 포인터, size_t : buf 크기
    time_t now = time(NULL); // 현재 시간을 초단위로 저장
    struct tm *t = localtime(&now); // time에서 받은 것을 구조체로 변환(time에서 받은 것을 구조체로 변환)
    if(!t){
        snprintf(buf, size, "00-00-00 00:00:00");
        return;
    }
    
    strftime(buf, size, "%y-%m-%d %H:%M:%S", t); // 로그에 기록할 것을 문자열로 변환
}

//-------------------------DB 관련 로그---------------------------------
#ifdef USE_DB
static MYSQL *g_conn = NULL;

//DB 연결 : security_project로 접속 (localhost)
static int db_init(void){
    g_conn = mysql_init(NULL);
    if(!g_conn) return 0;
    
    if(!mysql_real_connect(g_conn, "127.0.0.1", "secapp", "1234", "security_project", 0, NULL, 0)){
        fprintf(stderr, "[DB] connect error : %s\n", mysql_error(g_conn));
        mysql_close(g_conn);
        g_conn = NULL;
        return 0;
    }
    return 1;
}

static void db_close(void){
    if(g_conn) {
        mysql_close(g_conn);
        g_conn = NULL;
    }
}

static void db_log_access(const char *user_id, const char *ip_addr, const char *result){
    if(!g_conn) return;
    
    char query[512];
    snprintf(query, sizeof(query), "INSERT INTO ACCESS_LOG (USER_ID, IP_ADDR, RESULT) "
    "VALUES ('%s', '%s', '%s')", 
    user_id, ip_addr, result);
    
    if(mysql_query(g_conn, query)){
        fprintf(stderr, "[DB] insert error : %s\n", mysql_error(g_conn));
    }
    
}
#endif

//------------------------파일 관련 로그-------------------------------------
void login(const char *id, const char *result){ // 입력한 id와 로그인 결과에 대한 함수 정의
  FILE *fp = fopen("access.log", "a"); // FILE 자료형을 갖는 포인터 변수 fp를 선언하고 fp변수에서 access.log파일을 만들고 모드는 a(없으면 새 파일 생성, 있으면 끝에 추가 기록한다)로 한다
  if(fp == NULL) return; // fp가 NULL이면 파일 열기 실패
  
  char time_buf[32]; // 충분한 여유를 두고 문자열을 지정
  get_current_time(time_buf, sizeof(time_buf)); // time_buf : 현재 시간을 문자열로 만들어 넣는 함수 호출
  
  fprintf(fp, "TIME : %s IP : LOCALHOST RESULT : %s ID : %s\n", time_buf, result, id); // 향후 접근 결과와 접근한 ID를 기록
  fclose(fp);
  
  #ifdef USE_DB
    db_log_access(id, "127.0.0.1", result);
  #endif
}

int main() {
  char ID[100], usr_id[100]; // 사용자가 ID, PW를 많이 칠 수도 있기 때문에 크기를 100으로 설정
  char PW[100], usr_pw[100];
  int i = 0; // 실패횟수
  
  #ifdef USE_DB
    if(!db_init()){
        fprintf(stderr, "[DB] DB 연결 실패 : 파일 로그만 기록합니다.\n");   
    }
  #endif
  
  printf("회원가입 아이디를 입력하세요 : ");
  scanf("%99s", ID); // 오버플로우 방지
  
  printf("회원가입 비밀번호를 입력하세요 : ");
  scanf("%99s", PW);

  while(1){
    printf("아이디를 입력하세요 : ");
    scanf("%99s", usr_id); // 오버플로우 방지
  
    printf("비밀번호를 입력하세요 : ");
    scanf("%99s", usr_pw);
  
    if(strcmp(ID, usr_id) == 0){
        if(strcmp(PW, usr_pw) == 0){ // 두 문자열을 사전식 비교해서 같으면 0, 다르면 0이 아닌 값을 반환
          printf("로그인 되었습니다.\n");
          login(usr_id, "SUCCESS");
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
  #ifdef USE_DB
    db_close();
  #endif    

  return 0;
}
