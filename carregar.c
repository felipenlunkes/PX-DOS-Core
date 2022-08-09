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
/*            Carrega o PX-DOS na mem�ria e o executa                */
/*                                                                   */
/*********************************************************************/

#define PXLOADER

#include "fat.h"
#include "bios.h"
#include "bnpxdos.h"
#include "suporte.h"
#include "pxdos.h"
#include "naousado.h"
#include "dump.h"



typedef struct {

    unsigned long setores_por_cilindro;
    unsigned int numero_de_cabecas;
    unsigned int setores_por_trilha;
    unsigned long oculto;
    int drive;
	
} INFODISCO;

/*********************************************************************/

static INFODISCO infodisco;

FAT gfat;

int lerabs(void *buf, int setores, int drive, int trilha, int cabeca, int sett);

static void corrigirexe(unsigned long enderecocarregado, unsigned int seg, unsigned int *entradaprograma);

static void lerlogico(void *ponteirododisco, long setor, void *buf);

static void analisarBpb(INFODISCO *infodisco, unsigned char *bpb);

static unsigned long pxloader_reiniciar(unsigned long parametro);

void novalinha(void);

/*********************************************************************/


void carregarPXDOS(void)
{
   
    unsigned long psp;
    unsigned long carregar;
    unsigned long carregars;
    unsigned char *bpb;

    ARQUIVOFAT arquivofat;
    static char buf[512];
    unsigned long inicio;
    tamanho_t ret;
    unsigned int entradaprograma;
    int x;
       


    psp = 0x10600UL;
	
    carregars = (unsigned long)psp + 0x100;
	
    carregar = carregars;
	
    bpb = (unsigned char *)(0x7c00 - 0x600 + 11);
	
    analisarBpb(&infodisco, bpb);
	
    padraofat(&gfat);
	
    IniciarFat(&gfat, bpb, lerlogico, &infodisco); 
	
    abrirarquivofat(&gfat, "PXDOS.SIS", &arquivofat);
    
	do 
    {
	
        ret = lerarquivofat(&gfat, &arquivofat, buf, 0x200);
        
		for (x = 0; x < 512; x++)
        {
		
            colocarAbs(carregar + x, buf[x]);
			
        }
		
        carregar += 0x200;
		
    } while (ret == 0x200);
	
    inicio = (unsigned long)psp;
	
    inicio >>= 4;
	
    corrigirexe(carregars, (unsigned int)inicio, &entradaprograma);
	
    inicio <<= 16;
	
    inicio += entradaprograma;
	
    chamarlongo(inicio);

    TerminarFat(&gfat);
    
    /* Caso sistema n�o encontrado ou inv�lido, a BIOS pula para o pr�ximo disco */
	
    PXDOS_NovoBoot();
	
    return;
	
}

/*****************************************************************************************/


static void corrigirexe(unsigned long enderecocarregado, 
                   unsigned int seg, 
                   unsigned int *entradaprograma)
{

  unsigned int realocar_segmento;
  int numeroentradas;
  unsigned long inicio_entrada;
  unsigned long inicioprograma; 
  unsigned long consertar;
  int x;
  unsigned int tamanhocabecalho;
  unsigned short val;
  unsigned short consertos;
  unsigned short fixo;
  realocar_segmento = seg; 
  
  tamanhocabecalho = obterAbs(enderecocarregado + 0x08) 
             | ((unsigned int)obterAbs(enderecocarregado + 0x09) << 8);
			 
  tamanhocabecalho *= 16;
  
  inicio_entrada = (unsigned long)(enderecocarregado + (obterAbs(enderecocarregado + 0x18) 
                    | ((unsigned int)obterAbs(enderecocarregado + 0x19) << 8)));
					
  numeroentradas = obterAbs(enderecocarregado + 0x06) 
            | ((unsigned int)obterAbs(enderecocarregado + 0x07) << 8);
			
   inicioprograma = (unsigned long)(enderecocarregado + tamanhocabecalho);
   
    *entradaprograma = tamanhocabecalho + (obterAbs(enderecocarregado + 0x14) 
               | ((unsigned int)obterAbs(enderecocarregado + 0x15) << 8));
			   
    *entradaprograma += 0x100;
	
    realocar_segmento += (tamanhocabecalho + 0x100) / 16;
	
    for (x = 0; x < numeroentradas; x++)
	
    {
        consertos = obterAbs(inicio_entrada + x * 4 + 3);
		
        consertos = (consertos << 8) | obterAbs(inicio_entrada + x * 4 + 2);
		
        fixo = obterAbs(inicio_entrada + x * 4 + 1);
		
        fixo = (fixo << 8) | obterAbs(inicio_entrada + x * 4);
		
        consertar = (unsigned long)(inicioprograma + fixo + consertos * 16);
		
        val = obterAbs(consertar + 1);
		
        val = (val << 8) | obterAbs(consertar);
		
        val += realocar_segmento;
		
        colocarAbs(consertar, val & 0xff);
		
        colocarAbs(consertar + 1, val >> 8);
		
    }
	
   return;
   
}

/*****************************************************************************************/

static void lerlogico(void *ponteirododisco, long setor, void *buf)
{
    int trilha;
    int cabeca;
    int sett;
    INFODISCO *infodisco;
    int ret;

    infodisco = (INFODISCO *)ponteirododisco;
	
    setor += infodisco->oculto;
	
    trilha = (int)(setor / infodisco->setores_por_cilindro);
	
    cabeca = (int)(setor % infodisco->setores_por_cilindro);
	
    sett = cabeca % infodisco->setores_por_trilha + 1;
	
    cabeca = cabeca / infodisco->setores_por_trilha;
	
    ret = lerabs(buf, 1, infodisco->drive, trilha, cabeca, sett);
	
    if (ret != 0) // Caso o carregador n�o encontre o Kernel
    { 
	
        dumpbuf("                                                                        ",72);
		
	    dumpbuf("O Kernel do PX-DOS nao foi encontrado neste disco.", 50);
	    
		dumpbuf("                                                                                                 ",97);
		
		dumpbuf("Pressione qualquer tecla para tentar novamente.", 47);
		
		asm {
		
		mov ax, 0
		int 16h
		
		int 19h
		
		}
		
    }

    return;
	
}

/*****************************************************************************************/

static void analisarBpb(INFODISCO *infodisco, unsigned char *bpb)
{
    infodisco->drive = bpb[25];
	
    infodisco->numero_de_cabecas = bpb[15];
	
    infodisco->oculto = bpb[17]
                       | ((unsigned long)bpb[18] << 8)
                       | ((unsigned long)bpb[19] << 16)
                       | ((unsigned long)bpb[20] << 24);
					   
    infodisco->setores_por_trilha = (bpb[13] | ((unsigned int)bpb[14] << 8));
	
    infodisco->setores_por_cilindro = infodisco->setores_por_trilha 
                                     * infodisco->numero_de_cabecas;
									 
    return;
	
}

/*****************************************************************************************/

static unsigned long pxloader_reiniciar(unsigned long parametro)
{

    naousado(parametro);
    reiniciar();
	
    return (0);
	
}

/*****************************************************************************************/

#ifdef NECESSARIO_EXIBIR

void dumpbuf(unsigned char *buf, int tamanho)
{
    int x;

    for (x = 0; x < tamanho; x++)
    {
	
        PXDOS_ImprimirTexto(0, buf[x], 0);
		
    }
	
    return;
	
}

/*****************************************************************************************/

void novalinha(void){


dumpbuf ("                                                                                ", 80);


}

/*****************************************************************************************/

void dumplongo(unsigned long x)
{
    int y;
    char *z = "0123456789abcdef";
    char buf[9];
    
    for (y = 0; y < 8; y++)
    {
        buf[7 - y] = z[x & 0x0f];
        x /= 16;
    }
	
    buf[8] = '\0';
	
    dumpbuf(buf, 8);
	
    return;
	
}

#endif
