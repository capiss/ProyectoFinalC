/************************************************************/  
/**************                         cliente.c           **********************/  
/************************************************************/  
/*********                   proceso cliente con sockets AF_UNIX   **********/  
/************************************************************/  

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
 int dfClient, longServer, resultado;  
 struct sockaddr_un dirUNIXServer;  
 struct sockaddr* puntSockServer;  

 puntSockServer = ( struct sockaddr* ) &dirUNIXServer;  
 longServer = sizeof ( dirUNIXServer );  

 dfClient = socket ( AF_UNIX, SOCK_STREAM, PROTOCOLO_DEFECTO );  
                 /* se crea un socket UNIX, bidireccional */  

 dirUNIXServer.sun_family = AF_UNIX;    /* tipo de dominio server */  
 strcpy ( dirUNIXServer.sun_path, "fichero" );   /* nombre server */  

 do    
  {  
   resultado = connect ( dfClient, puntSockServer, longServer );  
   if ( resultado == -1 ) sleep (1);   /* reintento */  
 }  
 while ( resultado == -1 );  
 do{
   while(leeFichero ( dfClient ));
   while(escribir(dfClient));
 }while(1);
 close ( dfClient );      /* cierra el socket */  
 exit (0);     /* buen fin */  

}  

escribir( int df ){
 printf("Para poder leer al servidor y terminar de escribir ponga un punto al final.\n");
 printf(" Vamos a escribir\n");
 int c;
 do{
   c=getchar();
   write(df,&c,sizeof(c));

 }while(!(c=='.'));
 printf(" Ya no podemos escribir\n");
 return (!(c=='.'));
}


/*************     leeFichero ( df )  *****************/  

leeFichero ( int df )  
{  
 char cad[200]; 
 printf("vamo a leer: \n"); 

 while (leeLinea ( df, cad ) );      /* lee hasta fin de la entrada */  
  //printf ("recibi: %s\n", cad );            /* e imprime lo leido */  
 return 0;
}  


/*************   leeLinea ( df, cad )  ******************/  
leeLinea ( int df, char *cad )  
{  
 int n;
 char c;
 do  
  { 
    n = read ( df,cad, 1 );          /* lectura de un caracter */ 
    c=*cad; 
    //printf("n=%d\n",n);
    //printf("*cad=%c\n",*cad);
    printf("%c",*cad);
    //printf("%c",c);
  }  
 //while ( n > 0 && (*cad)++ != '.' );   /* lee hasta NULL o fin entrada */  
 while ( (c)!= '.' );   /* lee hasta NULL o fin entrada */  
 printf("\n Hemos terminado\n");
 //return ( n > 0 );   /* devuelve falso si fin de entrada */  
 return !(c=='.');

}  



/************* fin de cliente.c ****************************/  
