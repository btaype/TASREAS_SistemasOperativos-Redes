#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include<fcntl.h>

#include <errno.h> 
#include <sys/wait.h>
int main()
{
        int     fd[2], nbytes;
        pid_t   childpid;
        
        char    readbuffer[80];
        int c;

        pipe(fd);

        if((childpid = fork()) == -1)
        {
                perror("fork");
                exit(1);
        }
 
        

        if(childpid == 0)
        {       int file_desc = open("input.txt", O_RDONLY);
                int c;
                printf("Child program:"); 
                char *args[] = {"./p2", NULL};  

                
                dup2(file_desc, 0);
                int file_desc2 = open("output.txt",O_WRONLY | O_APPEND); 
    
  
                dup2(file_desc2, 1) ; 

                execv("./p2", args); 
                
        }
        else
        {
          
        }

        return(0);
}