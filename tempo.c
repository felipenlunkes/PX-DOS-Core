/*********************************************************************/
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*   #$$%%@!#$%                                                      */
/*   !!@#!$!$!$         Sistema Operacional PX-DOS ®                 */
/*   !@@#   #$%                                                      */
/*   #$$%   &*$                                                      */
/*   $#%@   @#&                                                      */
/*   #%$&*(@*@&                                                      */
/*   @#$@$#@$%$     © 2013-2016 Felipe Miguel Nery Lunkes            */
/*   $%&*                Todos os direitos reservados                */
/*   @#&*                                                            */
/*   @&*%                                                            */
/*   #&*@                                                            */
/*                                                                   */
/*                                                                   */
/* O PX-DOS ® é marca registrada de Felipe Miguel Nery Lunkes no     */
/* Brasil. © 2013-2016 Felipe Miguel Nery Lunkes. Todos os direitos  */
/* reservados. A reprodução total ou parcial, de quaisquer trechos   */
/* do código aqui presente é expressamente probida, sendo passível   */
/* de punição legal severa.                                          */
/*                                                                   */
/* Copyright © 2013-2016 Felipe Miguel Nery Lunkes                   */
/* Todos os direitos reservados.                                     */
/*                                                                   */
/*********************************************************************/ 

#include <time.h>

#include "tempo.h"


// Converte para formato DOS

void tempo_para_dos(tempo_t agora, void *dos)
{
    struct tm *ts;
    unsigned long x;
    unsigned char *arr = (unsigned char *)dos;

    ts = horalocal(&agora);
    x = ts->tm_ano - 80;
    x = (x << 4) | (ts->tm_mes + 1);
    x = (x << 5) | ts->tm_diames;
    x = (x << 5) | ts->tm_hora;
    x = (x << 6) | ts->tm_minuto;
    x = (x << 5) | (ts->tm_segundo / 2);
    arr[0] = (unsigned char)((x >> 16) & 0xff);
    arr[1] = (unsigned char)((x >> 24) & 0xff);
    arr[2] = (unsigned char)(x & 0xff);
    arr[3] = (unsigned char)((x >> 8) & 0xff);
}

/*****************************************************************************************/

tempo_t dos_para_tempo(void *dos)
{
    struct tm tms;
    tempo_t tt;
    unsigned long x;
    unsigned char *arr = (unsigned char *)dos;

    x = ((unsigned long)arr[0]) | ((unsigned long)arr[1] << 8) 
        | ((unsigned long)arr[2] << 16) | ((unsigned long)arr[3] << 24);
		
    tms.tm_segundo = (x & 0x1f) * 2;
	
    x >>= 5;
	
    tms.tm_minuto = x & 0x3f;
	
    x >>= 6;
	
    tms.tm_hora = x & 0x1f;
	
    x >>= 5;
	
    tms.tm_diames = x & 0x1f;
	
    x >>= 5;
	
    tms.tm_mes = (x & 0x0f) - 1;
	
    x >>= 4;
	
    tms.tm_ano = (x & 0x7f) + 80;
	
    tms.tm_eHorarioDeVerao = -1;
	
    tt = fazertempo(&tms);
	
    return (tt);
}

