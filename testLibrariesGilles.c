/* Test de get_tk_str et get_tk_strB
 * Test de la nouvelle fonction "get_tk_strB" basée sur la précédente "get_tk_str".
 * A l'intérieur elle intègre un "algorithme" pour éliminer les espaces en début de ligne, 
 * ce qui permet d'indiquer l'absence de token p.e NBR_ALM , ALM_NUM, ALM_TXT
 * 
* Pour réaliser ce banc de test, le fichier de configuration a une structure comme suit 
*NBR_ALM: 8
*ALM_NUM: n
*ALM_TXT: TEXTE xxxx
* 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef unsigned char 	PI_TBYTE;	/* 8  bits unsigned */
typedef unsigned short 	PI_TWORD;	/* 16 bits unsigned */
typedef unsigned int	PI_TLONG;	/* 32 bits unsigned */

/* #if defined AXPF_WINDOWS || defined AXPF_LINUX */
int findnstr( int i, char *buf, char *token, int bufsz )
{
	char	*ptr;
	if( i>0 )
	{
		i=i-1;
		ptr = strstr (buf+i,token);
		if (!ptr)
			return (0);

		return (ptr - buf + i + 1);
	}
	return(0);
}


int  pi_file_readchar(FILE *stream, int *read0d)

{
  int c;

  c = fgetc (stream);
  if (c == 0xd)
  {
    *read0d = 1;
    return ('\n');
  }

  if (c == 0xa)
  {
    if (*read0d)
    {
	*read0d = 1;
	return (pi_file_readchar (stream, read0d));
    } else {
	return ('\n');
    }
  }

  *read0d = 0;

  return (c);
}

int	pi_file_readline(FILE *stream, char *buff, int buffsz)

{
	int	c;
	int     read0d = 0;

	if (!buff || !stream || !buffsz)
		return (0);

	buffsz--;

	c = pi_file_readchar(stream,&read0d);

	while (buffsz && c != -1 && c != '\n')
	{
		*buff = c;
		buff++;
		c = pi_file_readchar(stream,&read0d);
		buffsz--;
	}

	*buff = '\0';

	if (c == -1)
		return (0);

	return (1);
}


//Routine originale Gilles
int get_tk_str( FILE *fp, unsigned char *buf,unsigned char *s, unsigned char *token, int len_token )
/*
FILE *fp;
unsigned char *buf, *s, *token;
int len_token;
*/
{
	int pos1=0, len2, len3;


	while( feof( fp )==0 )// <<<<<<<<<<<<<<<<<<<<< Il continue à chercher tant qu'il n'atteint pas la fin du fichier fp 
	{

	  /*		fgets( buf, BUFSIZ, fp ); */
	  pi_file_readline( fp, buf, BUFSIZ );
		len3=strlen( buf );
		if( (pos1=findnstr( 1, buf, token, len3 ))!=0 )
		{
			len2=len3-( len_token+pos1-1 );
			len_token+=(pos1-1);
			memcpy( s, buf+len_token, len2 );
			/*
#if defined AXPF_LINUX
			len2--;
#endif
			*/
			s[ len2 ]=0;
			return( len2 );
		}
	}
	return( -1 );
}



/*Dans cette version, si le "token" est manquant, ne cherchera pas le suivant, elle renvoie une erreur -1. 
 * Elle implémente aussi un algorithme pour éliminer les espaces
 * 
 *  while (  buf[0] <=32 && (pt=strchr(buf,32)) !=NULL )//Pointeur sur la première occurrence de 32 SPACE
                {sprintf (buf,"%s",pt+1) ;}//Copie du pointeur+1 .. en avant
 */

int get_tk_strB( FILE *fp, unsigned char *buf,unsigned char *s, unsigned char *token, int len_token )

{
	int pos1=0, len2, len3;
    buf[0]=0;// Dirty ...Efface le buffer
    

    while (feof( fp )==0 &&  (strlen(buf)<1)  ){  //Récupère une ligne tant qu'elle n'atteint pas la fin ou est un saut de ligne 1 char 
	    pi_file_readline( fp, buf, BUFSIZ );
              
        char *pt;//pointeur vers un caractère 
        
        if (strlen(buf)>1) { //supprime  les espaces au début  de la chaine , plus de un char 
            while (  buf[0] <=32 && (pt=strchr(buf,32)) !=NULL )//Pointeur sur la première occurrence de 32 SPACE
                {sprintf (buf,"%s",pt+1) ;}//Copie du pointeur+1 .. en avant
        }    
    }
    
        if (strlen(buf)<1){return -1;} //Pas de buffer ,chaîne vide strlen =0
        if (feof( fp )!=0){ return -1;}//Fin du fichier , sortir , pas trouve 
                
		len3=strlen( buf );
		if( (pos1=findnstr( 1, buf, token, len3 ))!=0 )
		{
			len2=len3-( len_token+pos1-1 );
			len_token+=(pos1-1);
			memcpy( s, buf+len_token, len2 );
			/*
#if defined AXPF_LINUX
			len2--;
#endif
			*/
			s[ len2 ]=0;
			return( len2 );
		}
	return( -1 );
}



/*-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------*/

/* MAIN PROGRAM */

int main()
{
	
    unsigned char info_token[128],read_ligne[ 1024] ;
    int nbr_alm;
    
	
	//Open file
	
	FILE *fp;//Config File
	fp	=	fopen( "panneau.txt", "r" );
	
    //Read NBR_ALM
    if (get_tk_strB( fp,read_ligne,info_token,(unsigned char *) "NBR_ALM:", 8 )<=0 ){
            printf(  " NBR_ALM: is not defined !\n" );
			exit(0);        
    }
    
    nbr_alm=atoi((char *)info_token);//récupére l'en-tête du fichier de configuration nbr_alm
    
    printf ("NBR_ALM:%d \n",nbr_alm);//ok
    
        
    /*-------------------------------------------------------------------------------*/
    /*-------------------------------------------------------------------------------*/
    /*-------------------------------------------------------------------------------*/
    
	for (int alm=0 ;alm<nbr_alm;alm++){
        
		if( get_tk_strB( fp ,read_ligne,info_token, (unsigned char *)"ALM_NUM:", 8 )<=0 )
		{
            printf(  "n%d: ALM_NUM: is not defined !\n",alm );
			exit(0);
		}else{printf(  "ALM_NUM:%s OK  !\n",info_token );}
        
//MESSAGE TXT 
		if( get_tk_strB( fp,read_ligne,info_token, (unsigned char *)"ALM_TXT:", 8 )<=0 )
		{           
            printf( "n%d: ALM_TXT: is not defined\n", alm);
			exit(0);
		} else{printf("ALM_TXT:%s OK  !\n\n",info_token );}      
	}
    
    
	fclose (fp);
		
	return 0;
}

