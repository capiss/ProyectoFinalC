#include <stdio.h>  
#include <stdlib.h>  
#include <signal.h>  
#include <sys/types.h>  
#include <sys/socket.h>  

#include <sys/un.h>           /*  para sockets UNIX  */  

#define PROTOCOLO_DEFECTO 0  

/****************************************************/  
main()  
{  
 int dfServer, dfClient, longServer, longClient;  

 struct sockaddr_un dirUNIXServer;  
 struct sockaddr_un dirUNIXClient;  

 struct sockaddr* puntSockServer;  
 struct sockaddr* puntSockClient;  
 signal ( SIGCHLD, SIG_IGN );    /*  para no crear zombies */  

 puntSockServer = ( struct sockaddr* ) &dirUNIXServer;  
 longServer = sizeof ( dirUNIXServer );  
 puntSockClient = ( struct sockaddr* ) &dirUNIXClient;  
 longClient = sizeof ( dirUNIXClient );  

 dfServer = socket ( AF_UNIX, SOCK_STREAM, PROTOCOLO_DEFECTO );  
                 /* se crea un socket UNIX, bidireccional */  
 dirUNIXServer.sun_family = AF_UNIX;    /* tipo de dominio */  
 strcpy ( dirUNIXServer.sun_path, "fichero" );   /* nombre */  
 unlink ( "fichero" );  
 bind ( dfServer, puntSockServer, longServer );   /* crea el fichero */  
                                          /* o sea, nombra el socket */  
 printf ("\n estoy a la espera \n"); 
 listen ( dfServer, 5 );  
 while (1)  
   {  
     dfClient = accept ( dfServer, puntSockClient, &longClient );  
                              /* acepta la conexion cliente */  
     printf ("\n acepto la conexion \n");  
 printf("Para poder leer al cliente y terminar de escribir ponga un punto al final.\n"); 
     if ( fork() == 0 )  /* crea hijo y envia fichero */  
       {  
         //escribeFichero ( dfClient );  
  //         escribir( dfClient );
          do{
		while(escribir(dfClient));
   		while(leeFichero( dfClient ));
	  }while(1);

         close ( dfClient );        /* cierra el socket */  
         exit ( 0 );    
        }  
     else  
       close ( dfClient );      /* cierra el descriptor cliente */  
   }                            /* en el padre */  
}  

escribir( int df ){
 printf(" Vamos a escribir\n");
 int c;
 do{
   c=getchar();
   write(df,&c,sizeof(c));

 }while(!(c=='.'));
 printf(" Ya no podemos escribir\n");
 return (!(c=='.'));
}


/******** funcion escribeFichero( df ) ***************/  
escribeFichero ( int df )  
{  
 static char* linea1 = "esta es la linea 1, ";  
 static char* linea2 = "y esta la linea 2. ";  

 write ( df, linea1, strlen (linea1) + 1 );   
  write ( df, linea2, strlen (linea2) + 1 );  
}  


/*************     leeFichero ( df )  *****************/

leeFichero ( int df )
{
 char cad[200];
 printf("vamo a leer: \n");

 while (leeLinea ( df, cad ) );      /* lee hasta fin de la entrada */   
  /**printf ("recibi: %s\n", cad );            /* e imprime lo leido */ 
 return 0;
}
  
  
/*************   leeLinea ( df, cad )  ******************/
leeLinea ( int df, char *cad )
{
 int n;
 char c;
 do{
    n = read ( df,cad, 1 );          /* lectura de un caracter */
    c=*cad;
    printf("%c",*cad);
  }
 while ( (c)!= '.' );   /* lee hasta NULL o fin entrada */
 printf("\n Hemos terminado\n");
 return !(c=='.');

}

/******************   fin de servidor.c   ***********************/  
