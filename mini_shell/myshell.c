#include <stdio.h>
#include <string.h>
#include <unistd.h> // fork, execvp
#include <sys/wait.h> // wait

int main() {

	char in[1024]; // user 

	for (;;){
		printf("MY_SHELL$ ");
		if (fgets(in, 1024, stdin) == NULL){
		  break;
		}
		in[strcspn(in, "\n")] = 0;
		
		if(strcmp(in, "exit") == 0){
		printf("program off\n");
		break;
		}
                          
                char *args[10];                
                int i = 0;
                char *ptr = strtok(in, " ");
                while (ptr!=NULL){                  
                  args[i] == ptr;
                  i++;                
                  ptr = strtok(NULL, " ");
                }
                args[i] == NULL;// command end
                
                if (i == 0) continue;
                
                pid_t pid = fork();
                
                if (pid == 0){
                  execvp(args[0], args);
                  printf("ERROR : Comand Not Found\n");
                  return 1;
                }
                else{
                  wait(NULL);
                }
        }
}
