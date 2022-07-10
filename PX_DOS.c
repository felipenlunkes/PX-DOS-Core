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
/*                   Implementações de PX_DOS.H                      */
/*********************************************************************/

#include <stddef.h>

#include "PX_DOS.h"
#include "suporte.h"

static void int86n(unsigned int numint);
static void int86i(unsigned int numint, union REGS *registradoresentrada);

/* int 20h */

void PX_DOSTermSemRC(void)
{

    int86n(0x20);
    return;

	}

/*****************************************************************************************/


/* int 21h */

void PX_DOSMostrarSaida(int ch)
{

    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x02;
    registradoresentrada.h.dl = ch;
    int86i(0x21, &registradoresentrada);
    return;
	
}

/*****************************************************************************************/

int PX_DOSObterCharSemEcho(void)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x08;
    int86(0x21, &registradoresentrada, &registradoressaida);

    return (registradoressaida.h.al);
	
}

/*****************************************************************************************/

void PX_DOSImprimirString(const char *buf)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;

    registradoresentrada.h.ah = 0x09;
      
    registradoresentrada.x.dx = FP_OFFSET(buf);
    sregistradores.ds = FP_SEGMENTO(buf);
    
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    return;
	
}

/*****************************************************************************************/

int PX_DOSSelecionarDisco(int drive)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x0e;
    registradoresentrada.h.dl = drive;
    int86(0x21, &registradoresentrada, &registradoressaida);
    return (registradoressaida.h.al);
	
}

/*****************************************************************************************/

int PX_DOSObterDrivePadrao(void)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x19;
    int86(0x21, &registradoresentrada, &registradoressaida);
    return (registradoressaida.h.al);
	
}

/*****************************************************************************************/

void PX_DOSDefinirATD(void *ATD)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;

    registradoresentrada.h.ah = 0x1a;
        
    sregistradores.ds = FP_SEGMENTO(ATD);
    registradoresentrada.x.dx = FP_OFFSET(ATD);
   
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    return;
	
}

/*****************************************************************************************/

void PX_DOSDefinirVetordeInterrupcoes(int numeroint, void *manipulador_int)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x25;
    registradoresentrada.h.al = (char)numeroint;

    sregistradores.ds = FP_SEGMENTO(manipulador_int);
    registradoresentrada.x.dx = FP_OFFSET(manipulador_int);

    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    return;
	
}

/*****************************************************************************************/

void PX_DOSObterDatadoSistema(int *ano, int *mes, int *dia, int *complemento)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x2a;
    int86(0x21, &registradoresentrada, &registradoressaida);
    *ano = registradoressaida.x.cx;
    *mes = registradoressaida.h.dh;
    *dia = registradoressaida.h.dl;
    *complemento = registradoressaida.h.al;
    return;
	
}

/*****************************************************************************************/

void PX_DOSObterHoradoSistema(int *hora, int *minuto, int *seg, int *milisegundos)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x2c;
    int86(0x21, &registradoresentrada, &registradoressaida);
    *hora = registradoressaida.h.ch;
    *minuto = registradoressaida.h.cl;
    *seg = registradoressaida.h.dh;
    *milisegundos = registradoressaida.h.dl;
    return;
	
}

/*****************************************************************************************/

void *PX_DOSObterATD(void)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    void *ATD;

    registradoresentrada.h.ah = 0x2f;
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
       
    ATD = MK_FP(sregistradores.es, registradoressaida.x.bx);
    
    return (ATD);
	
}

/*****************************************************************************************/

unsigned int PX_DOSObterVersaodoDOS(void)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x30;
    int86(0x21, &registradoresentrada, &registradoressaida);
    return ((registradoressaida.h.al << 8) | registradoressaida.h.ah);
	
}

/*****************************************************************************************/

void *PX_DOSObterVetordeInterrupcoes(int numeroint)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x35;
    registradoresentrada.h.al = (char)numeroint;
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
        
    return (MK_FP(sregistradores.es, registradoressaida.x.bx));

}

/*****************************************************************************************/

int PX_DOSAlterarDir(char *para)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x3b;
     
    registradoresentrada.x.dx = FP_OFFSET(para);
    sregistradores.ds = FP_SEGMENTO(para);
    
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (registradoressaida.x.cflag)
    {
	
        return (registradoressaida.x.ax);
   
   }
   
    else
    {
	
        return (0);
    
	} 
	
}

/*****************************************************************************************/

int PX_DOSAbrirArquivo(const char *buffer,
                int modo,
                int *manuseador)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x3d;
    registradoresentrada.h.al = (unsigned char)modo;

    sregistradores.ds = FP_SEGMENTO(buffer);
    registradoresentrada.x.dx = FP_OFFSET(buffer);
   
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);

    *manuseador = registradoressaida.x.ax;

    return (registradoressaida.x.cflag);
	
}

/*****************************************************************************************/

int PX_DOSFecharArquivo(int manuseador)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x3e;
      
    registradoresentrada.x.bx = manuseador;

    int86(0x21, &registradoresentrada, &registradoressaida);

    if (!registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;
		
    }
	
    return (registradoressaida.x.ax);
           
}

/*****************************************************************************************/

void PX_DOSLerArquivo(int conteudo_arquivo_a_ser_lido, void *dados, tamanho_t bytes, tamanho_t *byteslidos)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
     
    registradoresentrada.h.ah = 0x3f;
      
    registradoresentrada.x.bx = (unsigned int)conteudo_arquivo_a_ser_lido;
    registradoresentrada.x.cx = (unsigned int)bytes;
    sregistradores.ds = FP_SEGMENTO(dados);
    registradoresentrada.x.dx = FP_OFFSET(dados);
   
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (registradoressaida.x.cflag)
    {
	
        *byteslidos = 0;
		
    }
	
    else
    {
            
        *byteslidos = registradoressaida.x.ax;

    }
	
    return;
	
}

/*****************************************************************************************/

int PX_DOSEscreverArquivo(int manuseador,
                 const void *dados,
                 tamanho_t tamanho)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x40;
        
    registradoresentrada.x.bx = (unsigned int)manuseador;
    registradoresentrada.x.cx = (unsigned int)tamanho;
    sregistradores.ds = FP_SEGMENTO(dados);
    registradoresentrada.x.dx = FP_OFFSET(dados);
   
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (registradoressaida.x.cflag)
    {
	
        return (-1);
		
    }
	
    else
    {
            
        return (registradoressaida.x.ax);

    }
	
}

/*****************************************************************************************/

int PX_DOSDeletarArquivo(const char *nomearquivo)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x41;
       
    sregistradores.ds = FP_SEGMENTO(nomearquivo);
    registradoresentrada.x.dx = FP_OFFSET(nomearquivo);

    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
   
   if (!registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;
		
    }
	
    return (registradoressaida.x.ax);
	
}

/*****************************************************************************************/

long PX_DOSMoverPonteirodoArquivo(int manuseador, long deslocamento, int deonde)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x42;
    
    registradoresentrada.x.bx = manuseador;
    registradoresentrada.x.dx = FP_OFFSET(deslocamento);
    registradoresentrada.x.cx = FP_SEGMENTO(deslocamento);

    registradoresentrada.h.al = (char)deonde;
    int86(0x21, &registradoresentrada, &registradoressaida);

    if (!registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0xffff;
        registradoressaida.x.dx = 0xffff;
		
    }
	
    return (((unsigned long)registradoressaida.x.dx << 16) | registradoressaida.x.ax);
           
}

/*****************************************************************************************/

int PX_DOSObterInformacoesdoDispositivo(int manuseador, unsigned int *devinfo)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x44;
    registradoresentrada.h.al = 0x00;

    registradoresentrada.x.bx = manuseador;

    int86(0x21, &registradoresentrada, &registradoressaida);
    if (!registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;

        *devinfo = registradoressaida.x.dx;

    }
	
    return (registradoressaida.x.ax);
	
}

/*****************************************************************************************/

int PX_DOSDispositivodeBlocoRemovivel(int drive)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x44;
    registradoresentrada.h.al = 0x08;
    registradoresentrada.h.bl = (char)drive;
    int86(0x21, &registradoresentrada, &registradoressaida);

    if (registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = -registradoressaida.x.ax;
   
   }
   
    return (registradoressaida.x.ax);
           
}

/*****************************************************************************************/

int PX_DOSObterDiretorioAtual(int drive, char *dir)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x47;
    registradoresentrada.h.dl = drive;

    registradoresentrada.x.si = FP_OFFSET(dir);
    sregistradores.ds = FP_SEGMENTO(dir);

    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (registradoressaida.x.cflag)
    {

        return (registradoressaida.x.ax);
       
    }
	
    else
    {
	
        return (0);
		
    } 
	
}

/*****************************************************************************************/

void *PX_DOSAlocarMemoriaRAM(unsigned int tamanho)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x48;
       
    registradoresentrada.x.bx = tamanho;
    registradoresentrada.x.bx >>= 4;
    if ((tamanho % 16) != 0)
    {
	
        registradoresentrada.x.bx++;
		
    }

    int86(0x21, &registradoresentrada, &registradoressaida);

    if (registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;
		
    }
	
    return (MK_FP(registradoressaida.x.ax, 0));
          
}

/*****************************************************************************************/

void *PX_DOSAlocarMem_Paginas(unsigned int paginas, unsigned int *paginasmaximas)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x48;
       
    registradoresentrada.x.bx = paginas;

    int86(0x21, &registradoresentrada, &registradoressaida);

    if (registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;
		
    }
	
    if (paginasmaximas != NULL)
    {
	
        *paginasmaximas = registradoressaida.x.bx;
    
	}
	
    return (MK_FP(registradoressaida.x.ax, 0));
            
}

/*****************************************************************************************/

int PX_DOSMemoriaLivre(void *ptr)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x49;
     
    sregistradores.es = FP_SEGMENTO(ptr);

    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (!registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;
		
    }
	
    return (registradoressaida.x.ax);
	
}

/*****************************************************************************************/

int PX_DOSRealocarMemoria(void *ptr, unsigned int novaspaginas, unsigned int *maximo_de_paginas)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x4a;
       
    sregistradores.es = FP_SEGMENTO(ptr);
    registradoresentrada.x.bx = novaspaginas;

    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (!registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;

        *maximo_de_paginas = registradoressaida.x.bx;

    }
	
    return (registradoressaida.x.ax);
}

/*****************************************************************************************/

void PX_DOSExec(char *prog, void *blocodeParametros)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
     
    registradoresentrada.h.ah = 0x4b;
    registradoresentrada.h.al = 0;

    sregistradores.ds = FP_SEGMENTO(prog);
    registradoresentrada.x.dx = FP_OFFSET(prog);
    sregistradores.es = FP_SEGMENTO(blocodeParametros);
    registradoresentrada.x.bx = FP_OFFSET(blocodeParametros);
   
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    return;
}

/*****************************************************************************************/

void PX_DOSTerminar(int rc)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x4c;
    registradoresentrada.h.al = rc;
    int86i(0x21, &registradoresentrada);
    return;
}

/*****************************************************************************************/

int PX_DOSObterCodigodeRetorno(void)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x4d;
    int86(0x21, &registradoresentrada, &registradoressaida);
    return (registradoressaida.x.ax);
}

/*****************************************************************************************/

int PX_DOSEncontrarPrimeiro(char *pronto, int atributo)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x4e;
    registradoresentrada.x.cx = atributo;
        
    registradoresentrada.x.dx = FP_OFFSET(pronto);
    sregistradores.ds = FP_SEGMENTO(pronto);
    
    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (registradoressaida.x.cflag)
    {
	
        return (registradoressaida.x.ax);
    
	}
	
    else
    {
	
        return (0);
   
   }

}

/*****************************************************************************************/
 
int PX_DOSEncontrarProximo(void)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    
    registradoresentrada.h.ah = 0x4f;
    int86(0x21, &registradoresentrada, &registradoressaida);    
    if (registradoressaida.x.cflag)
    {
	
        return (registradoressaida.x.ax);
   
   }
   
    else
    {
	
        return (0);
    
	}
}

/*****************************************************************************************/

int PX_DOSRenomearArquivo(const char *antigo, const char *novo)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x56;
        
    sregistradores.ds = FP_SEGMENTO(antigo);
    registradoresentrada.x.dx = FP_OFFSET(antigo);
    sregistradores.es = FP_SEGMENTO(novo);
    registradoresentrada.x.di = FP_OFFSET(novo);

    int86x(0x21, &registradoresentrada, &registradoressaida, &sregistradores);
    if (!registradoressaida.x.cflag)
    {
	
        registradoressaida.x.ax = 0;
   
   }
   
    return (registradoressaida.x.ax);
}

/*****************************************************************************************/

void PX_DOSImprimirInteiro(int x)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0xf3;
    registradoresentrada.h.al = 0;

    registradoresentrada.x.cx = x;
       
    int86i(0x21, &registradoresentrada);
    return;
}

/*****************************************************************************************/

/* Inicia uma impressora à partir de aplicativos */

void PX_DOSIniciarImpressora(unsigned int impressora)
{

    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x14;
    registradoresentrada.x.dx = impressora;
    int86i(0x90, &registradoresentrada);
    return;

}

/*****************************************************************************************/

/* Inicia uma porta serial à partir de aplicativos */

void PX_DOSIniciarSerial(unsigned int porta)
{

    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x15;
    registradoresentrada.x.dx = porta;
    int86i(0x90, &registradoresentrada);
    return;

}

/*****************************************************************************************/

/* Envia um caractere para a porta serial à partir de aplicativos */

void PX_DOSEnviarSerial(unsigned int porta, int caractere, int metodo)
{

    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x16;
	registradoresentrada.h.al = 0x00;
    registradoresentrada.x.dx = porta;
	registradoresentrada.x.bx = caractere;
	registradoresentrada.x.cx = metodo;
    int86i(0x90, &registradoresentrada);
    return;

}
/*****************************************************************************************/

/* Reiniciar computador */

void PX_DOSReiniciar(void)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x04;
    registradoresentrada.h.al = 1;
    int86i(0x90, &registradoresentrada);
    return;
}

/*****************************************************************************************/

/* Configurar biblioteca de execução */

void PX_DOSDefinirBibliotecadeExecucao(void *PXDOSinicio, void *capi)
{
    union REGS registradoresentrada;
    
    registradoresentrada.h.ah = 0x06;
    registradoresentrada.h.al = 2;

    int86i(0x90, &registradoresentrada);
}

/*****************************************************************************************/

/* int86n - Interrupção sem registradores */

static void int86n(unsigned int numint)
{
    union REGS registradoresentrada;
    union REGS registradoressaida;

    int86(numint, &registradoresentrada, &registradoressaida);
    return;
}    

/*****************************************************************************************/

/* int86i - Interrupção com registradores de entrada apenas */

static void int86i(unsigned int numint, union REGS *registradoresentrada)
{
    union REGS registradoressaida;
    
    int86(numint, registradoresentrada, &registradoressaida);
    return;
}    
