#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
int c=5;
int tamano=100;
 
int main (){
    char **frases = (char **) malloc(c * sizeof(char *));
    for (int i = 0; i < c; i++) {
        frases[i] = (char *)malloc(tamano * sizeof(char));  
        if (frases[i] == NULL) {
            printf("Error al asignar memoria para la frase %d\n", i);
            return 1;
        }
    }
    
    for(int n=0;n<c;n++ ){
        fgets(frases[n],tamano,stdin);
        
    }
   
     for (int i = 0; i < c; i++) {
        printf("%s",frases[i]);
    }
    
}