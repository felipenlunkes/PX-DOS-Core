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

/* Encontra o maior bloco disponível */
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

/* Altera o bloco de memória */

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
            
            /* Combina cm próximo bloco caso possível */
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

