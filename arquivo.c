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
// Implementação stdio

#include <stdio.h>

#include "fat.h"
#include "bios.h"
#include "bnpxdos.h"
#include "naousado.h"

static ARQUIVOFAT arquivofat;
extern FAT gfat;

ARQUIVO *abrirarquivo(const char *nome, const char *modo)
{

    naousado(modo);
	
    abrirarquivofat(&gfat, nome, &arquivofat);
	
    return ((ARQUIVO *)1);
	
}

/*****************************************************************************************/

int fechar(ARQUIVO *ArquivoEmDisco)
{

    naousado(ArquivoEmDisco);
	
    return (0);
	
}

/*****************************************************************************************/

tamanho_t lerarquivo(void *buf, tamanho_t tamanho, tamanho_t buffat, ARQUIVO *ArquivoEmDisco)
{
    tamanho_t retorno;
    
    naousado(tamanho);
    naousado(ArquivoEmDisco);
    ret = lerarquivofat(&gfat, &arquivofat, buf, buffat);
	
    return (retorno);
	
}
