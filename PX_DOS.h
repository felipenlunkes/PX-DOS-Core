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

/* Fun��es de API do PX-DOS */


/* int 20h */

void PX_DOSTermSemRC(void); 

/*****************************************************************************************/


/* int 21h */

void PX_DOSMostrarSaida(int ch); /* fun��o 2 */

int PX_DOSObterCharSemEcho(void); /* fun��o 8 */

void PX_DOSImprimirString(const char *buf); /* fun��o 9 */

int PX_DOSSelecionarDisco(int drive); /* fun��o e */

int PX_DOSObterDrivePadrao(void); /* fun��o 19 */

void PX_DOSDefinirATD(void *ATD); /* fun��o 1a */

void PX_DOSDefinirVetordeInterrupcoes(int numeroint, void *manipulador_int); /* fun��o 25 */

void PX_DOSObterDatadoSistema(int *ano, int *mes, int *dia, int *complemento); /* fun��o 2a */

void PX_DOSObterHoradoSistema(int *hora, /* fun��o 2c */
                      int *minuto, 
                      int *seg, 
                      int *milisegundos);

void *PX_DOSObterATD(void); /* fun��o 2f */

unsigned int PX_DOSObterVersaodoDOS(void); /* fun��o 30 */

void *PX_DOSObterVetordeInterrupcoes(int numeroint); /* fun��o 35 */

int PX_DOSAlterarDir(char *para); /* fun��o 3b */

int PX_DOSAbrirArquivo(const char *nome, /* fun��o 3d */
                int modo,
                int *manuseador);
                
int PX_DOSFecharArquivo(int manuseador); /* fun��o 3e */

void PX_DOSLerArquivo(int fh, /* fun��o 3f */
                 void *dados, 
                 tamanho_t bytes, 
                 tamanho_t *byteslidos);
                 
int PX_DOSEscreverArquivo(int manuseador, /* fun��o 40 */
                 const void *dados,
                 tamanho_t tamanho);
                 
int PX_DOSDeletarArquivo(const char *nomearquivo); /* fun��o 41 */

long PX_DOSMoverPonteirodoArquivo(int manuseador, long deslocamento, int deonde); /* fun��o 42 */

int PX_DOSObterInformacoesdoDispositivo(int manuseador, unsigned int *devinfo);
      /* fun��o 44 subfun��o 0 */

int PX_DOSDispositivodeBlocoRemovivel(int drive); /* fun��o 44 subfun��o 8 */

int PX_DOSObterDiretorioAtual(int drive, char *dir); /* fun��o 47 */

void *PX_DOSAlocarMemoriaRAM(unsigned int tamanho); /* fun��o 48 */

void *PX_DOSAlocarMem_Paginas(unsigned int paginas, unsigned int *paginasmaximas);

int PX_DOSMemoriaLivre(void *ptr); /* fun��o 49 */

/* fun��o 4a */
int PX_DOSRealocarPaginas(void *ptr, unsigned int novaspaginas, unsigned int *maximo_de_paginas);

void PX_DOSExec(char *prog, void *blocodeParametros); /* fun��o 4b */

void PX_DOSTerminar(int rc); /* fun��o 4c */

int PX_DOSObterCodigodeRetorno(void); /* fun��o 4d */

int PX_DOSEncontrarPrimeiro(char *pronto, int atributo); /* fun��o 4e */

int PX_DOSEncontrarProximo(void); /* fun��o 4f */

int PX_DOSRenomearArquivo(const char *antigo, const char *novo); /* fun��o 56 */


void PX_DOSImprimirInteiro(int x); /* fun��o f3.00 */

void PX_DOSIniciarImpressora(unsigned int impressora); /* fun��o 14 int 90h */

void PX_DOSIniciarSerial(unsigned int porta); /* fun��o 15 int 90h */

void PX_DOSEnviarSerial(unsigned int porta, int caractere, int metodo); /* fun��o 16 int 90h */

void PX_DOSReiniciar(void); /* fun��o f3.01 */

void PX_DOSDefinirBibliotecadeExecucao(void *PXDOSinicio, void *capi); /* fun��o f3.02 */



typedef struct {

    int tamanho;
	
    int abscor;
	
    int subcor;
	
    unsigned char *psp;
	
    unsigned char *cl;

    void (*chamaratras)(void);
	
    int (*PXDOSinicio)(int *i1, int *i2, int *i3, void *parametros);
	
    void *PX_DOSapi;
	
    void *PXDOSapi;

} PX_DOS_PARAMETROS;

typedef struct {

    char atributo_busca;   // Atributo da pesquisa(0x00)*/
    char drive;            // Drive usado na pesquisa (0x01)*/
    char procura[11];      // Nome usado na pesquisa (0x02)*/
    int numero_entrada_diretorio;    // N�mero da entrada do diret�rio(0x0D)*/
    int inicio_cluster;      // N�mero do cluster inicial para o diret�rio corrente(0x0F)*/
    int reservado;          
    int inicio_cluster_root;     // N�mero do cluster para o diret�rio root. (0x13)*/
    unsigned char atributo;  // Atributo para o arquivo(0x15)*/
    int hora_arquivo;         // Hora do arquivo (0x16)*/
    int data_arquivo;         // Data do arquivo (0x18)*/
    long tamanho_arquivo;     // Tamanho do arquivo (0x1A)*/
    char nome_arquivo[13];    // Nome do arquivo com extens�o (0x1E)*/

} Bloco_Arquivo;