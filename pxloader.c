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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "util.px"

#include "bios.h"
#include "suporte.h"
#include "int13x.h"
#include "naousado.h"

/*********************************************************************/

typedef struct
{

    unsigned long oculto;
    unsigned long setores_por_cilindro;
    unsigned int numero_de_cabecas;
    unsigned int setores_por_trilha;
    unsigned int tamanho_setor;
    unsigned int numfats;
    unsigned int iniciofat;
    unsigned int inicioroot;
    unsigned int inicioarquivo;
    unsigned int drive;
    unsigned int entradasroot;
    unsigned int tamanhoroot;
    unsigned int tamanhofat;
	
} INFODISCO;

/*********************************************************************/

void carregarPXDOS(void);

static void carregarKernel(void);

static void analisarBpb(INFODISCO *infodisco, unsigned char *bpb);

static void lerlogico(INFODISCO *infodisco, long setor, void *buf);

int lerabs(void *buf, int setores, int drive, int trilhas, int cabeca, int set);

/*********************************************************************/

int Carregador(void)
{
    limpartela();           // Limpa a tela
	
	dumpbuf("                                                                                ",80);
	
	dumpbuf("Iniciando o PX-DOS...", 21);

    carregarKernel(); // Inicia a carga do Kernel do PX-DOS
	       
    carregarPXDOS();
	
    return (0);
	
}

/*********************************************************************/

static void carregarKernel(void)
{
    long setor = 0;
    unsigned char *p;
    INFODISCO infodisco;
    int x;

#if 0    
    
    infodisco.setores_por_cilindro = 1;
	
    infodisco.numero_de_cabecas = 1;
	
    infodisco.setores_por_trilha = 1;
	
    infodisco.inicioboot = 0;
	
    infodisco.iniciofat = 1;
	
    p = (char *)0xc000;
	
    lerlogico(&infodisco, setor, p);
	
    p += 11;
	
#endif

    p = (unsigned char *)(0x7c00 - 0x600 + 11); 
	
    analisarBpb(&infodisco, p);
	
    p = (unsigned char *)0x100;
	
    setor = infodisco.inicioarquivo;
    
    // Não se pode carregar mais de 58 setores (setores 7b00-7d00)
	
    for (x = 0; x < 58; x++)
    {
	
#if 1 

        lerlogico(&infodisco, setor + x, p);
		
#endif  
      
        p += 512;
		
    }
	
    return;
	
}

/*********************************************************************/

static void analisarBpb(INFODISCO *infodisco, unsigned char *bpb)
{

    infodisco->tamanho_setor = bpb[0] | ((unsigned int)bpb[1] << 8);
	
    infodisco->numfats = bpb[5];
	
    infodisco->tamanhofat = bpb[11] | ((unsigned int)bpb[12] << 8);
	
    infodisco->numero_de_cabecas = bpb[15];
	
    infodisco->oculto = bpb[17]
                       | ((unsigned long)bpb[18] << 8)
                       | ((unsigned long)bpb[19] << 16)
                       | ((unsigned long)bpb[20] << 24);
					   
    infodisco->drive = bpb[25];
	
    infodisco->iniciofat = 1;
	
    infodisco->inicioroot = infodisco->tamanhofat 
                          * infodisco->numfats + infodisco->iniciofat;
						  
    infodisco->entradasroot = bpb[6] | ((unsigned int)bpb[7] << 8);
	
    infodisco->tamanhoroot = infodisco->entradasroot / (infodisco->tamanho_setor / 32);
	
    infodisco->setores_por_trilha = (bpb[13] | ((unsigned int)bpb[14] << 8));
	
    infodisco->inicioarquivo = infodisco->inicioroot + infodisco->tamanhoroot;
	
    infodisco->setores_por_cilindro = infodisco->setores_por_trilha 
                                     * infodisco->numero_de_cabecas;
									 
    return;
	
}

/*********************************************************************/

static void lerlogico(INFODISCO *infodisco, long setor, void *buf)
{
    int trilhas;
    int cabeca;
    int set;

    setor += infodisco->oculto;
    trilhas = (int)(setor / infodisco->setores_por_cilindro);
    cabeca = (int)(setor % infodisco->setores_por_cilindro);
    set = cabeca % infodisco->setores_por_trilha + 1;
    cabeca = cabeca / infodisco->setores_por_trilha;
    lerabs(buf, 1, infodisco->drive, trilhas, cabeca, set);
	
    return;
	
}

/*********************************************************************/

int lerabs(void *buf, int setores, int drive, int trilhas, int cabeca, int set)
{

    int rc;
    int ret = -1;
    int tentativas;
    
    naousado(setores);
    tentativas = 0;
	
    while (tentativas < 5)
    {
	
        rc = PXDOSLersetordodisco(buf, 1, drive, trilhas, cabeca, set);
        if (rc == 0)
        {
		
            ret = 0;
			
            break;
			
        }
        PXDOSResetarDisco_CAR(drive);
        tentativas++;
		
    }
	
    return (ret);
	
}

/*********************************************************************/

static int PXDOSLersetordodisco(void *buffer,
                             unsigned int setores,
                             unsigned int drive,
                             unsigned int trilhas,
                             unsigned int cabeca,
                             unsigned int setor)
{

    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;
    
    registradoresentrada.h.ah = 0x02;
    registradoresentrada.h.al = (unsigned char)setores;
    registradoresentrada.h.ch = (unsigned char)(trilhas & 0xff);
    registradoresentrada.h.cl = (unsigned char)setor;
    registradoresentrada.h.cl |= (((unsigned int)trilhas & 0x0300U) >> 2);
    registradoresentrada.h.dh = (unsigned char)cabeca;
    registradoresentrada.h.dl = (unsigned char)drive;
    sregistradores.es = FP_SEGMENTO(buffer);
    registradoresentrada.x.bx = FP_OFFSET(buffer);
	        

    int13x(&registradoresentrada, &registradoressaida, &sregistradores);
    return (registradoressaida.x.cflag);
	
}

/*********************************************************************/

static int PXDOSResetarDisco_CAR(unsigned int drive)
{

    union REGS registradoresentrada;
    union REGS registradoressaida;
    struct SREGS sregistradores;

    registradoresentrada.h.ah = 0x00;
    registradoresentrada.h.dl = (unsigned char)drive;    
    int13x(&registradoresentrada, &registradoressaida, &sregistradores);
    return (registradoressaida.x.cflag);
	
}

/*********************************************************************/