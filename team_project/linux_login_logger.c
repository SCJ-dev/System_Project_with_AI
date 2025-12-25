#include <stdio.h>
#include <string.h> // strcmp, strlen 등 문자열 처리 관련 헤더파일
#include <unistd.h> // sleep 함수 관련 헤더파일
#include <time.h> // 시간 관련 헤더 파일

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
static MYSQL *g_conn = NULL; // 

static int db_init(void){
    g_conn = mysql_init(NULL);
    if(!g_conn) return 0;

    if(!mysql_real_connect(g_conn, "127.0.0.1", "secapp", "1234",
                           "security_project", 0, NULL, 0)){
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

    char esc_id[128];
    unsigned long id_len = strlen(user_id);
    if(id_len > 60) id_len = 60;
    mysql_real_escape_string(g_conn, esc_id, user_id, id_len);

    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO ACCESS_LOG (USER_ID, IP_ADDR, RESULT) VALUES ('%s','%s','%s')",
             esc_id, ip_addr, result);

    if(mysql_query(g_conn, query)){
        fprintf(stderr, "[DB] insert error : %s\n", mysql_error(g_conn));
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
    unsigned long id_len = strlen(user_id);
    if(id_len > 60) id_len = 60;
    mysql_real_escape_string(g_conn, esc_id, user_id, id_len);

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT PASSWORD, NAME FROM USER_TB WHERE ID='%s' LIMIT 1",
             esc_id);

    if(mysql_query(g_conn, query)){
        fprintf(stderr, "[DB] select error : %s\n", mysql_error(g_conn));
        return -1;
    }

    MYSQL_RES *res = mysql_store_result(g_conn);
    if(!res){
        fprintf(stderr, "[DB] store_result error : %s\n", mysql_error(g_conn));
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if(!row){
        mysql_free_result(res);
        return 0;
    }

    snprintf(out_pw, out_pw_sz, "%s", row[0] ? row[0] : "");
    snprintf(out_name, out_name_sz, "%s", row[1] ? row[1] : "");
    mysql_free_result(res);
    return 1;
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
    fprintf(stderr, "DB 기반 로그인 전용입니다. -DUSE_DB로 컴파일하세요.\n");
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

        int success = 0;

        char db_pw[64], db_name[64];
        int r = db_fetch_user(usr_id, db_pw, sizeof(db_pw), db_name, sizeof(db_name));

        if(r == 1){
            if(strcmp(db_pw, usr_pw) == 0){
                printf("로그인 되었습니다. (%s님)\n", db_name[0] ? db_name : usr_id);
                login(usr_id, "SUCCESS");
                success = 1;
            } else {
                printf("비밀번호가 틀렸습니다.\n");
                login(usr_id, "FAIL_PW");
                i++;
            }
        } else if(r == 0){
            printf("아이디가 틀렸습니다.\n");
            login(usr_id, "FAIL_ID");
            i++;
        } else {
            printf("DB 오류로 로그인 처리에 실패했습니다.\n");
            login(usr_id, "FAIL_DB");
            i++;
        }

        if(success) break;

        if(i >= 3) {
            printf("5초 후에 다시 시도해주세요.\n");
            sleep(5); // 5초 잠금
            i = 0; // 기회를 다시 초기화화
        }
    }

    db_close();
    return 0;
#endif
}
