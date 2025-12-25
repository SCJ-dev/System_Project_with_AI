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
 *  1 : ID 존재 (out_pw/out_name 호가 틀렸습니다.\n이디;
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
