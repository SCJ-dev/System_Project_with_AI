#include <stdio.h>
#include <string.h> // strcmp, strlen 등 문자열 처리 관련 헤더파일
#include <unistd.h> // sleep 함수 관련 헤더파일
#include <time.h> // 시간 관련 헤더 파일
#include <stdlib.h> // getenv, atoi

#ifdef USE_DB // ifdef USE_DB : 이 프로그램에 DB 기능을 포함시킬지 말지 결정하는 체크박스
#include <mariadb/mysql.h> // DB 관련 헤더파일
#endif

//--------------------------시간 관련 로그-----------------------------
void get_current_time(char *buf, size_t size) {
    time_t now = time(NULL); // time_t : 1970년 1/1일을 기준으로 현재시간을 저장(대부분 초로 저장) / time : 현재 시간을 초단위로 저장
    struct tm *t = localtime(&now); // localtime : time 함수에 저장 되어 있는 것을 구조체 형태로 변환
    if(!t){
        snprintf(buf, size, "00-00-00 00:00:00"); // localtime 실패 시 buf에 해당 문자열을 저장(로그가 깨지지 않게 하기 위한 방어 코딩)
        return;
    }
    strftime(buf, size, "%y-%m-%d %H:%M:%S", t); // strftime : t 구조체를 문자 형태로 변환
}

//-------------------------DB 관련 로그---------------------------------
// 🎯 DB 관련 로그 확인
#ifdef USE_DB
static MYSQL *g_conn = NULL; // *g_conn은 MYSQL이라는 구조체를 가리키는 포인터이고 프로그램 실행 동안 유지되는 DB 연결 핸들을 파일 내부에서만 사용하도록 static으로 선언

static int db_init(void){
    g_conn = mysql_init(NULL); // mysql_init : 구조체를 초기화하는 함수
    if(!g_conn) return 0; // g_conn이 비어있다면 초기화 실패
    
    const char *db_host = getenv("DB_HOST"); // DB 서버 주소
    const char *db_user = getenv("DB_USER"); // DB 사용자 계정
    const char *db_pass = getenv("DB_PASS"); // DB 비번
    const char *db_name = getenv("DB_NAME"); // DB 이름
    
    if(!db_host) db_host = "127.0.0.1";
    
    
    if(!db_user || !db_pass || !db_name){ // 하나라도 맞지 않으면
        fprintf(stderr, "[DB] missing env : DB_USER/DB_PASS/DB_NAME\n"); // 터미널에 에러 사유를 출력
        mysql_close(g_conn); // DB 연결 종료
        g_conn = NULL; // g_conn을 비워줌
        return 0;
    }

    if(!mysql_real_connect(g_conn, db_host, db_user, db_pass, db_name, 0, NULL, 0)){ // mysql_real_connect 함수 선언 db 서버 연결 시도
        fprintf(stderr, "[DB] connect error : %s\n", mysql_error(g_conn)); // 실패할 시 에러메시지 출력
        mysql_close(g_conn); // mysql_close : db 연결종료
        g_conn = NULL; //g_conn을 null 값으로 초기화
        return 0; // 연결 실패
    }
    return 1; // 연결 성공
}

static void db_close(void){
    if(g_conn) {
        mysql_close(g_conn);
        g_conn = NULL;
    }
}

static void db_log_access(const char *user_id, const char *ip_addr, const char *result){
    if(!g_conn) return; // g_conn이 비어있으면 실행안됨

    char esc_id[128]; // 이스케이프된 아이디를 저정하는 버퍼
    unsigned long id_len = strlen(user_id); // 유저아이디의 길이를 숫자로 표현
    if(id_len > 60) id_len = 60; // 입력한 유저아이디가 널 문자 포함 60글자 이상이 되면 
    mysql_real_escape_string(g_conn, esc_id, user_id, id_len); // sql 인젝션 공격 위험 완화

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO ACCESS_LOG (USER_ID, IP_ADDR, RESULT) VALUES ('%s','%s','%s')",
             esc_id, ip_addr, result); // 구조체를 문자열로 저장

    if(mysql_query(g_conn, query)){
        fprintf(stderr, "[DB] insert error : %s\n", mysql_error(g_conn)); // 에러 발생시 에러 내용 출력
    }
}

/*
 * return:
 *  1 : ID 존재 (out_pw/out_name 채움)
 *  0 : ID 없음
 * -1 : DB 에러
 */
static int db_fetch_user(const char *user_id,
                         char *out_pw, size_t out_pw_sz,
                         char *out_name, size_t out_name_sz) {
    if(!g_conn) return -1;

    char esc_id[128];
    unsigned long id_len = strlen(user_id); // unsgined = 부호 없는 정수 타입 = admin의 길이는 5
    if(id_len > 60) id_len = 60; // 아이디가 너무 길면 60글자만 인정
    mysql_real_escape_string(g_conn, esc_id, user_id, id_len);

    char query[256]; // DB에 보낼 쿼리를 담을 256칸 짜리 표를 준비
    snprintf(query, sizeof(query),
             "SELECT PASSWORD, NAME FROM USER_TB WHERE ID='%s' LIMIT 1",
             esc_id); // 아이디가 admin인 pw와 name을 딱 하나만 찾음

    if(mysql_query(g_conn, query)){ // mysql_query : C가 DB 서버에게 명령어를 전달하는 역할(g_conn 연결통로, query 명령어 종이)
        fprintf(stderr, "[DB] select error : %s\n", mysql_error(g_conn)); // 만약 실패시 에러메시지 0아니면 -1을 출력하고 종료
        return -1;
    }

    MYSQL_RES *res = mysql_store_result(g_conn); // DB가 준 결과물을 'res'라는 메모리에 담음
    
    if(!res){ // 만약 res에 값이 없다면 에러 출력
        fprintf(stderr, "[DB] store_result error : %s\n", mysql_error(g_conn)); // mysql_error = 어떤 이유 때문에 실행이 안되는지에 대한 것을 출력
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(res); // res안에 실제 데이터가 들어있는 첫 번째 줄을 꺼내서 row에 저장
    if(!row){ // row가 비어있으면
        mysql_free_result(res); // 쿼리 결과 집합이 점유하던 메모리 헤제
        return 0;
    }
    
    // res의 값이 들어있으면
    snprintf(out_pw, out_pw_sz, "%s", row[0] ? row[0] : ""); // pw값이 있으면 그대로 없으면 ""을 out_pw에 복사
    snprintf(out_name, out_name_sz, "%s", row[1] ? row[1] : ""); // name값이 있으면 그대로 없으면 ""을 out_name에 복사
    mysql_free_result(res); // 쿼리 결과 집합이 점유하던 메모리 헤제
    return 1; // ID 존재
}
#endif

//------------------------파일 관련 로그-------------------------------------
void login(const char *id, const char *result){
    FILE *fp = fopen("access.log", "a"); // access.log라는 파일이 없으면 새파일을 만들고 파일이 있으면 이어쓰기를 함 (a = 가장 뒤에 글을 추가), 만약 파일이 없을 시 생성
    if(fp == NULL) return; // 파일 열기 실패 시 종료

    char time_buf[32];
    get_current_time(time_buf, sizeof(time_buf));

    fprintf(fp, "TIME : %s IP : LOCALHOST RESULT : %s ID : %s\n", time_buf, result, id); // ip를 로컬호스트로 고정 기록
    fclose(fp);

#ifdef USE_DB
    // DB에는 SUCCESS/FAIL로 통일
    const char *db_result = result;
    if(strcmp(result, "FAIL_ID") == 0 || strcmp(result, "FAIL_PW") == 0 || strcmp(result, "FAIL_DB") == 0) {
        db_result = "FAIL"; // FAIL_ID, FAIL_PW, FAIL_DB는 DB에 FAIL로 통일해서 저장
    }
    db_log_access(id, "127.0.0.1", db_result);
#endif
}

//----------------------------main-----------------------------
int main(void) {
#ifndef USE_DB
    fprintf(stderr, "DB 기반 로그인 전용입니다. -DUSE_DB로 컴파일하세요.\n"); // 만약 -DUSE_DB로 리눅스 내에 설정이 되어 있지 않을때
    return 1;
#else
    char usr_id[100], usr_pw[100]; // 널문자 제외 최대 99개의 문자를 입력 가능
    int i = 0; // 입력 기회

    if(!db_init()){
        fprintf(stderr, "[DB] DB 연결 실패 : 프로그램을 종료합니다.\n"); 
        return 1;
    }

    while(1){
        printf("아이디를 입력하세요 : ");
        scanf("%99s", usr_id); // 총 99개의 문자 입력 가능능

        printf("비밀번호를 입력하세요 : ");
        scanf("%99s", usr_pw);

        int success = 0; // 성공

        char db_pw[64], db_name[64];
        int r = db_fetch_user(usr_id, db_pw, sizeof(db_pw), db_name, sizeof(db_name));

        if(r == 1){ // 만약 아이디가 존재시
            if(strcmp(db_pw, usr_pw) == 0){ // pw비교 후 맞는 조건문 출력
                printf("로그인 되었습니다. (%s님)\n", db_name[0] ? db_name : usr_id); // 이름의 첫번째에 글자가 존재하면 이름 출력, 첫번째가 비어있으면 아이디 출력
                login(usr_id, "SUCCESS");
                success = 1; 
            } else {
                printf("아이디 또는 비밀번호가 틀렸습니다.\n");
                login(usr_id, "FAIL_PW");
                i++;
            }
        } else if(r == 0){ // 만약 아이디가 존재하지 않을 시
            printf("아이디 또는 비밀번호가 틀렸습니다.\n");
            login(usr_id, "FAIL_ID");
            i++;
        } else { // DB 오류
            printf("DB 오류로 로그인 처리에 실패했습니다.\n");
            login(usr_id, "FAIL_DB");
            i++;
        }

        if(success) break; // success 발생시 프로그램 반복문 종료

        if(i >= 3) {
            printf("5초 후에 다시 시도해주세요.\n");
            sleep(5); // 5초 잠금
            i = 0; // 기회를 다시 초기화
        }
    }

    db_close(); // DB 연결 종료
    return 0; // 프로그램 정상 종료
#endif
}
