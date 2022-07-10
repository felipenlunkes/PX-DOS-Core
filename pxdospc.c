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
// Ponto flutuante para C



#define CTYP


unsigned long CTYP __divide(unsigned long x, unsigned long y)
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned int e;
    unsigned int f;
    unsigned int g = 0;
    unsigned int h;
    
    b = (unsigned int)(y & 0xffffU);
    a = (unsigned int)(y >> 16);
    c = -b;
    d = a;
	
    if (b > 0)
    {
	
        d++;
		
    }
	
    f = 1;
	
    while (f != 0)
    {
	
        e = (unsigned int)(x >> 16);
        h = (unsigned int)(x & 0xffffU);
        f = e / d;
        g += f;
        e = e % d;
        x = ((unsigned long)e << 16) + (unsigned long)c * f + h;
		
    }
	
    if (x >= y)
    {
	
        g++;
		
    }
	
    return ((unsigned long)g);
}

/*****************************************************************************************/

unsigned long CTYP __modulo(unsigned long x, unsigned long y)
{
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned int e;
    unsigned int f;
    unsigned int g = 0;
    unsigned int h;
    
    b = (unsigned int)(y & 0xffffU);
    a = (unsigned int)(y >> 16);
    c = -b;
    d = a;
    if (b > 0)
    {
	
        d++;
    
	}
	
    f = 1;
	
    while (f != 0)
    {
	
        e = (unsigned int)(x >> 16);
        h = (unsigned int)(x & 0xffffU);
        f = e / d;
        g += f;
        e = e % d;
        x = ((unsigned long)e << 16) + (unsigned long)c * f + h;
		
    }
	
    if (x >= y)
    {
    
    x -= y;

    }
	
    return (x);
}



