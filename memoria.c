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

// Gerenciamento de Mem�ria do PX-DOS

#include "memoria.h"

void gerenciamentomemoriaPadroes(GERMEMORIA *gerenciamentomemoria)
{

    return;
	
}

/*****************************************************************************************/

void gerenciamentomemoriaIniciar(GERMEMORIA *gerenciamentomemoria)
{

    gerenciamentomemoria->inicio = NULL;
    return;
	
}

/*****************************************************************************************/

void gerenciamentomemoriaTerm(GERMEMORIA *gerenciamentomemoria)
{

    return;
	
}

/*****************************************************************************************/

void gerenciamentomemoriaSuprir(GERMEMORIA *gerenciamentomemoria, void *buffer, tamanho_t tamanhobuffer)
{

    GERMEMORIAN *p, *l, *b;

    if (((int)buffer % ALINHAMENTO_MEMORIA) != 0)
    {
	
        tamanhobuffer -= (ALINHAMENTO_MEMORIA - (int)buffer % ALINHAMENTO_MEMORIA);
        buffer = (char *)buffer + (ALINHAMENTO_MEMORIA - (int)buffer % ALINHAMENTO_MEMORIA);
    
	}
    
    if ((tamanhobuffer % ALINHAMENTO_MEMORIA) != 0)
    {
	
        tamanhobuffer -= tamanhobuffer % ALINHAMENTO_MEMORIA;
   
   }
    
    p = gerenciamentomemoria->inicio;
    l = NULL;
	
    while ((p != NULL) && ((GERMEMORIAN *)buffer >= p))
    {
	
        l = p;
        p = p->proximo;
   
   }
        
    b = (GERMEMORIAN *)buffer;
        
    b->anterior = l;
    b->proximo = p;
        
    if (l != NULL)
    {
	
        l->proximo = b;
    
	}
    else
    {
	
        gerenciamentomemoria->inicio = b;
   
   }
        
    if (p != NULL)
    {
	
        p->anterior = b;
   
   }
        
    b->fixado = 1;
    b->tamanho = tamanhobuffer - GERMEMORIAN_SZ;
    b->alocado = 0;
	
    return;
	
}

/*****************************************************************************************/

void *gerenciamentomemoriaAlocar(GERMEMORIA *gerenciamentomemoria, tamanho_t bytes, int id)
{

    GERMEMORIAN *p, *n;
    tamanho_t antigobytes = bytes;
    
    if ((bytes % ALINHAMENTO_MEMORIA) != 0)
    {
	
        bytes += (ALINHAMENTO_MEMORIA - bytes % ALINHAMENTO_MEMORIA);
    
	}

       
    if (bytes < antigobytes)
    {
	
        return (NULL);
    
	}
    
    p = gerenciamentomemoria->inicio;
    
    while (p != NULL)
    {
	
        if ((p->tamanho >= bytes) && !p->alocado)
        {
		
            if ((p->tamanho - bytes) > GERMEMORIAN_SZ)
            {
			
                n = (GERMEMORIAN *)((char *)p + GERMEMORIAN_SZ + bytes);
                n->proximo = p->proximo;
                n->anterior = p;
                p->proximo = n;
                n->fixado = 0;
                n->tamanho = p->tamanho - bytes - GERMEMORIAN_SZ;
                n->alocado = 0;
                p->tamanho = bytes;
            
			}
			
            p->alocado = 1;
            p->id = id;
            break;
			
        }
		
        p = p->proximo;
    }
	
    if (p == NULL)
    {
	
        return (p);
   
   }
    else
    {
	
        return ((char *)p + GERMEMORIAN_SZ);
   
   }
   
}

/*****************************************************************************************/

void gerenciamentomemoriaLivre(GERMEMORIA *gerenciamentomemoria, void *ptr)
{

    GERMEMORIAN *p, *n, *l;

    ptr = (char *)ptr - GERMEMORIAN_SZ;
    
    p = gerenciamentomemoria->inicio;
    l = NULL;
    
    while (p != NULL)
    {
        if (p == ptr)
        {
            p->alocado = 0;
            
            n = p->proximo;
			
            if ((n != NULL) && !n->alocado && !n->fixado)
            {
			
                p->tamanho += n->tamanho + GERMEMORIAN_SZ;
                p->proximo = n->proximo;
            
			}
            
            if (!p->fixado && (l != NULL) && !l->alocado)
            {
			
                l->tamanho += p->tamanho + GERMEMORIAN_SZ;
                l->proximo = p->proximo;
            
			}
			
            break;
        }
		
        l = p;
        p = p->proximo;
    }
	
    return;
	
}

/*****************************************************************************************/

void gerenciamentomemoriaIdLivre(GERMEMORIA *gerenciamentomemoria, int id)
{
    GERMEMORIAN *p, *n, *l;

    p = gerenciamentomemoria->inicio;
    l = NULL;
    
    while (p != NULL)
    {
        if ((p->id == id) && p->alocado)
        {
            p->alocado = 0;
            
            n = p->proximo;
			
            if ((n != NULL) && !n->alocado && !n->fixado)
            {
			
                p->tamanho += n->tamanho + GERMEMORIAN_SZ;
                p->proximo = n->proximo;
            
			}
            
            if (!p->fixado && (l != NULL) && !l->alocado)
            {
			
                l->tamanho += p->tamanho + GERMEMORIAN_SZ;
                l->proximo = p->proximo;
          
		  }
        }
		
        l = p;
        p = p->proximo;
    }
	
    return;
	
}

/*****************************************************************************************/

/* Encontra o maior bloco dispon�vel */
tamanho_t gerenciamentomemoriaTamanhoMaximo(GERMEMORIA *gerenciamentomemoria)
{
    GERMEMORIAN *p, *n;
    tamanho_t max = 0;
    
    p = gerenciamentomemoria->inicio;
    
    while (p != NULL)
    {
	
        if ((p->tamanho > max) && !p->alocado)
        {
		
            max = p->tamanho;
        
		}
		
        p = p->proximo;
    }
	
    return (max);
	
}

/*****************************************************************************************/

/* Altera o bloco de mem�ria */

int gerenciamentomemoriaRealocar(GERMEMORIA *gerenciamentomemoria, void *ptr, tamanho_t novotamanho)
{
    GERMEMORIAN *p, *n, *z;
    tamanho_t antigobytes = novotamanho;
    
    if ((novotamanho % ALINHAMENTO_MEMORIA) != 0)
    {
	
        novotamanho += (ALINHAMENTO_MEMORIA - novotamanho % ALINHAMENTO_MEMORIA);
    
	}

    
    if (novotamanho < antigobytes)
    {
	
        return (-1);
    
	}
    

    ptr = (char *)ptr - GERMEMORIAN_SZ;
    
    p = gerenciamentomemoria->inicio;
    
    while (p != NULL)
    {
        if (p == ptr)
        {
           
            if (p->tamanho < (novotamanho + GERMEMORIAN_SZ))
            {
			
                return (-1);
           
		   }
            
         
            if ((novotamanho + GERMEMORIAN_SZ) < novotamanho)
            {
			
                return (-1);
            
			}
            
            /* Insere novo bloco de controle */
            n = (GERMEMORIAN *)((char *)p + GERMEMORIAN_SZ + novotamanho);
            n->proximo = p->proximo;
            n->anterior = p;
            p->proximo = n;
            n->fixado = 0;
            n->tamanho = p->tamanho - novotamanho - GERMEMORIAN_SZ;
            n->alocado = 0;
            p->tamanho = novotamanho;
            
            /* Combina cm pr�ximo bloco caso poss�vel */
            z = n->proximo;
			
            if ((z != NULL) && !z->alocado && !z->fixado)
            {
			
                n->tamanho += z->tamanho + GERMEMORIAN_SZ;
                n->proximo = z->proximo;
            
			}
            
            break;
        }
		
        p = p->proximo;
    }
    
    
    if (p == NULL)
    {
	
        return (-2);
    
	}

	return (0);
	
}

