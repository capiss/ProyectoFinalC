#include <sys/types.h>
#include </usr/include/linux/input.h>
#include <stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include <dirent.h>

void cifrar(char *archivo);

int main(){
  DIR *d = opendir("prueba");
  struct dirent *dentry;
 
  while((dentry=readdir(d))!=NULL){
//    printf("%s\n",dentry->d_name);
    if(strcmp(dentry->d_name,".")!=0 || strcmp(dentry->d_name,"..")==0 ){
      //printf("%s\n",dentry->d_name);
      char *ruta=(char *) calloc(sizeof(char),512); 
      strcpy(ruta, "./prueba/" );
      strcat(ruta, dentry->d_name );
      //printf("%s",ruta);
      cifrar(ruta);
    }
  }
 
  return 0;
}

void cifrar(char *archivo){
  int fd,i=0;
  char palabra;
  printf("-----------------------------------------------\n");
  printf("abriendo archivo: %s\n",archivo);
  fd=open(archivo,O_RDWR);
  int llave=41;
  if(fd!=-1){ 
     struct stat fileStat;
     if(fstat(fd,&fileStat) < 0)    
         exit(1);
     char *palabraCifrada=(char *) calloc(sizeof(char),fileStat.st_size);
     int nread=0;
   while( ( nread=read(fd,&palabra,sizeof(palabra)) ) >0 ){
       palabraCifrada[i]=palabra^llave;
       printf("%c \t:\t %c\n",palabra,palabraCifrada[i]);
       lseek(fd,-1,SEEK_CUR);
       write(fd,&palabraCifrada[i++],nread);
   }
   close(fd);
  }
}
