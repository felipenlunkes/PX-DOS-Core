/*********************************************************************/
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*                                                                   */
/*   #$$%%@!#$%                                                      */
/*   !!@#!$!$!$         Sistema Operacional PX-DOS                   */
/*   !@@#   #$%                                                      */
/*   #$$%   &*$                                                      */
/*   $#%@   @#&                                                      */
/*   #%$&*(@*@&                                                      */
/*   @#$@$#@$%$       2013-2022 (c) Felipe Miguel Nery Lunkes        */
/*   $%&*                Todos os direitos reservados                */
/*   @#&*                                                            */
/*   @&*%       Esse software se baseia em códigos disponíveis       */
/*   #&*@                     em domínio público                     */
/*                                                                   */
/*                                                                   */
/*********************************************************************/
/*

Copyright (c) 2013-2022, Felipe Miguel Nery Lunkes
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
