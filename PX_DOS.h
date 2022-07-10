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

#include <stddef.h>

/* Funções de API do PX-DOS */


/* int 20h */

void PX_DOSTermSemRC(void); 

/*****************************************************************************************/


/* int 21h */

void PX_DOSMostrarSaida(int ch); /* função 2 */

int PX_DOSObterCharSemEcho(void); /* função 8 */

void PX_DOSImprimirString(const char *buf); /* função 9 */

int PX_DOSSelecionarDisco(int drive); /* função e */

int PX_DOSObterDrivePadrao(void); /* função 19 */

void PX_DOSDefinirATD(void *ATD); /* função 1a */

void PX_DOSDefinirVetordeInterrupcoes(int numeroint, void *manipulador_int); /* função 25 */

void PX_DOSObterDatadoSistema(int *ano, int *mes, int *dia, int *complemento); /* função 2a */

void PX_DOSObterHoradoSistema(int *hora, /* função 2c */
                      int *minuto, 
                      int *seg, 
                      int *milisegundos);

void *PX_DOSObterATD(void); /* função 2f */

unsigned int PX_DOSObterVersaodoDOS(void); /* função 30 */

void *PX_DOSObterVetordeInterrupcoes(int numeroint); /* função 35 */

int PX_DOSAlterarDir(char *para); /* função 3b */

int PX_DOSAbrirArquivo(const char *nome, /* função 3d */
                int modo,
                int *manuseador);
                
int PX_DOSFecharArquivo(int manuseador); /* função 3e */

void PX_DOSLerArquivo(int fh, /* função 3f */
                 void *dados, 
                 tamanho_t bytes, 
                 tamanho_t *byteslidos);
                 
int PX_DOSEscreverArquivo(int manuseador, /* função 40 */
                 const void *dados,
                 tamanho_t tamanho);
                 
int PX_DOSDeletarArquivo(const char *nomearquivo); /* função 41 */

long PX_DOSMoverPonteirodoArquivo(int manuseador, long deslocamento, int deonde); /* função 42 */

int PX_DOSObterInformacoesdoDispositivo(int manuseador, unsigned int *devinfo);
      /* função 44 subfunção 0 */

int PX_DOSDispositivodeBlocoRemovivel(int drive); /* função 44 subfunção 8 */

int PX_DOSObterDiretorioAtual(int drive, char *dir); /* função 47 */

void *PX_DOSAlocarMemoriaRAM(unsigned int tamanho); /* função 48 */

void *PX_DOSAlocarMem_Paginas(unsigned int paginas, unsigned int *paginasmaximas);

int PX_DOSMemoriaLivre(void *ptr); /* função 49 */

/* função 4a */
int PX_DOSRealocarPaginas(void *ptr, unsigned int novaspaginas, unsigned int *maximo_de_paginas);

void PX_DOSExec(char *prog, void *blocodeParametros); /* função 4b */

void PX_DOSTerminar(int rc); /* função 4c */

int PX_DOSObterCodigodeRetorno(void); /* função 4d */

int PX_DOSEncontrarPrimeiro(char *pronto, int atributo); /* função 4e */

int PX_DOSEncontrarProximo(void); /* função 4f */

int PX_DOSRenomearArquivo(const char *antigo, const char *novo); /* função 56 */


void PX_DOSImprimirInteiro(int x); /* função f3.00 */

void PX_DOSIniciarImpressora(unsigned int impressora); /* função 14 int 90h */

void PX_DOSIniciarSerial(unsigned int porta); /* função 15 int 90h */

void PX_DOSEnviarSerial(unsigned int porta, int caractere, int metodo); /* função 16 int 90h */

void PX_DOSReiniciar(void); /* função f3.01 */

void PX_DOSDefinirBibliotecadeExecucao(void *PXDOSinicio, void *capi); /* função f3.02 */



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
    int numero_entrada_diretorio;    // Número da entrada do diretório(0x0D)*/
    int inicio_cluster;      // Número do cluster inicial para o diretório corrente(0x0F)*/
    int reservado;          
    int inicio_cluster_root;     // Número do cluster para o diretório root. (0x13)*/
    unsigned char atributo;  // Atributo para o arquivo(0x15)*/
    int hora_arquivo;         // Hora do arquivo (0x16)*/
    int data_arquivo;         // Data do arquivo (0x18)*/
    long tamanho_arquivo;     // Tamanho do arquivo (0x1A)*/
    char nome_arquivo[13];    // Nome do arquivo com extensão (0x1E)*/

} Bloco_Arquivo;