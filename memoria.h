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
//
// Gerenciamento de Memória do PX-DOS


#ifndef MEMORIA_INCLUIDA
#define MEMORIA_INCLUIDA

#include <stddef.h>

typedef struct gerenciamentomemorian {

    struct gerenciamentomemorian *proximo;
	
    struct gerenciamentomemorian *anterior;
	
    int fixado;
	
    tamanho_t tamanho;
	
    int alocado;
	
    int id;
	
} GERMEMORIAN;

typedef struct {

    GERMEMORIAN *inicio;
	
} GERMEMORIA;

#define ALINHAMENTO_MEMORIA 16

#define GERMEMORIAN_SZ \
  ((sizeof(GERMEMORIAN) % ALINHAMENTO_MEMORIA == 0) ? \
   sizeof(GERMEMORIAN) : \
   ((sizeof(GERMEMORIAN) / ALINHAMENTO_MEMORIA + 1) * 16))
   
void gerenciamentomemoria_Padroes(GERMEMORIA *gerenciamentomemoria);

void gerenciamentomemoria_Iniciar(GERMEMORIA *gerenciamentomemoria);

void gerenciamentomemoriaTerm(GERMEMORIA *gerenciamentomemoria);

void gerenciamentomemoriaSuprir(GERMEMORIA *gerenciamentomemoria, void *buffer, tamanho_t tamanhobuffer);

void *gerenciamentomemoriaAlocar(GERMEMORIA *gerenciamentomemoria, tamanho_t bytes, int id);

void gerenciamentomemoriaLivre(GERMEMORIA *gerenciamentomemoria, void *ptr);

void gerenciamentomemoriaIdLivre(GERMEMORIA *gerenciamentomemoria, int id);

tamanho_t gerenciamentomemoriaTamanhoMaximo(GERMEMORIA *gerenciamentomemoria);

int gerenciamentomemoriaRealocar(GERMEMORIA *gerenciamentomemoria, void *ptr, tamanho_t novotamanho);

#endif
