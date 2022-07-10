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


#ifndef SUPORTE_INCLUIDO
#define SUPORTE_INCLUIDO


typedef struct {
    unsigned short ax;
    unsigned short bx;
    unsigned short cx;
    unsigned short dx;
    unsigned short si;
    unsigned short di;
    unsigned short cflag;
    unsigned short flags;
} WORDREGS;


typedef struct {
    unsigned char al;
    unsigned char ah;
    unsigned char bl;
    unsigned char bh;
    unsigned char cl;
    unsigned char ch;
    unsigned char dl;
    unsigned char dh;
} BYTEREGS;



union REGS

{
    BYTEREGS h;
    WORDREGS x;  
};


struct SREGS
{
    unsigned int es;
    unsigned int cs;
    unsigned int ss;
    unsigned int ds;
};



#define far


/* EnderecoParaAbsoluto - Converte o endereço para 16:16*/

#define EnderecoParaAbsoluto(x) ((((unsigned long)(x) >> 16) << 4) \
                   + ((unsigned long)(x) & 0xffffU))
				   
/* AbsolutoParaEndereco - Converte endereço flat para 16:16 */

#define AbsolutoParaEndereco(x) ((void far *)((((unsigned long)(x) & 0xf0000UL) << 12) \
                   | ((unsigned long)(x) & 0xffff)))

#define MK_FP(x, y) (void far *)(((unsigned long)(x) << 16) | (y))

#define FP_OFFSET(x) (unsigned int)((unsigned long)(x) & 0xffffU)

#define FP_SEGMENTO(x) (unsigned int)((unsigned long)(x) >> 16)

#define FP_NORM(x) (void *)(((((unsigned long)(x) >> 16) \
                     + (((unsigned long)(x) & 0xffffU) / 16)) << 16) \
                     | (((unsigned long)(x) & 0xffffU) % 16))
					 

int int86(int numint, union REGS *registradoresentrada, union REGS *registradoressaida);
int int86x(int numint, union REGS *registradoresentrada, union REGS *registradoressaida, struct SREGS *sregistradores);

#endif


