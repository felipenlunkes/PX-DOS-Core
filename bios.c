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
/*********************************************************************/
/*                                                                   */
/*  PXDOS - Funções do BIOS em baixo nível                           */
/*                                                                   */
/*********************************************************************/

#include "bios.h"
#include "suporte.h"

static void int86n(unsigned int numint);
static void int86i(unsigned int numint, union REGS *registradoresentrada);

/* PXDOS_ImprimirTela - BIOS Int 05h */

int PXDOS_ImprimirTela(void)
{
    int86n(0x05);
    return (0);
}

/*****************************************************************************************/

/* PXDOS_AlterarModoVideo - BIOS Int 10h Função 00h */
/* Modo 03h = Modo texto */
/*      13h = 320*200 Modo gráfico */

int PXDOS_AlterarModoVideo(unsigned int modo)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x00;
    registradoresentrada.h.al = (unsigned char)modo;
    int86i(0x10, &registradoresentrada);
    return (0);
}

/*****************************************************************************************/

/* PXDOS_AlterarTipoCursor - BIOS Int 10h Função 01h */

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

/* PXDOS_DefinirPosicaoCursor - BIOS Int 10h Função 02h */

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

/* PXDOSReadCursorPositon - BIOS Int 10h Função 03h */

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

/* PXDOS_LerCanetaDeLuz - BIOS Int 10h Função 04h */

int PXDOS_LerCanetaDeLuz(int *gatilho,
                    unsigned int *pcolunas,
                    unsigned int *plinha1,
                    unsigned int *plinha2,
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

/* PXDOS_DefinirPaginaDisplayAtiva - BIOS Int 10h Função 05h */

int PXDOS_DefinirPaginaDisplayAtiva(int pagina)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x05;
    registradoresentrada.h.al = pagina;
    int86i(0x10, &registradoresentrada);
    return (0);
}

/*****************************************************************************************/

/* PXDOS_RolarJanelaCima - BIOS Int 10h Função 06h */

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

/* PXDOS_RolarJanelaBaixo - BIOS Int 10h Função 07h */

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

/* PXDOS_LerAtributoChar - BIOS Int 10h Função 08h */

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

/* PXDOS_EscreverAtributoChar - BIOS Int 10h Função 09h */

int PXDOS_EscreverAtributoChar(int pagina, int ch, int atributo, unsigned int num)
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

/* PXDOS_ImprimirCursorChar - BIOS Int 10h Função 0Ah */

int PXDOS_ImprimirCursorChar(int pagina, int ch, int coluna, unsigned int num)
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

/* PXDOS_DefinirPaletadeCores - BIOS Int 10h Função 0Bh */

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

/* PXDOS_ImprimirPixel - BIOS Int 10h Função 0Ch */

int PXDOS_ImprimirPixel(int pagina, 
                          int cor, 
                          unsigned int linha,
                          unsigned int coluna)
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

/* PXDOS_LerPixel - BIOS Int 10h Função 0Dh */

int PXDOS_LerPixel(int pagina, 
                         unsigned int linha,
                         unsigned int coluna,
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

/* PXDOS_ImprimirTexto - BIOS Int 10h Função 0Eh */

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

/* PXDOS_ObterModoVideo - BIOS Int 10h Função 0Fh */

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

/*****************************************************************************************/

int PXDOS_ResetarDisco(unsigned int drive)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;

    registradoresentrada.h.ah = 0x00;
    registradoresentrada.h.dl = (unsigned char)drive;    
    int86(0x13, &registradoresentrada, &registradoressaida);
    return (registradoressaida.x.cflag);
}

int PXDOS_StatusdoDisco(unsigned int drive, unsigned int *status)
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

int PXDOS_LerSetornoDisco(void         *buffer,
                      unsigned int setores,
                      unsigned int drive,
                      unsigned int trilha,
                      unsigned int cabeca,
                      unsigned int setor)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x02;
    registradoresentrada.h.al = (unsigned char)setores;
    registradoresentrada.h.ch = (unsigned char)(trilha & 0xff);
    registradoresentrada.h.cl = (unsigned char)setor;
    registradoresentrada.h.cl |= (((unsigned int)trilha & 0x0300U) >> 2);
    registradoresentrada.h.dh = (unsigned char)cabeca;
    registradoresentrada.h.dl = (unsigned char)drive;
    sregistradores.es = FP_SEGMENTO(buffer);
    registradoresentrada.x.bx = FP_OFFSET(buffer);

    int86x(0x13, &registradoresentrada, &registradoressaida, &sregistradores);
    return (registradoressaida.x.cflag);
}

/*****************************************************************************************/

int PXDOS_EscreverSetornoDisco(void         *buffer,
                       unsigned int setores,
                       unsigned int drive,
                       unsigned int trilha,
                       unsigned int cabeca,
                       unsigned int setor)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x03;
    registradoresentrada.h.al = (unsigned char)setores;
    registradoresentrada.h.ch = (unsigned char)(trilha & 0xff);
    registradoresentrada.h.cl = (unsigned char)setor;
    registradoresentrada.h.cl |= (((unsigned int)trilha & 0x0300U) >> 2);
    registradoresentrada.h.dh = (unsigned char)cabeca;
    registradoresentrada.h.dl = (unsigned char)drive;
    sregistradores.es = FP_SEGMENTO(buffer);
    registradoresentrada.x.bx = FP_OFFSET(buffer);
    int86x(0x13, &registradoresentrada, &registradoressaida, &sregistradores);
    return (registradoressaida.x.cflag);
}

/*****************************************************************************************/

int PXDOS_ParametrosdoDisco(unsigned int drive,
                  unsigned int *trilhas,
                  unsigned int *setores,
                  unsigned int *cabecas,
                  unsigned int *atacado,
                  unsigned char **tabelaparametros,
                  unsigned int *tipodedrive)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x08;
    registradoresentrada.h.dl = (unsigned char)drive;
    
    int86x(0x13, &registradoresentrada, &registradoressaida, &sregistradores);
    *trilhas = (((registradoressaida.h.cl & 0xC0U) << 2) | registradoressaida.h.ch) + 1;
    *setores = (registradoressaida.h.cl & 0x3F);
    *cabecas = registradoressaida.h.dh + 1;
    *atacado = registradoressaida.h.dl;
    *tabelaparametros = (unsigned char *)MK_FP(sregistradores.es, registradoressaida.x.di);
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

int PXDOS_FixarStatusdoDisco(unsigned int drive)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x10;
    registradoresentrada.h.dl = (unsigned char)drive;
    int86(0x13, &registradoresentrada, &registradoressaida);
    if (registradoressaida.x.cflag)
    {
	
       
	   return (registradoressaida.x.ax);
    
	}
	
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_LerSetoremLBA - Lê o disco usando o LBA e a Int 13h Função 42h */
/* return 0 = sucesso */

int PXDOS_LerSetoremLBA(void         *buffer,
                      unsigned int setores,
                      unsigned int drive,
                      unsigned long setor,
                      unsigned long setorSuperior)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    unsigned char lpacote[16];
    unsigned char *pacote;
    
    registradoresentrada.h.ah = 0x42;
    registradoresentrada.h.dl = (unsigned char)drive;
    pacote = lpacote;

    pacote[0] = sizeof lpacote;
    pacote[1] = 0;
    pacote[2] = setores & 0xff;
    pacote[3] = (setores >> 8) & 0xff;
    pacote[4] = FP_OFFSET(buffer) & 0xff;
    pacote[5] = (FP_OFFSET(buffer) >> 8) & 0xff;
    pacote[6] = FP_SEGMENTO(buffer) & 0xff;
    pacote[7] = (FP_SEGMENTO(buffer) >> 8) & 0xff;
    pacote[8] = setor & 0xff;
    pacote[9] = (setor >> 8) & 0xff;
    pacote[10] = (setor >> 16) & 0xff;
    pacote[11] = (setor >> 24) & 0xff;
    pacote[12] = setorSuperior & 0xff;
    pacote[13] = (setorSuperior >> 8) & 0xff;
    pacote[14] = (setorSuperior >> 16) & 0xff;
    pacote[15] = (setorSuperior >> 24) & 0xff;
    sregistradores.ds = FP_SEGMENTO(pacote);
    registradoresentrada.x.si = FP_OFFSET(pacote);   
    
    int86x(0x13, &registradoresentrada, &registradoressaida, &sregistradores);
    return (registradoressaida.h.ah);
	
}

/*****************************************************************************************/

int PXDOS_EscreverSetorLBA(void         *buffer,
                      unsigned int setores,
                      unsigned int drive,
                      unsigned long setor,
                      unsigned long setorOculto)
{
    union REGS regsentrada;
    union REGS regssaida;
    struct SREGS sregs;
    unsigned char lpacote[16];
    unsigned char *pacote;

    regsentrada.h.ah = 0x43;
    regsentrada.h.dl = (unsigned char)drive;
    pacote = lpacote;
    pacote[0] = sizeof lpacote;
    pacote[1] = 0;
    pacote[2] = setores & 0xff;
    pacote[3] = (setores >> 8) & 0xff;
    pacote[4] = FP_OFFSET(buffer) & 0xff;
    pacote[5] = (FP_OFFSET(buffer) >> 8) & 0xff;
    pacote[6] = FP_SEGMENTO(buffer) & 0xff;
    pacote[7] = (FP_SEGMENTO(buffer) >> 8) & 0xff;
    pacote[8] = setor & 0xff;
    pacote[9] = (setor >> 8) & 0xff;
    pacote[10] = (setor >> 16) & 0xff;
    pacote[11] = (setor >> 24) & 0xff;
    pacote[12] = setorOculto & 0xff;
    pacote[13] = (setorOculto >> 8) & 0xff;
    pacote[14] = (setorOculto >> 16) & 0xff;
    pacote[15] = (setorOculto >> 24) & 0xff;
    sregs.ds = FP_SEGMENTO(pacote);
    regsentrada.x.si = FP_OFFSET(pacote);


    int86x(0x13, &regsentrada, &regssaida, &sregs);
    return (regssaida.h.ah);
}


/*****************************************************************************************/

long PXDOS_TamanhoMemoriaExtendida(void)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x88;
    int86(0x15, &registradoresentrada, &registradoressaida);
    
    
    if (registradoressaida.x.cflag
        && ((registradoressaida.h.ah == 0x80) || (registradoressaida.h.ah == 0x86)))
    {
	
        return (0);
		
    }
	
    else
    {
	
        
		return ((long)registradoressaida.x.ax << 10);
   
   }
   
}

/*****************************************************************************************/

/* PXDOS_LerCaracteredoTeclado - BIOS Int 16h Função 00h */

int PXDOS_LerCaracteredoTeclado(int *EscaniarCodigo, int *ascii)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x00;
    int86(0x16, &registradoresentrada, &registradoressaida);
    *EscaniarCodigo = registradoressaida.h.ah;
    *ascii = registradoressaida.h.al;
    return (0);
	
}

/*****************************************************************************************/

/* PXDOS_IniciarImpressora - BIOS Int 17h Função 01 */

int PXDOS_IniciarImpressora(unsigned int impressora)
{

    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x01;
	registradoresentrada.x.dx = impressora;
    int86(0x17, &registradoresentrada, &registradoressaida);
    return (0);

}
	
/*****************************************************************************************/

/* PXDOS_IniciarSerial - BIOS Int 14h Função 00 */

int PXDOS_IniciarSerial(unsigned int porta)
{

    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x00;
	registradoresentrada.h.al = 0xe3;
	registradoresentrada.x.dx = porta;
    int86(0x14, &registradoresentrada, &registradoressaida);
    return (0);

}

/*****************************************************************************************/

/* PXDOS_EnviarSerial - BIOS Int 14h Função 01 */

int PXDOS_EnviarSerial(unsigned int porta, int caractere)
{

    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x01;
	registradoresentrada.h.al = (char)caractere;
	registradoresentrada.x.dx = (char)porta;
    int86(0x14, &registradoresentrada, &registradoressaida);
    return (0);

}


/*****************************************************************************************/

/* PXDOS_NovoBoot - BIOS Int 19h */

void PXDOS_NovoBoot(void)
{

    int86n(0x19);
    return;
	
}

/*****************************************************************************************/

/* int86n - Interrupção sem parâmetros */

static void int86n(unsigned int numint)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    int86(numint, &registradoresentrada, &registradoressaida);
    return;
	
}    

/*****************************************************************************************/

/* int86i - Interrupção com parâmetros e sem retorno */

static void int86i(unsigned int numint, union REGS *registradoresentrada)
{
    union REGS registradoressaida;
    
    int86(numint, registradoresentrada, &registradoressaida);
    return;
}    

