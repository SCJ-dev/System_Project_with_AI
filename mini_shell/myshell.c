#include <stdio.h>
#include <string.h>
#include <unistd.h> // fork, execvp 등을 쓰기 위해 필요
#include <sys/wait.h> // wait 함수를 쓰기 위해 필요

int main() {

	char in[1024]; // 명령어 입력받을 문자열

	for (;;){ // 무한반복
		printf("MY_SHELL$ "); 
		if (fgets(in, 1024, stdin) == NULL){ // fgets(저장할 곳, 최대크기, 입력소스 (어디에 입력 받을 것인가?)) = 문자열을 입력받을 때 사용하는 가장 안전한 함수
		  break;
		}
		in[strcspn(in, "\n")] = 0; // strcspn(변수명, 만나면 멈추고 싶은 글자) = 어떤 글자가 나오면 멈춤
		// 엔터키(\n)를 찾아서 0으로 바꾸고 문장 끝을 알림
		
		if(strcmp(in, "exit") == 0){ // strcmp(변수명, 비교할 문자) = 문자열 비교 -> 문자열 끝이 0이면?
		printf("program off\n");
		break;
		}
                          
        char *args[10]; // 명령어를 담을 배열 최대 10개     
        int i = 0;
    	char *ptr = strtok(in, " "); // strtok(분리하고자하는 문자열, 구분자로 사용할 문자들의 집합을 가르키는 문자열)
		// 1. 자른 단어를 배열에 차곡차곡 담는다
        while (ptr!=NULL){                  
                args[i] == ptr;
                i++;                                  
			    ptr = strtok(NULL, " "); // in에 마지막 부분이 아무것도 없을때
                }
        args[i] = NULL;// 한 명령어가 종료
                
        if (i == 0) continue; // 명령어가 없으면 넘겨라
                
        pid_t pid = fork(); // fork는 현재 프로세스를 복제함, pid_t는 그냥 프로세스 번호 담는 int형 수
                
        if (pid == 0){ // 자식 프로세스 : 여기서 실제 명령어 수행
			execvp(args[0], args); // execvp(실행하려는 변수명, 인수목록)
            printf("ERROR : Comand Not Found\n");
            return 1; // 자식이 죽음 return 0 제외하면 오류 발생
            }
        else{
            wait(NULL); // 부모프로세스 : 자식이 끝날 때까지 기다림
        }
        }
}
