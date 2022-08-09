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


/* EnderecoParaAbsoluto - Converte o endere�o para 16:16*/

#define EnderecoParaAbsoluto(x) ((((unsigned long)(x) >> 16) << 4) \
                   + ((unsigned long)(x) & 0xffffU))
				   
/* AbsolutoParaEndereco - Converte endere�o flat para 16:16 */

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


