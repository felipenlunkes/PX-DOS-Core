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

/*********************************************************************/
/*                                                                   */
/*  Fun��es em baixo n�vel do sistema                                */
/*                                                                   */
/*********************************************************************/

#include <dos.h>

static void int86n(unsigned int numint);
static void int86i(unsigned int numint, union REGS *registradoresentrada);


int PXDOS_LerSetornoDisco(void         *buffer,
                      unsigned long setores,
                      unsigned long drive,
                      unsigned long trilha,
                      unsigned long cabeca,
                      unsigned long setor)
{

    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x02;
    registradoresentrada.h.al = (unsigned char)setores;
    registradoresentrada.h.ch = (unsigned char)(trilha & 0xff);
    registradoresentrada.h.cl = (unsigned char)setor;
    registradoresentrada.h.cl |= ((trilha & 0x0300U) >> 2);
    registradoresentrada.h.dh = (unsigned char)cabeca;
    registradoresentrada.h.dl = (unsigned char)drive;
    sregistradores.es = FP_SEGMENTO(buffer);
    registradoresentrada.x.bx = FP_OFFSET(buffer);
	
    int86x(0x13, &registradoresentrada, &registradoressaida, &sregistradores);
	
    return (registradoressaida.x.cflag);
	
}

/*****************************************************************************************/

int PXDOS_ParametrosdoDisco(unsigned long drive,
                  unsigned long *trilhas,
                  unsigned long *setores,
                  unsigned long *cabecas,
                  unsigned long *atacado,
                  unsigned char **tabelaparametros,
                  unsigned long *tipodedrive)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x08;
    registradoresentrada.h.dl = (unsigned char)drive;
    
    int86x(0x13, &registradoresentrada, &registradoressaida, &sregistradores);
    *trilhas = ((registradoressaida.h.cl & 0xC0U) << 2) | registradoressaida.h.ch + 1;
    *setores = (registradoressaida.h.cl & 0x3F) + 1;
    *cabecas = registradoressaida.h.dh + 1;
    *atacado = registradoressaida.h.dl;
    *tabelaparametros = MK_FP(sregistradores.es, registradoressaida.x.di);
    *tipodedrive = registradoressaida.h.bl;
	
    if (registradoressaida.x.cflag == 0)
    {
	
        return (0);
		
    }
	
    else
    {
	
        return (registradoressaida.h.ah);
		
    }
}

/*****************************************************************************************/

int PXDOS_ResetarDisco(unsigned long drive)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;

    registradoresentrada.h.ah = 0x00;
    registradoresentrada.h.dl = (unsigned char)drive;    
	
    int86(0x13, &registradoresentrada, &registradoressaida);
	
    return (registradoressaida.x.cflag);
	
}

/*****************************************************************************************/

int PXDOS_StatusdoDisco(unsigned long drive, unsigned long *status)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x01;
    registradoresentrada.h.dl = drive;    
    int86(0x13, &registradoresentrada, &registradoressaida);
    *status = registradoressaida.h.ah;
    return (0);
	
}

/*****************************************************************************************/

static void int86n(unsigned int numint)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    int86(numint, &registradoresentrada, &registradoressaida);
	
    return;
	
}    

/*****************************************************************************************/

static void int86i(unsigned int numint, union REGS *registradoresentrada)
{
    union REGS registradoressaida;
    
    int86(numint, registradoresentrada, &registradoressaida);
	
    return;
	
}    

/*****************************************************************************************/

int PXDOS_ImprimirTela(void)
{

    int86n(0x05);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_AlterarModoVideo - BIOS Int 10h Fun��o 00h */

int PXDOS_AlterarModoVideo(unsigned long modo)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x00;
    registradoresentrada.h.al = (unsigned char)modo;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_AlterarTipoCursor - BIOS Int 10h Fun��o 01h */

int PXDOS_AlterarTipoCursor(int topo, int fundo)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x01;
    registradoresentrada.h.ch = topo;
    registradoresentrada.h.cl = fundo;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_DefinirPosicaoCursor - BIOS Int 10h Fun��o 02h */

int PXDOS_DefinirPosicaoCursor(int pagina, int linha, int coluna)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x02;
    registradoresentrada.h.bh = pagina;
    registradoresentrada.h.dh = linha;
    registradoresentrada.h.dl = coluna;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_LerPosicaoCursor - BIOS Int 10h Fun��o 03h */

int PXDOS_LerPosicaoCursor(int pagina, 
                          int *InicioCursor,
                          int *FimCursor,
                          int *linha,
                          int *coluna)
{
    union REGS registradores;
    
    registradores.h.ah = 0x03;
    registradores.h.bh = pagina;
    int86(0x10, &registradores, &registradores);
    *InicioCursor = registradores.h.ch;
    *FimCursor = registradores.h.cl;
    *linha = registradores.h.dh;
    *coluna = registradores.h.dl;
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_LerCanetaDeLuz - BIOS Int 10h Fun��o 04h */

int PXDOS_LerCanetaDeLuz(int *gatilho,
                    unsigned long *pcolunas,
                    unsigned long *plinha1,
                    unsigned long *plinha2,
                    int *clinha,
                    int *ccoluna)
{
    union REGS registradores;
    
    registradores.h.ah = 0x04;
    int86(0x10, &registradores, &registradores);
    *gatilho = registradores.h.ah;
    *pcolunas = registradores.x.bx;
    *plinha1 = registradores.h.ch;
    *plinha2 = registradores.x.cx;
    *clinha = registradores.h.dh;
    *ccoluna = registradores.h.dl;
	
    return (0);
	
}                    

/*****************************************************************************************/

/* PXDOS_DefinirPaginaDisplayAtiva - BIOS Int 10h Fun��o 05h */

int PXDOS_DefinirPaginaDisplayAtiva(int pagina)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x05;
    registradoresentrada.h.al = pagina;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_RolarJanelaCima - BIOS Int 10h Fun��o 06h */

int PXDOS_RolarJanelaCima(int numeroLinhas,
                      int atributo,
                      int inicioLinha,
                      int inicioColuna,
                      int fimLinha,
                      int fimColuna)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x06;
    registradoresentrada.h.al = numeroLinhas;
    registradoresentrada.h.bh = atributo;
    registradoresentrada.h.ch = inicioLinha;
    registradoresentrada.h.cl = inicioColuna;
    registradoresentrada.h.dh = fimLinha;
    registradoresentrada.h.dl = fimColuna;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_RolarJanelaBaixo - BIOS Int 10h Fun��o 07h */

int PXDOS_RolarJanelaBaixo(int numeroLinhas,
                        int atributo,
                        int inicioLinha,
                        int inicioColuna,
                        int fimLinha,
                        int fimColuna)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x07;
    registradoresentrada.h.al = numeroLinhas;
    registradoresentrada.h.bh = atributo;
    registradoresentrada.h.ch = inicioLinha;
    registradoresentrada.h.cl = inicioColuna;
    registradoresentrada.h.dh = fimLinha;
    registradoresentrada.h.dl = fimColuna;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_LerAtributoChar - BIOS Int 10h Fun��o 08h */

int PXDOS_LerAtributoChar(int pagina, int *ch, int *atributo)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x08;
    registradoresentrada.h.bh = pagina;
    int86(0x10, &registradoresentrada, &registradoressaida);
    *atributo = registradoressaida.h.ah;
    *ch = registradoressaida.h.al;
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_EscreverAtributoChar - BIOS Int 10h Fun��o 09h */

int PXDOS_EscreverAtributoChar(int pagina, int ch, int atributo, unsigned long num)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x09;
    registradoresentrada.h.al = ch;
    registradoresentrada.h.bh = pagina;
    registradoresentrada.h.bl = atributo;
    registradoresentrada.x.cx = (unsigned int)num;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_ImprimirCursorChar - BIOS Int 10h Fun��o 0Ah */

int PXDOS_ImprimirCursorChar(int pagina, int ch, int coluna, unsigned long num)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x0a;
    registradoresentrada.h.al = ch;
    registradoresentrada.h.bh = pagina;
    registradoresentrada.h.bl = coluna;
    registradoresentrada.x.cx = (unsigned int)num;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_DefinirPaletadeCores - BIOS Int 10h Fun��o 0Bh */

int PXDOS_DefinirPaletadeCores(int id, int val)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x0b;
    registradoresentrada.h.bh = id;
    registradoresentrada.h.bl = val;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_ImprimirPixel - BIOS Int 10h Fun��o 0Ch */

int PXDOS_ImprimirPixel(int pagina, 
                          int cor, 
                          unsigned long linha,
                          unsigned long coluna)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x0c;
    registradoresentrada.h.al = cor;
    registradoresentrada.h.bh = pagina;
    registradoresentrada.x.cx = (unsigned int)coluna;
    registradoresentrada.x.dx = (unsigned int)linha;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_LerPixel - BIOS Int 10h Fun��o 0Dh */

int PXDOS_LerPixel(int pagina, 
                         unsigned long linha,
                         unsigned long coluna,
                         int *cor)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x0d;
    registradoresentrada.h.bh = pagina;
    registradoresentrada.x.cx = (unsigned int)coluna;
    registradoresentrada.x.dx = (unsigned int)linha;
	
    int86(0x10, &registradoresentrada, &registradoressaida);
	
    *cor = registradoressaida.h.al;
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_ImprimirTexto - BIOS Int 10h Fun��o 0Eh */

int PXDOS_ImprimirTexto(int pagina, int ch, int cor)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x0e;
    registradoresentrada.h.al = ch;
    registradoresentrada.h.bh = pagina;
    registradoresentrada.h.bl = cor;
	
    int86i(0x10, &registradoresentrada);
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_ObterModoVideo - BIOS Int 10h Fun��o 0Fh */

int PXDOS_ObterModoVideo(int *colunas, int *modo, int *pagina)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x0f;
	
    int86(0x10, &registradoresentrada, &registradoressaida);
	
    *colunas = registradoressaida.h.ah;
    *modo = registradoressaida.h.al;
    *pagina = registradoressaida.h.bh;
	
    return (0);
}
