#include <stdio.h>
#include <string.h>

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
                char *ptr = strtok(in, " ");
                
                int i = 0;
                while (ptr!=NULL){                  
                  printf("Token[%d] : %s\n", i, ptr);
                  i++;
                
                  ptr = strtok(NULL, " ");
                }
        }
}
