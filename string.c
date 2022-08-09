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

#include "string.h"

#ifdef memmove
#undef memmove
#endif

void *memmove(void *s1, const void *s2, tamanho_t n)
{
    char *p = s1;
    const char *cs2 = s2;
    tamanho_t x;
    
    if (p <= cs2)
    {
	
        for (x=0; x < n; x++)
        {
		
            *p = *cs2;
            p++;
            cs2++;
			
        }
    }
	
    else
    {
        
		if (n != 0)
        {
            for (x=n-1; x > 0; x--)
            {
			
                *(p+x) = *(cs2+x);
				
            }
			
        }
		
        *(p+x) = *(cs2+x);
    }
	
    return (s1);
}

#ifdef strcpy
#undef strcpy
#endif

/*****************************************************************************************/

char *strcpy(char *s1, const char *s2)
{
    char *p = s1;
    
    while ((*p++ = *s2++) != '\0') ;
    return (s1);
}

#ifdef strncpy
#undef strncpy
#endif

/*****************************************************************************************/

char *strncpy(char *s1, const char *s2, tamanho_t n)
{
    char *p = s1;
    tamanho_t x;
    
    for (x=0; x < n; x++)
    {
	
        *p = *s2;
        if (*s2 == '\0') break;
        p++;
        s2++;
		
    }
	
    for (; x < n; x++)
    {
	
        *p++ = '\0';
		
    }
    return (s1);
}

#ifdef strcat
#undef strcat
#endif

/*****************************************************************************************/

char *strcat(char *s1, const char *s2)
{
    char *p = s1;
    
    while (*p != '\0') p++;
    while ((*p = *s2) != '\0')
    {
	
        p++;
        s2++;
		
    }
	
    return (s1);
}

#ifdef strncat
#undef strncat
#endif

/*****************************************************************************************/

char *strncat(char *s1, const char *s2, tamanho_t n)
{
    char *p = s1;
    tamanho_t x = 0;
    
    while (*p != '\0') p++;
	
    while ((*s2 != '\0') && (x < n))
    {
        *p = *s2;
        p++;
        s2++;
        x++;
    }
	
    *p = '\0';
	
    return (s1);
}

#ifdef memcmp
#undef memcmp
#endif

/*****************************************************************************************/

int memcmp(const void *s1, const void *s2, tamanho_t n)
{
    const unsigned char *p1;
    const unsigned char *p2;
    tamanho_t x = 0;
    
    p1 = (const unsigned char *)s1;
    p2 = (const unsigned char *)s2;
    while (x < n)
    {
	
        if (p1[x] < p2[x]) return (-1);
		
        else if (p1[x] > p2[x]) return (1);
		
        x++;
    }
	
    return (0);
}
  
/*****************************************************************************************/
  
#ifdef strcmp
#undef strcmp
#endif

int strcmp(const char *s1, const char *s2)
{
    const unsigned char *p1;
    const unsigned char *p2;
    
    p1 = (const unsigned char *)s1;
    p2 = (const unsigned char *)s2;
    while (*p1 != '\0')
    {
	
        if (*p1 < *p2) return (-1);
		
        else if (*p1 > *p2) return (1);
		
        p1++;
		
        p2++;
    }
	
    if (*p2 == '\0') return (0);
	
    else return (-1);
}

/*****************************************************************************************/

#ifdef strcoll
#undef strcoll
#endif

/*****************************************************************************************/

int strcoll(const char *s1, const char *s2)
{

    return (strcmp(s1, s2));
	
}

#ifdef strncmp
#undef strncmp
#endif

/*****************************************************************************************/

int strncmp(const char *s1, const char *s2, tamanho_t n)
{
    const unsigned char *p1;
    const unsigned char *p2;
    tamanho_t x = 0;
    
    p1 = (const unsigned char *)s1;
    p2 = (const unsigned char *)s2;
    while (x < n)
    {
	
        if (p1[x] < p2[x]) return (-1);
		
        else if (p1[x] > p2[x]) return (1);
		
        else if (p1[x] == '\0') return (0);
		
        x++;
		
    }
    return (0);
}

/*****************************************************************************************/

#ifdef strxfrm
#undef strxfrm
#endif

tamanho_t strxfrm(char *s1, const char *s2, tamanho_t n)
{
    tamanho_t antigolen;
    
    antigolen = tamanhostring(s2);
    if (antigolen < n)
    {
	
        memcpy(s1, s2, antigolen);
		
        s1[antigolen] = '\0';
		
    }
	
    return (antigolen);
}

/*****************************************************************************************/

#ifdef memchr
#undef memchr
#endif

void *memchr(const void *s, int c, tamanho_t n)
{
    const unsigned char *p;
    tamanho_t x = 0;
    
    p = (const unsigned char *)s;
    while (x < n)
    {
	
        if (*p == (unsigned char)c) return ((void *)p);
		
        p++;
		
        x++;
		
    }
	
    return (NULL);
}

/*****************************************************************************************/

#ifdef strchr
#undef strchr
#endif

char *strchr(const char *s, int c)
{
    while (*s != '\0')
    {
	
        if (*s == (char)c) return ((char *)s);
		
        s++;
    }
	
    if (c == '\0') return ((char *)s);
	
    return (NULL);
}

/*****************************************************************************************/

#ifdef strcspn
#undef strcspn
#endif

tamanho_t strcspn(const char *s1, const char *s2)
{
    const char *p1;
    const char *p2;
    
    p1 = s1;
    while (*p1 != '\0')
    {
	
        p2 = s2;
		
        while (*p2 != '\0')
        {
		
            if (*p1 == *p2) return ((tamanho_t)(p1 - s1));
            p2++;
			
        }
		
        p1++;
    }
	
    return ((tamanho_t)(p1 - s1));
}

/*****************************************************************************************/

#ifdef strpbrk
#undef strpbrk
#endif

char *strpbrk(const char *s1, const char *s2)
{
    const char *p1;
    const char *p2;
    
    p1 = s1;
    while (*p1 != '\0')
    {
	
        p2 = s2;
		
        while (*p2 != '\0')
        {
		
            if (*p1 == *p2) return ((char *)p1);
            p2++;
			
        }
		
        p1++;
		
    }
	
    return (NULL);
}

/*****************************************************************************************/

#ifdef strrchr
#undef strrchr
#endif

char *strrchr(const char *s, int c)
{
    const char *p;
    
    p = s + tamanhostring(s);
    while (p >= s)
    {
	
        if (*p == (char)c) return ((char *)p);
        p--;
		
    }
	
    return (NULL);
}

/*****************************************************************************************/

#ifdef strspn
#undef strspn
#endif

tamanho_t strspn(const char *s1, const char *s2)
{
    const char *p1;
    const char *p2;
    
    p1 = s1;
    while (*p1 != '\0')
    {
        p2 = s2;
		
        while (*p2 != '\0')
        {
		
            if (*p1 == *p2) break;
			
            p2++;
			
        }
		
        if (*p2 == '\0') return ((tamanho_t)(p1 - s1));
		
        p1++;
		
    }
    return ((tamanho_t)(p1 - s1));
}

/*****************************************************************************************/

#ifdef strstr
#undef strstr
#endif

char *strstr(const char *s1, const char *s2)
{
    const char *p = s1, *p1, *p2 = s2;

    while (*p)
    {
	
        if (*p == *s2)
        {
		
            p1 = p;
            p2 = s2;
			
            while ((*p2 != '\0') && (*p1++ == *p2++)) ;
            
			if (*p2 == '\0')
            {
			
                return (char *)p;
				
            }
        }
		
        p++;
		
    }
    return NULL;
}

/*****************************************************************************************/

#ifdef strtok
#undef strtok
#endif

char *strtok(char *s1, const char *s2)
{
    static char *antigo = NULL;
    char *p;
    tamanho_t tamanho;
    tamanho_t sobrando;
    
    if (s1 != NULL) antigo = s1;
	
    if (antigo == NULL) return (NULL);
	
    p = antigo;
	
    tamanho = strspn(p, s2);
	
    sobrando = tamanhostring(p);
	
    if (sobrando <= tamanho) { antigo = NULL; return (NULL); }
	
    p += tamanho;
	
    tamanho = strcspn(p, s2);
	
    sobrando = tamanhostring(p);
	
    if (sobrando <= tamanho) { antigo = NULL; return (p); }
	
    *(p + tamanho) = '\0';
	
    antigo = p + tamanho + 1;
	
    return (p);
}

/*****************************************************************************************/

#ifdef memset
#undef memset
#endif

void *memset(void *s, int c, tamanho_t n)
{
    tamanho_t x = 0;
    
    for (x = 0; x < n; x++)
    {
	
        *((char *)s + x) = (unsigned char)c;
		
    }
	
    return (s);
}

/*****************************************************************************************/

#ifdef strerror
#undef strerror
#endif

char *strerror(int NumeroDoErro)
{

    if (NumeroDoErro == 0) return ("Nenhum erro ocorreu.\n");
	
    else return ("Um erro ocorreu.\n");
}
     
/*****************************************************************************************/
	 
#ifdef tamanhostring
#undef tamanhostring
#endif

tamanho_t tamanhostring(const char *s)
{
    const char *p;
    
    p = s;
    while (*p != '\0') p++;
	
    return ((tamanho_t)(p - s));
}

/*****************************************************************************************/

#ifndef USE_ASSEMBLER
#ifdef memcpy
#undef memcpy
#endif


void *memcpy(void *s1, const void *s2, tamanho_t n)
{
    register const unsigned char *f = s2;
    register const unsigned char *fe;
    register unsigned char *t = s1;
    
    fe = f + n;
	
    while (f != fe)
    {
	
        *t++ = *f++;
		
    }
	
    return (s1);
}

#endif
