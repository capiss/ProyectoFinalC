#include <sys/types.h>
#include </usr/include/linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

void cifrar(char *archivo);
void abrirDirectorio(char *archivo);

int main(){
  abrirDirectorio("prueba");
  return 0;
}

void abrirDirectorio(char *directorio){
  // Abrimos el directorio actual
  DIR *d = opendir(directorio); //
  struct dirent *dentry;
  //mientras podamos leer los archivos dentro del directorio actual ejecutara el codigo
  while((dentry=readdir(d))!=NULL){
  //    printf("%s\n",dentry->d_name); //Imprime los archivos dentro del directorio
  // Validamos que no abra los archivos de . y ..
    if(strcmp(dentry->d_name,".")!=0 && strcmp(dentry->d_name,"..")!=0 ){
      // generamos un variable para guardar la ruta. Esto para poder cifrar los archivos que esten dentro de otro directorio.
      char *ruta=(char *) calloc(sizeof(char),512);
      //copiamos la ruta
       strcpy(ruta, directorio ); //ruta=./prueba/
      // le agregamos (concatenamos) el nombre del archivo
      strcat(ruta, "/" ); // ruta = ./prueba/
      strcat(ruta, dentry->d_name ); // ruta = ./prueba/nombre_archivo
      //printf("%s\n",dentry->d_name);
      //Mandamos a llamar la funcion cifrar
      cifrar(ruta);
    }
  }
}
void cifrar(char *archivo){
  int fd,i=0,fdw;
  char palabra;
  printf("-----------------------------------------------\n");
  printf("abriendo archivo: %s\n",archivo);
  int llave=41;
  // abrimos el archivo en modo de lectura y escrituta.
  fd=open(archivo,O_RDONLY);
  if(fd!=-1){
    struct stat fileStat;
    if(fstat(fd,&fileStat) < 0)    
      exit(1);
    if (S_ISDIR(fileStat.st_mode)) {
      abrirDirectorio(archivo);
    }else{
      close(fd);
      fd=open(archivo,O_RDWR);
      //usamos stat para saber el tamaño del archivo
      // Generamos una variable del tamaño del archivo
      char *palabraCifrada=(char *) calloc(sizeof(char),fileStat.st_size);
      int nread=0;
      // Leemos el archivo
      while( ( nread=read(fd,&palabra,sizeof(palabra)) ) >0 ){
        //ciframos
        palabraCifrada[i]=palabra^llave;
        printf("%c \t:\t %c\n",palabra,palabraCifrada[i]);
        //regresa uno en el puntero al archivo, para escrbir en el la palabra cifrada.
        lseek(fd,-1,SEEK_CUR);
        //escribimos en el archivo
        write(fd,&palabraCifrada[i++],nread);
      }
    }
  }
  //La llave para cifrar

  #if 0
    printf("Information for %s\n\n",archivo);
    printf("File Size: \t\t%d bytes\n",fileStat.st_size);
    printf("Number of Links: \t%d\n",fileStat.st_nlink);
    printf("File inode: \t\t%d\n",fileStat.st_ino);

    printf("File Permissions: \t");
    printf( (S_ISDIR(fileStat.st_mode)) ? "d" : "-");
    printf( (fileStat.st_mode & S_IRUSR) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWUSR) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXUSR) ? "x" : "-");
    printf( (fileStat.st_mode & S_IRGRP) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWGRP) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXGRP) ? "x" : "-");
    printf( (fileStat.st_mode & S_IROTH) ? "r" : "-");
    printf( (fileStat.st_mode & S_IWOTH) ? "w" : "-");
    printf( (fileStat.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n"); 
    printf("-----------------------------------------------\n");
  #endif
  close(fd);
}
