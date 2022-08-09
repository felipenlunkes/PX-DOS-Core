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

#include <stddef.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "naousado.h"
#define fputs(s, b) (s)

void despejarbuffer(void *, int);

typedef int ARQUIVO;

/*****************************************************************************************/

static int examine(const char **formt, ARQUIVO *arquivoIO, char *s, lista_va *arg, 
                   int contadorch);
                   
static void enviarVia(int via)
{
    unsigned char buf[1];
    
    if (via == '\n')
    {
	
        buf[0] = '\r';
        despejarbuffer(buf, 1);
		
    }
	
    buf[0] = (unsigned char)via;
	
    despejarbuffer(buf, 1);
	
    return;
	
}

/*****************************************************************************************/

static int vvprintf(const char *formato, lista_va arg, ARQUIVO *arquivoIO, char *s);

/*****************************************************************************************/

int printf(const char *formato, ...)
{
    lista_va arg;
    int ret;
    char buf[1];
  
    inicio_va(arg, formato);
    ret = vsprintf(buf, formato, arg);
    fim_va(arg);
	
    return (ret);
	
}
    
/*****************************************************************************************/
	
static int vvprintf(const char *formato, lista_va arg, ARQUIVO *arquivoIO, char *s)
{
    int leme = 0;
    int vint;
    char *vcptr;
    int contadorch = 0;
    tamanho_t tamanho;
    char numbuf[50];
    char *nptr;

    while (!leme)
    {
	
        if (*formato == '\0')
        {
		
            leme = 1;
			
        }
		
        else if (*formato == '%')
        {
		
            formato++;
			
            if (*formato == 'd')
            {
			
                int neg = 0;
                
                vint = va_arg(arg, int);
				
                if (vint < 0)
                {
				
                    neg = 1;
                    vint = -vint;
					
                }
				
                nptr = numbuf;
				
                do 
                {
				
                    *nptr++ = (char)('0' + vint % 10);
                    vint /= 10;
					
                } while (vint > 0);
               
			   if (neg)
                {
				
                    *nptr++ = '-';
					
                }
				
                do
                {
				
                    nptr--;
                    enviarVia(*nptr);
                    contadorch++;
					
                } while (nptr != numbuf);
            }
			
            else if (*formato == 's')
            {
                
				vcptr = va_arg(arg, char *);
                
				if (arquivoIO == NULL)
                {
				
                    tamanho = tamanhostring(vcptr);
                    despejarbuffer(vcptr, tamanho);
/*                    memcpy(s, vcptr, tamanho); */
                    s += tamanho;
                    contadorch += tamanho;
					
                }
				
                else
                {
				
                    fputs(vcptr, arquivoIO);
                    contadorch += tamanhostring(vcptr);
					
                }
				
            }
			
            else if (*formato == 'c')
            {
			
                vint = va_arg(arg, int);
                enviarVia(vint);
                contadorch++;
				
            }
			
            else if (*formato == '%')
            {
			
                enviarVia('%');
                contadorch++;
				
            }
			
            else
            {
                int extraCh;
                
                extraCh = examine(&formato, arquivoIO, s, &arg, contadorch);
                contadorch += extraCh;
              
			  if (s != NULL)
                {
				
                    s += extraCh;
					
                }
            }
        }
		
        else 
        {
		
            enviarVia(*formato);
            contadorch++;
			
        }
		
        formato++;
    }
	
    return (contadorch);
}

/*****************************************************************************************/

static int examine(const char **formt, ARQUIVO *arquivoIO, char *s, lista_va *arg, 
                   int contadorch)
{
    int extraCh = 0;
    int bandeiraMenos = 0;
    int bandeiraMais = 0;
    int bandeiraEspaco = 0;
    int bandeiraHash = 0;
    int bandeiraZero = 0;
    int tamanho = 0;
    int precisao = -1;
    int meio = 0;
    int lng = 0;
    int especificador = 0;
    int leme;
    long lvalor;
    unsigned long ulvalor;
    char *svalor;
    char trabalho[50];
    int x;
    int y;
    int bytes_disponiveis_no_setor;
    const char *formato;
    int base;
    int preencherCh;
    int neg;
    int comprimento;

    naousado(contadorch);    
    formato = *formt;
	
    /* Processando bandeiras */
	
    leme = 0;
	
    while (!leme)
    {
        switch (*formato)
        {
            case '-': bandeiraMenos = 1;
                      break;
            case '+': bandeiraMais = 1;
                      break;
            case ' ': bandeiraEspaco = 1;
                      break;
            case '#': bandeiraHash = 1;
                      break;
            case '0': bandeiraZero = 1;
                      break;
            default:  leme = 1;
                      break;
        }
		
        if (!leme)
        {
		
            formato++;
			
        }
		
        else
        {
           
		   if (bandeiraEspaco && bandeiraMais)
            {
			
                bandeiraEspaco = 0;
				
            }
			
            if (bandeiraMenos)
            {
			
                bandeiraZero = 0;
				
            }
        }
    }
    
  
    if (eumdigito((unsigned char)*formato))
    {
        while (eumdigito((unsigned char)*formato))
        {
		
            tamanho = tamanho * 10 + (*formato - '0');
            formato++;
			
        }
    }
    
   
    if (*formato == '.')
    {
        formato++;
        precisao = 0;
        while (eumdigito((unsigned char)*formato))
        {
		
            precisao = precisao * 10 + (*formato - '0');
            formato++;
			
        }
    }
    
  
    if (*formato == 'h')
    {
	
        meio = 1;
		
    }
	
    else if (*formato == 'l')
    {
	
        lng = 1;
		
    }
	
    else if (*formato == 'L')
    {
	
        lng = 1;
		
    }
	
    else
    {
	
        formato--;
		
    }
	
    formato++;

    if (precisao < 0)
    {
	
        precisao = 1;
		
    }    
   
    especificador = *formato;
    
    if (strchr("dxXuiop", especificador) != NULL)
    {
	
        if (especificador == 'p')
        {
		
            lng = 1;
			
        }
		
        
        if (lng)
        {
		
            lvalor = va_arg(*arg, long);
			
        }
		
        else if (meio)
        {
		
            lvalor = va_arg(*arg, short);
			
        }
		
        else
        {
		
            lvalor = va_arg(*arg, int);
			
        }
		
        ulvalor = (unsigned long)lvalor;
		
        if ((lvalor < 0) && ((especificador == 'd') || (especificador == 'i')))
        {
		
            neg = 1;
            ulvalor = -lvalor;
			
        }
		
        else
        {
		
            neg = 0;
			
        }
		
        if ((especificador == 'X') || (especificador == 'x') || (especificador == 'p'))
        {
		
            base = 16;
			
        }
		
        else if (especificador == 'o')
        {
		
            base = 8;
			
        }
		
        else
        {
		
            base = 10;
			
        }
		
        if (especificador == 'p')
        {
		
           


            precisao = 9;
			
        }
		
        x = 0;
		
        while (ulvalor > 0)
        {
		
            bytes_disponiveis_no_setor = (int)(ulvalor % base);
			
            if (bytes_disponiveis_no_setor < 10)
            {
			
                trabalho[x] = (char)('0' + bytes_disponiveis_no_setor);
				
            }
			
            else
            {
                if ((especificador == 'X') || (especificador == 'p'))
                {
				
                    trabalho[x] = (char)('A' + (bytes_disponiveis_no_setor - 10));
					
                }
				
                else
                {
				
                    trabalho[x] = (char)('a' + (bytes_disponiveis_no_setor - 10));
					
                }
            }
            x++;
			


            if ((x == 4) && (especificador == 'p'))
            {
			
                trabalho[x] = ':';
                x++;
				
            }
			
                     
            ulvalor = ulvalor / base;
        }
		

        if (especificador == 'p')
        {
            while (x < 5)
            {
			
                trabalho[x] = (x == 4) ? ':' : '0';
                x++;
				
            }
        }
                    
        while (x < precisao)
        {
		
            trabalho[x] = '0';
            x++;
			
        }
		
        if (neg)
        {
		
            trabalho[x++] = '-';
			
        }
		
        if (bandeiraZero)
        {
		
            preencherCh = '0';
			
        }
		
        else
        {
		
            preencherCh = ' ';
			
        }
		
        y = x;
		
        if (!bandeiraMenos)
        {
		
            while (y < tamanho)
            {
			
                enviarVia(preencherCh);
                extraCh++;
                y++;
				
            }
        }
		
        if (bandeiraHash && (paramaiusculo(especificador) == 'X'))
        {
		
            enviarVia('0');
            enviarVia('x');
            extraCh += 2;
			
        }
		
        x--;
		
        while (x >= 0)
        {
		
            enviarVia(trabalho[x]);
            extraCh++;
            x--;
			
        }
		
        if (bandeiraMenos)
        {
            while (y < tamanho)
            {
			
                enviarVia(preencherCh);
                extraCh++;
                y++;
				
            }
        }
    }
	
    else if (especificador == 's')
    {
        svalor = va_arg(*arg, char *);
        preencherCh = ' ';
        if (precisao > 1)
        {
            char *p;
            
            p = memchr(svalor, '\0', precisao);
            if (p != NULL)
            {
			
                comprimento = (int)(p - svalor);
				
            }
			
            else
            {
			
                comprimento = precisao;
				
            }
        }
		
        else
        {
		
            comprimento = tamanhostring(svalor);
			
        }
		
        if (!bandeiraMenos)
        {
            if (comprimento < tamanho)
            {
                extraCh += (tamanho - comprimento);
				
                for (x = 0; x < (tamanho - comprimento); x++)
                {
				
                    enviarVia(preencherCh);
					
                }
            }
        }
		
        for (x = 0; x < comprimento; x++)
        {
		
            enviarVia(svalor[x]);
			
        }
		
        extraCh += comprimento;
		
        if (bandeiraMenos)
        {
            if (comprimento < tamanho)
            {
			
                extraCh += (tamanho - comprimento);
				
                for (x = 0; x < (tamanho - comprimento); x++)
                {
				
                    enviarVia(preencherCh);
					
                }
            }
        }
    }
	
    *formt = formato;
    return (extraCh);
}

/*****************************************************************************************/

int vsprintf(char *s, const char *formato, lista_va arg)
{
    int ret;
    
    ret = vvprintf(formato, arg, NULL, s);
    return (ret);
}
