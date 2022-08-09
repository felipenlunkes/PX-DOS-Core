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
/*         N�cleo (Kernel) do Sistema Operacional PX-DOS �           */
/*                                                                   */
/*              � 2013-2016 Felipe Miguel Nery Lunkes                */
/*                  Todos os direitos reservados.                    */
/*                                                                   */
/*********************************************************************/

#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "pxdos.h"
#include "erros.h"
#include "PX_DOS.h"
#include "bios.h"
#include "fat.h"
#include "suporte.h"
#include "bnpxdos.h"
#include "memoria.h"
#include "patmat.h"
#include "naousado.h"

//*****************************************************************

// Defini��es do sistema

#define NUM_PASTAS_MAX 120

#define memoriainicial_pxdos 0x3000

#define VERPXDOS "0.9.0"

#define gerenciamentomemoriaAlocar(m,b,i) pxdosalocarmemoria(m,b,i)

#define gerenciamentomemoriaLivre(m,p) gerenciar_memlivre_PXDOS_16(m,p)

#define NumeroMaximoDiscos 20

#define NUM_ARQUIVOS_MAX 40

#define EnderecoAbsoluto(x) ((void *)(x)) // Obter um ponteiro que ser� convertido para
                                          // um endere�o que pode ser usado pelo sistema

//*****************************************************************

/* Valores das tabelas de parti��o */

// Tabela de parti��es = TP

#define TP_Deslocamento 0x1be 
#define TP_Tamanho 16 /* Comprimento de cada entrada */
#define TP_Entradas 4 /* N�mero m�ximo de parti��es em um disco */
#define TP_SISID 4 /* Tamanho do ID do sistema */
#define TP_FAT12 1 /* FAT 12 ( 12 Bits ) do DOS */
#define TP_FAT16S 4 /* FAT 16 ( 16 Bits < 32MB ) do DOS  */
#define TP_DOSE 5 /* Parti��o DOS extendida */
#define TP_FAT16B 6 /* FAT 16 ( 16 Bits >= 32MB ) do DOS */
#define TP_FAT32 0x0b /* FAT 32 ( 32 Bits ) do Windows 95 */
#define TP_FAT32L 0x0c /* FAT 32 ( 32 Bits ) do Windows 95 LBA */
#define TP_FAT16L 0x0e /* FAT 16 ( 16 Bits ) do Windows 95 LBA */
#define TP_WIN95EL 0x0f /* Extendido LBA do Windows 95 */

//*******************************************************************

// Fim das defini��es do sistema

// Estruturas de dados

//*******************************************************************


typedef struct {

    int ambiente;
	
    unsigned char *fimcmd;
	
    char *parametro1;
	
    char *parametro2;
	
	
} BLOCO_DE_PARAMETROS;


typedef struct {

    FAT fat;
	
    char diretorio_de_trabalho_atual[NUM_PASTAS_MAX];
	
    int acessado;
	
    unsigned long setores_por_cilindro;
	
    unsigned int numero_cabecas;
	
    unsigned int setores_por_trilha;
	
    unsigned long oculto;
	
    int drive;
	
    int lba;
	
} INFODisco;

static struct {

    ARQUIVOFAT arquivofat;
	
    FAT *ponteiroFat;
	
    int emuso;
	
    int especial;
	
} manipulador_arquivo[NUM_ARQUIVOS_MAX];

static struct {

    char *NomeDriver[11];
	int NumeroInterrupcao;
	int TipoDriver;
	int ModeloDriver;
	int VersaoDriver;
	int SubversaoDriver;
	unsigned char *EnderecoDriver;
	int SS;
	int SP;
	int CS;
	int IP;
	unsigned long TamanhoDriver;
	int Carregado;
	
} DRIVER[11];

static struct {

    char *NomeAPP[11];
	int TipoAPP;
	int ModeloAPP;
	int VersaoAPP;
	int SubversaoAPP;
	unsigned char *EnderecoAPP;
	unsigned long TamanhoAPP;
	int Carregado;
	
} APPS[5];

//*******************************************************************


void pxdosInit(void);

static void iniciardiscos(void);

static void escaniarparticoes(int drive);

static void processarparticao(int drive, unsigned char *parametro_disco);

static void processarparticaoextendida(int drive, unsigned char *parametro_disco);

static void PXDOS_TelaAzul(int codigo_erro);

static void iniciararquivos(void);

static int lerabs(void *buf, int setores, int drive, int trilha, int cabeca, int set);

static int registrarAPP(char *nomeAPP[12], int tipo, int modelo, int versao, int subversao, unsigned char *endereco, int arquitetura, int salvar, long tamanho, int carregado);

static void removerAPP(int ID);

static void removerDRIVER(int ID);

static void configurarDRIVER(int SS, int SP, long tamanho, unsigned char *endereco);

static int registrarDRIVER(char *nomeDriver[11], int numerointerrupcao, int tipo, int modelo, int versao, int subversao, int carregado);
				   
static void validarDriver(char *driver, BLOCO_DE_PARAMETROS *blocodeParametros);

static void manipular_divisaoporzero(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs);

static void manipular_int21h(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs);

static void manipular_int90h(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs);

static void manipular_ctrlc(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs);

static void PXDOS_FAZER(char *pronto);

static void PXDOS_TRITURAR(char *destino, char *novo);

static int PXDOS_PROCURAR(void);

static void carregarPXConfig(void);

static void executarPXConfig(void);

static void carregarExe(char *prog, BLOCO_DE_PARAMETROS *blocodeParametros, int SalvarNome);

static void carregarDriver(char *driver, BLOCO_DE_PARAMETROS *blocodeParametros, int contadorDRV);

static int biosPARAdrive(int bios);

static void IniciarImpressora(unsigned int impressora);

void IniciarSerial(unsigned int porta);

void EnviarSerial(unsigned int porta, int caractere);

static int abrirArquivo(const char *arquivo_nome);

static int fecharArquivo(int contArquivo);

static int escreverArquivo(int nome_do_arquivo_a_ser_lido, void *buf, tamanho_t tamanho_buffer);

static int lerArquivo(int contArquivo, void *buf, tamanho_t tamanhobuffer);

static void acessardisco(int drive);

static void ParaMaiusculo(char *palavra);

static void Ir_Para_Maiusculo(char *str);

void despejolongo(unsigned long x);

void despejarbuffer(unsigned char *buf, int tamanho);

static void lersetlogico(void *PonteiroDeDisco, long setor, void *buf);

static void escreversetlogico(void *PonteiroDeDisco, long setor, void *buf);

static int lerabs(void *buf, int setores, int drive, int trilha, int cabeca, int set);

static int lerLBA(void *buf, int setores, int drive, unsigned long setor);

static void analisarBpb(INFODisco *discoinfo, unsigned char *bpb);

int inicioPXDOS(void);


static GERMEMORIA gerenciamentomemoria;       // Inst�ncia da estrutura herdada

/*

�rea de Gerenciamento de Mem�ria do Kernel do PX-DOS

*/


static void gerenciar_memlivre_PXDOS_16(GERMEMORIA *gerenciamentomemoria, void *ptr);

static void *pxdosalocarmemoria(GERMEMORIA *gerenciamentomemoria, tamanho_t bytes, int id);

static void *PXDOSAlocarPaginas(GERMEMORIA *gerenciamentomemoria, tamanho_t paginas, int id);

static void gerenciar_memlivre_16_Paginas(GERMEMORIA *gerenciamentomemoria, tamanho_t pagina);

/*

Vari�veis e constantes globais do sistema


*/

static int pxdosrealrealocarpaginas(GERMEMORIA *gerenciamentomemoria, void *ptr, tamanho_t novaspaginas);

static INFODisco discos[NumeroMaximoDiscos];

static INFODisco InfodoBoot;

static char *dominio_drivers[]= { // Tabela de Dom�nio dos Drivers

"A: - C:    ",
"RELOGIO    ",
"MEM        "

};

static int drivecorrente;

static int drivebootfisico;

static int drivelogicodeboot;

static int ultimodrive;

static int posicaochar;

static int contDriver=0;

static int contAPP=0;

static unsigned char *BlocodeParametrosdoBIOS;

static char *diretorio_de_trabalho_atual;

static int ultimoCodigo;

static int lba;

static long memoria_utilizada_global=0;

static unsigned long setorParticao; /* Offset dos setores da parti��o */

static int atrib;

static int hal_instalada=0;

static char ff_pasta[MAXIMO_ARQUIVOS];

static char ff_pronto[MAXIMO_ARQUIVOS];

static int driver_permitido=0;

static char shell[100] = "";

static char *DiretorioDeTrabalhoAtual;

static char driver[64] = "";

static int offset_driver = 0;

static int manipulador_ff;

static char origem_dados[128];

static char *ATD = origem_dados;

static int contador_subsistemas = 0;

static char *subsistemas[11];

static void *exec_PXDOSinicio;

static void *Nivel_API_C_Execucao;

static int VERSAOPXDOS = 0; // Vers�o do Sistema

static int SUBVERSAOPXDOS = 9; // Subvers�o do Sistema

static int REVISAOPXDOS = 0; // Revis�o do Sistema

static int driversCarregados = 0;

static int retornoErro = 0;

/*

Agora, vari�veis e defini��es a serem utilizadas na implementa��o
APM do PX-DOS� (PXAPM)

*/

static int APM = 0; // Vari�vel que armazena a disponibilidade do recurso.
                    // Caso 1, o APM est� dispon�vel para uso.
					
static int FuncaoAPM = 0; // Armazena a fun��o a ser executada pela 
                          // implementa��o (0 - 3).

static int CodigoErroAPM = 0; // Armazena poss�veis c�digos de erro
                              // gerados ap�s chamadas a fun��es
                              // da implementa��o APM.							  

/*

In�cio do Kernel do PX-DOS


*/

/*****************************************************************************************/

int EntradaKernel(void)
{
    pxdosInit();
 
/*

Se o sistema n�o conseguir encontrar um Interpretador de Comandos valido do PX-DOS
no disco do sistema, ele retornar� a esta fun��o e exibir� a mensagem de erro fatal.
Sem um Interpretador de Comandos o sistema n�o consegue funcionar da maneira apropriada.


*/ 

    PXDOS_TelaAzul(8);
	
	
	printf("Certifique-se que um Interpretador de Comandos valido do PX-DOS esta\n");
	printf("presente neste disco do sistema e tente novamente.\n\n");
	printf("Verifique tambem se o arquivo 'PXCONFIG.SIS', no disco do sistema,\n");
	printf("declara um Interpretador valido para o PX-DOS.\n\n");
	printf("\nPressione a combinacao CTRL+ALT+DEL para reiniciar o computador...\n");
	
	
    return (0);
	
}

/*

M�todo de in�cio do Kernel do PX-DOS

- Instala as interrup��es de software ( Int 21h, Int 20h e a propriet�ria, Int 90h )
- Chama o restante do sistema ( Fun��es de inicializa��o dos componentes de Hardware )

*/

/*****************************************************************************************/

void pxdosInit(void)
{

    int numdrive;
    char *m;

    instalarInt();                 // Instala as interrup��es do PX-DOS
	


/*

Mensagem de Boas Vindas do PX-DOS

*/

// Separado do pr�ximo printf para facilitar a manuten��o.

    printf("\n\n\n"); 
	
    printf("                     Sistema Operacional PX-DOS 16 Bits\n\n");
	printf("                Copyright (C) 2016 Felipe Miguel Nery Lunkes\n\n");
	printf("                                Versao %s\n\n", VERPXDOS);
      
 

/*

O Bloco de Par�metros do BIOS fica no endere�o absoluto de Boot acrescido do
deslocamento de 11 bytes.

*/

    BlocodeParametrosdoBIOS = (unsigned char *)EnderecoAbsoluto(0x7c00 + 11);
	

    drivebootfisico = BlocodeParametrosdoBIOS[25];
	
    drivelogicodeboot = 0;
	
    discos[0].acessado = 0;
	
    discos[1].acessado = 0;
	
    analisarBpb(&InfodoBoot, BlocodeParametrosdoBIOS);
	
    InfodoBoot.lba = 0;
	
    iniciardiscos();
	

    if (drivelogicodeboot == 0)
    {
        /* Caso iniciado por disquete, usar BPB */
		
        if (drivebootfisico < 2)
        {
		
            drivelogicodeboot = drivebootfisico;
			
        }
  
        else
        {
		
            drivelogicodeboot = ultimodrive;
			
            ultimodrive++;
			
        }
		
        discos[drivelogicodeboot] = InfodoBoot;
		
       padraofat(&discos[drivelogicodeboot].fat);
	   
        IniciarFat(&discos[drivelogicodeboot].fat, 
                BlocodeParametrosdoBIOS, 
                lersetlogico, escreversetlogico,
                &discos[drivelogicodeboot]);
				
        strcpy(discos[drivelogicodeboot].diretorio_de_trabalho_atual, "");
		
        discos[drivelogicodeboot].acessado = 1;
    }
	
    drivecorrente = drivelogicodeboot;
	
    diretorio_de_trabalho_atual = discos[drivecorrente].diretorio_de_trabalho_atual;
	
    iniciararquivos();
	
    carregarPXConfig();
	
    gerenciamentomemoriaPadroes(&gerenciamentomemoria);
	
    gerenciamentomemoriaIniciar(&gerenciamentomemoria);

    gerenciamentomemoriaSuprir(&gerenciamentomemoria, (char *)MK_FP(memoriainicial_pxdos,0x0000), 0x5000U);


    executarPXConfig(); // Carrega o Interpretador de Comandos do PX-DOS
	
    gerenciamentomemoriaTerm(&gerenciamentomemoria);
  
    return;
}

/*****************************************************************************************/

/* Para todas as parti��es, discos, parti��es extendidas e n�mero do drive */
   
static void iniciardiscos(void)
{
    int rc;
    int x;

    ultimodrive = 2;    
	
    for (x = 0x80; x < 0x84; x++)
    {
	
        rc = PXDOS_FixarStatusdoDisco(x);
		
        if (rc == 0)
        {
		
            escaniarparticoes(x);
			
        }
    }
	
    return;
}

/*****************************************************************************************/

static void escaniarparticoes(int drive)
{

    unsigned char buf[512];
    int rc;
    int setores = 1;
    int trilha = 0;
    int cabeca = 0;
    int setor = 1;
    int x;
    int IDdosistema;

    /* Ler tabela de parti��es */
	
    rc = lerabs(buf, setores, drive, trilha, cabeca, setor);
	
    if (rc == 0)
    {        
        setorParticao = 0;
		
        /* Para qualquer parti��o */
		
        for (x = 0; x < TP_Entradas; x++)
        {
            lba = 0;
			
            IDdosistema = buf[TP_Deslocamento + x * TP_Tamanho + TP_SISID];
			
            if ((IDdosistema == TP_FAT12)
                || (IDdosistema == TP_FAT16S)
                || (IDdosistema == TP_FAT16B))
            {
			
                processarparticao(drive, &buf[TP_Deslocamento + x * TP_Tamanho]);
				
            } 
			
			
		
				
        }
		
        for (x = 0; x < TP_Entradas; x++)
        {
		
            lba = 0;
			
            IDdosistema = buf[TP_Deslocamento + x * TP_Tamanho + TP_SISID];
			
            if ((IDdosistema == TP_DOSE)
                || (IDdosistema == TP_WIN95EL))
            {
			
                processarparticaoextendida(drive, &buf[TP_Deslocamento + x * TP_Tamanho]);
				
            }
        }
    }
	
    return;
}

/*****************************************************************************************/

static void processarparticao(int drive, unsigned char *parametro_disco)
{
    unsigned char buf[512];
    int rc;
    int setores = 1;
    int trilha;
    int cabeca;
    int set;
    unsigned char *bpb;
    unsigned long setor;

    cabeca = parametro_disco[1];
	
    set = parametro_disco[2] & 0x1f;
	
    trilha = (((unsigned int)parametro_disco[2] & 0xc0) << 2) | parametro_disco[3];
	
    setor = parametro_disco[8]
             | ((unsigned long)parametro_disco[9] << 8)
             | ((unsigned long)parametro_disco[10] << 16)
             | ((unsigned long)parametro_disco[11] << 24);
			 
    setor += setorParticao;
	
    if (lba)
    {
	
        rc = lerLBA(buf,
                     setores,
                     drive,
                     setor);
					 
    }
	
    else
    {
	
        rc = lerabs(buf, 
                     setores, 
                     drive, 
                     trilha,
                     cabeca,
                     set);
					 
    }
	
    if (rc != 0)
    {
	
	
			 PXDOS_TelaAzul(9);
			 
			 printf("\nO sistema de arquivos nesta unidade nao e suportado pelo\n");
			 printf("PX-DOS neste momento. E impossivel iniciar o sistema por este\n");
			 printf("disco. Tente reinstalar o sistema em uma particao formatada\n");
			 printf("como FAT12 ou FAT16, apenas, ou use um disquete do sistema ou\n");
			 printf("disco de recuperacao para iniciar o sistema da forma apropriada.\n\n");

			 printf("Pressione CTRL+ALT+DEL para reiniciar seu computador...\n");
			 
			 for(;;);
			
			
        return;
		
    }
	
    bpb = buf + 11;
	
    analisarBpb(&discos[ultimodrive], bpb);
    
    discos[ultimodrive].lba = lba;
    
    /* 
	
	Alguns setores ocultos n�o aparecem quando a parti��o � formatada como FAT12. 
	Por isso, usamos a quantidade computada.

	*/
	
    discos[ultimodrive].oculto = setor;
    
    /* Caso as parti��es e setores ocultos coincidam, este � o disco de boot */
	
    if ((drive == drivebootfisico)
        && (discos[ultimodrive].oculto == InfodoBoot.oculto))
    {
	
        drivelogicodeboot = ultimodrive;
		
    }
	
    discos[ultimodrive].drive = drive;
	
    padraofat(&discos[ultimodrive].fat);
   
    IniciarFat(&discos[ultimodrive].fat, bpb, lersetlogico, escreversetlogico, &discos[ultimodrive]);
	
    strcpy(discos[ultimodrive].diretorio_de_trabalho_atual, "");
	
    discos[ultimodrive].acessado = 1;
	
    ultimodrive++;
	
    return;
}

/*****************************************************************************************/

static void processarparticaoextendida(int drive, unsigned char *parametro_disco)
{
    unsigned char buf[512];
    int rc;
    int setores = 1;
    int trilha;
    int cabeca;
    int set;
    int x;
    int IDdosistema;
    unsigned long setor;
    unsigned long setorExtendido;

    if (parametro_disco[TP_SISID] == TP_WIN95EL)
    {
	
        lba = 1;
		
    }
	
    cabeca = parametro_disco[1];
	
    set = parametro_disco[2] & 0x1f;
	
    trilha = (((unsigned int)parametro_disco[2] & 0xc0) << 2) | parametro_disco[3];
	
    setor = parametro_disco[8]
             | ((unsigned long)parametro_disco[9] << 8)
             | ((unsigned long)parametro_disco[10] << 16)
             | ((unsigned long)parametro_disco[11] << 24);
			 
    setorExtendido = setor;
	
    while (set != 0)
    {
        if (lba)
        {
            lerLBA(buf,
                    setores,
                    drive,
                    setor);
        }
		
        else
        {
            lerabs(buf,
                    setores, 
                    drive, 
                    trilha,
                    cabeca,
                    set);
        }
		
        IDdosistema = buf[TP_Deslocamento + 0 * TP_Tamanho + TP_SISID];
		
        if ((IDdosistema == TP_FAT12)
            || (IDdosistema == TP_FAT16S)
            || (IDdosistema == TP_FAT16B))
        {
		
            setorParticao = setor;
            processarparticao(drive, buf + TP_Deslocamento);
			
        }

        cabeca = buf[TP_Deslocamento + 1 * TP_Tamanho + 1];
		
        set = buf[TP_Deslocamento + 1 * TP_Tamanho + 2] & 0x1f;
		
        trilha = (((unsigned int)buf[TP_Deslocamento + 1 * TP_Tamanho + 2] 
                & 0xc0) << 2)
                | buf[TP_Deslocamento + 1 * TP_Tamanho + 3];
				
        setor = buf[TP_Deslocamento + 1 * TP_Tamanho + 8]
                 | ((unsigned long)buf[TP_Deslocamento + 1 * TP_Tamanho + 9] << 8)
                 | ((unsigned long)buf[TP_Deslocamento + 1 * TP_Tamanho + 10] << 16)
                 | ((unsigned long)buf[TP_Deslocamento + 1 * TP_Tamanho + 11] << 24);
				 
        setor += setorExtendido;
		
    }
	
    return;
	
}

/*****************************************************************************************/

static void iniciararquivos(void)
{
    int x;
    
    manipulador_arquivo[0].emuso = 1;
    manipulador_arquivo[1].emuso = 1;
    manipulador_arquivo[2].emuso = 1;
    manipulador_arquivo[0].especial = 1;
    manipulador_arquivo[1].especial = 1;
    manipulador_arquivo[2].especial = 1;
	
    for (x = 3; x < NUM_ARQUIVOS_MAX; x++)
    {
	
        manipulador_arquivo[x].emuso = 0;
        manipulador_arquivo[x].especial = 0;
		
    }
    return;
}


/*****************************************************************************************/

static void manipular_int90h(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs)
{

void *var;
int procura_tipo;
long memoria_utilizada=0;
unsigned int dispositivo;
int contar=0;

BLOCO_DE_PARAMETROS p = { 0, "\x2/p\r", NULL, NULL };
    
    int ret;

	 switch (regsentrada->h.ah)
	 
    {
        case 0x01:
		
		if (regsentrada->x.bx == 0x01){  // Exibe apenas a vers�o do Sistema
		
           printf("PX-DOS versao %s", VERPXDOS);
		   
		   }
		   
		if (regsentrada->x.bx == 0x02){ // Exibe uma mensagem completa da vers�o do Sistema
           
		   printf("\nSistema Operacional PX-DOS %s", VERPXDOS);
		   printf("\n\nCopyright (C) 2016 Felipe Miguel Nery Lunkes\n");
		   printf("Todos os direitos reservados.");
		   
		   }
		   
		if (regsentrada->x.bx == 0x03){ // Retorna nos registradores a vers�o

         regssaida->x.ax=VERSAOPXDOS;
         regssaida->x.bx=SUBVERSAOPXDOS;	
         regssaida->x.cx=REVISAOPXDOS;	
		 regssaida->x.dx=2016;
		 regssaida->x.di=0;
		 regssaida->x.si=1;
		 regssaida->x.cflag=0;

        /*

         A vers�o � retornada da seguinte forma:

        AX retorna o n�mero da vers�o de lan�amento.
        BX retorna o n�mero do lan�amento.		
		CX retorna o n�mero da revis�o final do lan�amento.

        */

		}
		
		   break;
		 
		case 0x02:
		

		   PX_DOSTermSemRC(); // Finaliza o programa em execu��o
		   
		   
           break;		

		case 0x03:

         PX_DOSImprimirString(MK_FP(sregs->ds, regsentrada->x.dx));	
		 
		 // Exibe o que est� em DX, que deve terminar em '$'
    
	
	       break;
		   
		case 0x04:

         PX_DOSReiniciar();

           break;	

        case 0x05: // Fun��o para finalizar a execu��o de um Driver

		 PX_DOSTermSemRC(); // Finaliza o programa em execu��o
		 
		 retornoErro = regsentrada->x.ax; // Salva o retorno de erros, se informado
		 
		 regssaida->x.ax=0;
		 
           break;	
		   
	    case 0x06: // Fun��o para definir uma API - Retorno Nulo
		
		 regssaida->x.ax=0;
		 regssaida->x.bx=0;
		 regssaida->x.cx=0;
		 regssaida->x.dx=0;
		 regssaida->h.ah=0;
		
		
		break;
		
		case 0x07:
		
		if (driver_permitido == 0 ){
		
		 validarDriver(MK_FP(sregs->ds, regsentrada->x.dx), &p);
		
		} else {
		
		 regssaida->x.ax=0;
		 regssaida->x.bx=1;
		 regssaida->x.cx=0;
		 regssaida->x.dx=0;
		 regssaida->h.ah=0;
		
		}
		
		break;
		
		case 0x08: // Bloqueia o carregamento de Drivers
		
		if (regsentrada->x.bx == 1){
		
		 driver_permitido=1;
		
		}
		
		if (regsentrada->x.bx == 0){
		
		 driver_permitido=0;
		
		}
		
		break;
		
		case 0x09: // Interrup��o usada pelo Kernel para chamar o carregador
		           // de Drivers
				   
		 carregarExe("CRGDRV.SIS", &p, 0);	 

				   
		break;	
		
		case 0x10:
		
		if (regsentrada->x.bx == 1){
		
		 regssaida->x.ax = driversCarregados; // Retorna o valor de Drivers carregados
		
		}
		
		if (regsentrada->x.bx == 2){
		
		 regssaida->x.ax = retornoErro; // Retorna o retorno de erro do �ltimo Driver
		
		}
		
		break;
		
		case 0x11:
		
		 var = MK_FP(sregs->ds, regsentrada->x.dx);
           
         regssaida->x.ax = deletarArquivo(var);
			
        if (regssaida->x.ax != 0)
        {
			
         regssaida->x.cflag = 1;
				
        }
            break;
			
		case 0x12: // Fun��o de retorno de dados
		
		 procura_tipo = *(int *)&regsentrada->x.bx;
		
		for (contar=0; contar < 11; contar++){
		
		if (DRIVER[contar].TipoDriver == procura_tipo){
		
		 regssaida->x.cx=DRIVER[contar].NumeroInterrupcao; // Retorna o n�mero da interrup��o
		 regssaida->x.dx=contar; // Retorna a posi��o na tabela, que pode ser usada 
		                         // em outras fun��es
			/*					 
		regssaida->x.bx = FP_OFFSET(DRIVER[contar].NomeDriver);
			
        sregs->es = FP_SEGMENTO(DRIVER[contar].NomeDriver);
		*/
		
		} else { // Caso n�o encontre
		
		 regssaida->x.bx=0;
		
		}
		
		}
		
		break;
		
		case 0x13: // Fun��o de retorno de informa��es do sistema
		
		 regssaida->x.ax=VERSAOPXDOS;
		 regssaida->x.bx=SUBVERSAOPXDOS;
		 regssaida->x.cx=0;
		 regssaida->x.dx=0;
		
		break;
		
		case 0x14: // Fun��o para iniciar uma impressora conectada
		
		
		dispositivo = regsentrada->x.dx;
		
		IniciarImpressora(dispositivo);
		
		break;
		
		case 0x15: // Fun��o para iniciar uma impressora conectada
		
		
		dispositivo = regsentrada->x.dx;
		
		IniciarSerial(dispositivo);
		
		break;
		
		case 0x16:
		
		if (regsentrada->x.cx == 0x01) {
		
		dispositivo = regsentrada->x.dx;
		
		EnviarSerial(dispositivo, regsentrada->x.bx);
		
		}
		
		else if (regsentrada->x.cx == 0x02) 
		{
		
		regsentrada->x.dx=regsentrada->x.bx;
		EnviarDadosSerial();
		
		}
		
		else {
		
		printf("\n\nO metodo de envio serial nao foi especificado.\n\n");
		
		regssaida->x.ax = 0x01;
		regssaida->x.cflag = 0x01;
		
		}
	
		break;
		
		case 0x17: // Fun��o para desligamento via APM
		
		regssaida->x.ax = desligar();
		
		// Caso a fun��o retorne, indicando erro em sua execu��o,
		// o sistema dever� informar o usu�ro, assim como tomar
		// as medidas necess�rias. Cada um dos retornos num�ricos
		// apresentam um significado de erro.
		
		if (regssaida->x.ax = 0x00){ // Falha na instala��o da
		                             // implementa��o APM.
		PXDOS_TelaAzul(16);
		
		}
		
		if (regssaida->x.ax = 0x01){ // Falha na conex�o com a interface
		                             // de Modo Real.
		
		PXDOS_TelaAzul(17);
		
		}
		
		if (regssaida->x.ax = 0x02){ // N�o existe suporte ao protocolo
		                             // APM 1.2 necess�rio.
		
		PXDOS_TelaAzul(18);
		
		}
		
		if (regssaida->x.ax = 0x03){ // Erro durante a execu��o do comando
		                             // enviado. Erro desconhecido!
		
		PXDOS_TelaAzul(19);
		
		}
		
		
		break;
		
		case 0x18:
		
		if (regsentrada->x.bx == 0x01) // Fun��o para exibir na tela a rela��o de execut�veis carregados
		{
		
		printf("       NOME        TIPO   TAMANHO\n");
        printf("   -------------  ------ ---------\n"); 		
		
		for (contar=0;contar<10;contar++){
		
		if (DRIVER[contar].Carregado != 0){
		
		memoria_utilizada+=DRIVER[contar].TamanhoDriver/1024;
		
	    printf("   %s       %d     %ld\n", DRIVER[contar].NomeDriver, DRIVER[contar].TipoDriver, DRIVER[contar].TamanhoDriver);
		
		}
		
		}
		
		for (contar=0;contar<5;contar++){
		
		if (APPS[contar].Carregado == 1){
		
		memoria_utilizada+=APPS[contar].TamanhoAPP/1024;
		
	    printf("   %-12s    %s    %ld\n", APPS[contar].NomeAPP,APPS[contar].TipoAPP & 1? "APPX":"EXE ", APPS[contar].TamanhoAPP);
		
		}
		}
		
		}
		
		//****************
		
		if (regsentrada->x.bx == 0x02) // Fun��o para retornar a quantidade de memoria utilizada
		{
		
		for (contar=0;contar<10;contar++){
		
		if (DRIVER[contar].Carregado != 0){
		
		memoria_utilizada+=DRIVER[contar].TamanhoDriver/1024;
		
		}
		}
		
		for (contar=0;contar<5;contar++){
		
		if (APPS[contar].Carregado == 1){
		
		memoria_utilizada+=APPS[contar].TamanhoAPP/1024;
		
		}
		}
	
	    regssaida->x.ax=memoria_utilizada;
		
		}
		
		break;
		
	default:  // Caso nenhuma fun��o v�lida seja declarada, exibir a mensagem de 
	          // erro gen�rica.
	
	     printf("\nInterrupcao PX-DOS %x nao implementada!\n", regsentrada->x.ax);
	
	      regssaida->x.ax = 1; // Retornar c�digo de erro 1
	
	break;
	
	
	}
	
	return;
	
	
}

/*****************************************************************************************/

static void manipular_ctrlc(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs)
{

printf("\n\nCtrl-C pressionado.\n");
printf("O aplicativo atualmente aberto foi finalizado.\n\n");

PX_DOSTermSemRC();; // Finaliza o programa em execu��o

return;

}

/*****************************************************************************************/

static void manipular_divisaoporzero(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs)
{


printf("\n\nUma divis�o por zero ocorreu.\n");
printf("O programa sera finalizado para evitar a execucao de\n");
printf("instrucoes invalidas.\n\n");

PX_DOSTermSemRC();; // Finaliza o programa em execu��o

return;

}

/*****************************************************************************************/

static void manipular_int21h(union REGS *regsentrada, union REGS *regssaida, struct SREGS *sregs)
{

// Interrup��o 21h mantida para a compatibilidade com os aplicativos feitos para outros 
// sistemas baseados na arquitetura DOS. Nem todas as implementa��es est�o presentes.

    void *p;
    void *q;
    long byteslidos;
    int ret;
	int atributo;
    void *dadostemporarios;
    BLOCO_DE_PARAMETROS *bp;

 
    switch (regsentrada->h.ah)
    {
        case 0x02:
		
            PX_DOSMostrarSaida(regsentrada->h.dl);
           
		   break;

        case 0x08:
		
	    regssaida->h.al = PX_DOSObterCharSemEcho();
          
		  break;

        case 0x09:
		
            PX_DOSImprimirString(MK_FP(sregs->ds, regsentrada->x.dx));
           
		   break;

        case 0x0e:
		
            regssaida->h.al = PX_DOSSelecionarDisco(regsentrada->h.dl);
           
		   break;
            
        case 0x19:
		
            regssaida->h.al = PX_DOSObterDrivePadrao();
           
		   break;

        case 0x1a:
           
            dadostemporarios = MK_FP(sregs->ds, regsentrada->x.dx);
            
            PX_DOSDefinirATD(dadostemporarios);
            
			break;

        case 0x25:
                        
            p = MK_FP(sregs->ds, regsentrada->x.dx);
            
            PX_DOSDefinirVetordeInterrupcoes(regsentrada->h.al, p);
           
		   break;
            
        case 0x2a:
            {
                int ano, mes, dia, complemento;
                
                PX_DOSObterDatadoSistema(&ano, &mes, &dia, &complemento);
				
                regssaida->x.cx = ano;
				
                regssaida->h.dh = mes;
				
                regssaida->h.dl = dia;
				
                regssaida->h.al = complemento;
				
            }
           
		   break;

        case 0x2c:
            {
                int hora, minuto, segundo, milisegundos;
                
                PX_DOSObterHoradoSistema(&hora, &minuto, &segundo, &milisegundos);
				
                regssaida->h.ch = hora;
				
                regssaida->h.cl = minuto;
				
                regssaida->h.dh = segundo;
				
                regssaida->h.dl = milisegundos;
				
            }
            
			break;

        case 0x2f:
		
            dadostemporarios = PX_DOSObterATD();
                       
            regssaida->x.bx = FP_OFFSET(dadostemporarios);
			
            sregs->es = FP_SEGMENTO(dadostemporarios);
           
            break;

        case 0x30:
		
           // regssaida->x.ax = PX_DOSObterVersaodoDOS();
			
			regssaida->h.al=VERSAOPXDOS;
			
			regssaida->h.ah=SUBVERSAOPXDOS;
			
            regssaida->x.bx = 0;
			
            regssaida->x.cx = 0;
           
		   break;
        
        case 0x35:
		
            p = PX_DOSObterVetordeInterrupcoes(regsentrada->h.al);
                       
            regssaida->x.bx = FP_OFFSET(p);
			
            sregs->es = FP_SEGMENTO(p);
           
		   break;
            
        case 0x3b:

            regssaida->x.ax = PX_DOSAlterarDir(MK_FP(sregs->ds, regsentrada->x.dx));            
           
		   break;
            
		case 0x3c:

            p = MK_FP(sregs->ds, regsentrada->x.dx);
            ret = PX_DOSCriarArquivo(p, regsentrada->x.cx, &regssaida->x.ax);

            if (ret < 0)
            {
                regssaida->x.cflag = 1;
                regssaida->x.ax = -ret;
            }
			
            break;
			
        case 0x3d:
    
            p = MK_FP(sregs->ds, regsentrada->x.dx);
			
            ret = PX_DOSAbrirArquivo(p, regsentrada->h.al, &regssaida->x.ax);
           
            if (ret < 0)
            {
                regssaida->x.cflag = 1;
				
                regssaida->x.ax = 0x02;
            }
			
            break;
            
        case 0x3e:

            regssaida->x.ax = PX_DOSFecharArquivo(regsentrada->x.bx);
            
            break;
            
        case 0x3f:
            
            p = MK_FP(sregs->ds, regsentrada->x.dx);
			
            PX_DOSLerArquivo(regsentrada->x.bx,
                        p,
                        regsentrada->x.cx,
                        &byteslidos);
						
            regssaida->x.ax = byteslidos;
			
            break;
            
        case 0x40:

            p = MK_FP(sregs->ds, regsentrada->x.dx);
			
            regssaida->x.ax = PX_DOSEscreverArquivo(regsentrada->x.bx,
                                         p,
                                         regsentrada->x.cx);

            break;

        case 0x41:

            p = MK_FP(sregs->ds, regsentrada->x.dx);
           
            regssaida->x.ax = deletarArquivo(p);
			
            if (regssaida->x.ax != 0)
            {
			
                regssaida->x.cflag = 1;
				
            }
            break;

        case 0x42:

            byteslidos = ((unsigned long)regsentrada->x.cx << 16) | regsentrada->x.dx;
			
            byteslidos = PX_DOSMoverPonteirodoArquivo(regsentrada->x.bx,
                                           byteslidos,
                                           regsentrada->h.al);
										   
            regssaida->x.cx = byteslidos >> 16;
			
            regssaida->x.dx = byteslidos & 0xffff;
                                                
            break;
            
			 case 0x43:
			 
            if (regsentrada->h.al == 0x00)
            {

                p = MK_FP(sregs->ds, regsentrada->x.dx);
				
                regssaida->x.ax = PX_DOSObterAtributos(p,&atributo);
                regssaida->x.cx=atributo;
                if (regssaida->x.ax != 0)
                {
                    regssaida->x.cflag = 1;
                }
                else
                {
                    regssaida->x.cflag=0;
                }
            }
            break;
			
        case 0x44:
		
            if (regsentrada->h.al == 0x00)
            {

                regssaida->x.ax = PX_DOSObterInformacoesdoDispositivo(regsentrada->x.bx,
                                                        &regssaida->x.dx);

                if (regssaida->x.ax != 0)
                {
				
                    regssaida->x.cflag = 1;
					
                }
            }
            else if (regsentrada->h.al == 0x08)
            {
                
                regssaida->x.ax = PX_DOSDispositivodeBlocoRemovivel(regsentrada->h.bl);
				
                if ((int)regssaida->x.ax < 0)
                {
				
                    regssaida->x.cflag = 1;
                    regssaida->x.ax = -regssaida->x.ax;
					
                }

            }
			
            break;
                                    
        case 0x47:

            p = MK_FP(sregs->ds, regsentrada->x.si);
			
            regssaida->x.ax = PX_DOSObterDiretorioAtual(regsentrada->h.dl, p);
            
            break;
            
        case 0x48:

            /*
			
			Por algum motivo, AMEMPXDOS (alocador de mem�ria do PX-DOS) aloca 32 bytes  
			
			*/
			
            if (regsentrada->x.bx == 2)
            {
			
                regsentrada->x.bx = 0x400;
				
            }
            p = PX_DOSAlocarMem_Paginas(regsentrada->x.bx, &regssaida->x.bx);
            if (p == NULL)
            {
			
                regssaida->x.cflag = 1;
				
                regssaida->x.ax = 0x08;
				
            }
            else
            {
			
                regssaida->x.ax = FP_SEGMENTO(p) + (FP_OFFSET(p) >> 4);
				
            }
            
            break;
        
        case 0x49:

            regssaida->x.ax = PX_DOSMemoriaLivre(MK_FP(sregs->es, 0));
            
            if (regssaida->x.ax != 0)
            {
			
                regssaida->x.cflag = 1;
				
            }
			
            break;
            
        case 0x4a:

            regssaida->x.ax = PX_DOSRealocarPaginas(MK_FP(sregs->es, 0),
                                            regsentrada->x.bx,
                                            &regssaida->x.bx);
											
            if (regssaida->x.ax != 0)
            {
			
                regssaida->x.cflag = 1;
				
            }
			
            break;
            
        case 0x4b:

             PX_DOSExec(MK_FP(sregs->ds, regsentrada->x.dx), MK_FP(sregs->es, regsentrada->x.bx));
                     
             break;
             
        case 0x4c: 

             PX_DOSTerminar(regsentrada->h.al);

             break;

        case 0x4d:
		
             regssaida->x.ax = PX_DOSObterCodigodeRetorno();
            
			break;
             
        case 0x4e:

             regssaida->x.ax = PX_DOSEncontrarPrimeiro(MK_FP(sregs->ds, regsentrada->x.dx), 
			 regsentrada->x.cx);
			 
             if (regssaida->x.ax != 0)
             {
			 
                 regssaida->x.cflag = 1;
				 
             }  
			 
             break;
             
        case 0x4f:
		
             regssaida->x.ax = PX_DOSEncontrarProximo();
			 
             if (regssaida->x.ax != 0)
             {
			 
                 regssaida->x.cflag = 1;
				 
             }
			 
             break;
             
        case 0x56:

            p = MK_FP(sregs->ds, regsentrada->x.dx);
			
            q = MK_FP(sregs->es, regsentrada->x.di);
            
            regssaida->x.ax = PX_DOSRenomear_Arquivo(p, q);
			
            if (regssaida->x.ax != 0)
            {
			
                regssaida->x.cflag = 1;
				
            }
			
            break;

        /* EMX � a fun��o 0x7f */ 

/* 

Fun��es espec�ficas do PX-DOS

*/	
	
        case 0xfa:

		printf("\nCopyright (C) 2016 Felipe Miguel Nery Lunkes\n");

            break;

			
		case 0xff:
            
            printf("\nSistema Operacional PX-DOS %s\n", VERPXDOS);
            regssaida->x.ax=0;
			regssaida->x.bx=1;
			regssaida->x.cx=1;
			regssaida->x.cflag=0;
			regssaida->x.si='0';
			
			break;
  			

        default:
		
            printf("\nInterrupcao PX-DOS %x nao implementada!\n", regsentrada->x.ax);
           
		   break;
             
    }
	
    return;
	
}

/*****************************************************************************************/

void PX_DOSTermSemRC(void)
{

    PX_DOSTerminar(0);
	
}

/*****************************************************************************************/

void PX_DOSMostrarSaida(int ch)
{

    unsigned char buf[1];

    buf[0] = ch;
	
    PX_DOSEscreverArquivo(1, buf, 1);
	
    return;
	
}

/*****************************************************************************************/

int PX_DOSObterCharSemEcho(void)
{

    int escaniar;
    int ascii;


    PXDOS_LerCaracteredoTeclado(&escaniar, &ascii);

    return ascii;
	
}
      
/*****************************************************************************************/
      
void PX_DOSImprimirString(const char *buf)
{

    const char *p;

    p = memchr(buf, '$', (tamanho_t)-1);
	
    if (p == NULL) p = buf;
	
    PX_DOSEscreverArquivo(1, buf, p - buf);
	
    return;
	
}

/*****************************************************************************************/

int PX_DOSSelecionarDisco(int drive)
{
    drivecorrente = drive;
	
    if (drive < 2)
    {
	
        acessardisco(drive);
		
    }
	
    diretorio_de_trabalho_atual = discos[drive].diretorio_de_trabalho_atual;
	
    return (ultimodrive);
	
}

/*****************************************************************************************/

int PX_DOSObterDrivePadrao(void)
{

    return (drivecorrente);
	
}

/*****************************************************************************************/

void PX_DOSObterDatadoSistema(int *ano, int *mes, int *dia, int *complemento)
{
    return;
}

/*****************************************************************************************/

void PX_DOSObterHoradoSistema(int *hora, int *minuto, int *segundo, int *milisegundos)
{

    return;
	
}

/*****************************************************************************************/

void *PX_DOSObterATD(void)
{

    return (ATD);
	
}

/*****************************************************************************************/

void PX_DOSDefinirATD(void *p)
{

    ATD = p;   
	
    return;
	
}

/*****************************************************************************************/

void PX_DOSDefinirVetordeInterrupcoes(int numeroint, void *manipulador_int)
{

    desativar();
	
    *((void **)0 + numeroint) = manipulador_int;
	
    ativar();
	
}

/*****************************************************************************************/

unsigned int PX_DOSObterVersaodoDOS(void)
{

    int versao;
    
    /* 
	
	Retornar vers�o do PX-DOS para 4.00, para manter compatibilidade com programas
    antigos desenvolvidos para vers�es do DOS 4.00
	  
	*/
	
    versao = (0x00 << 8) | 0x04;
	
    return (versao);
	
}

/*****************************************************************************************/

void *PX_DOSObterVetordeInterrupcoes(int numeroint)
{

    return *((void **)0 + numeroint);
	
}

/*****************************************************************************************/

int PX_DOSAlterarDir(char *para)
{

    char *p;
    
    if (strcmp(para, "..") == 0)
    {
	
        p = strrchr(diretorio_de_trabalho_atual, '\\');
		
        if (p != NULL)
		
        {
            *p = '\0';
			
        }
		
        else
        {
		
            strcpy(diretorio_de_trabalho_atual, "");
			
        }
    }
	
    else if (para[0] == '\\')
    {
	
        strcpy(diretorio_de_trabalho_atual, para + 1);
		
    }
	
    else
    {
	
        if (strcmp(diretorio_de_trabalho_atual, "") != 0)
        {
		
            strcat(diretorio_de_trabalho_atual, "\\");
			
        }
		
        strcat(diretorio_de_trabalho_atual, para);
		
    }
	
    ParaMaiusculo(diretorio_de_trabalho_atual);
	
    return (0);
	
}

/*****************************************************************************************/

int PX_DOSAbrirArquivo(const char *nome, int modo, int *manuseador)
{

    char NOMEARQUIVO[NUM_PASTAS_MAX];
    int contArquivo;
    int ret;
	char *nome_original;

    nome_original = nome;

    if( modo==1 )
    {
	
        ret=PX_DOSCriarArquivo(nome, 0, manuseador);
		
        return(ret);
		
    }
	
    if ( nome[1] == ':' )
    {
	
        nome += 2;
		
    }
	
    if ((nome[0] == '\\') || (nome[0] == '/'))
    {
	
        contArquivo = abrirArquivo(nome);
		
    }
	
    else
    {
        
		strcpy(NOMEARQUIVO, diretorio_de_trabalho_atual);
		
        strcat(NOMEARQUIVO, "\\");
        strcat(NOMEARQUIVO, nome);
		
        contArquivo = abrirArquivo(NOMEARQUIVO);
		
    }
	
    if (contArquivo < 0)
    {
	
        *manuseador = -contArquivo;
		
    }
	
    else
    {
	
        *manuseador = contArquivo;
		
    }
	
    return (contArquivo);
	
}

/*****************************************************************************************/

int PX_DOSFecharArquivo(int contArquivo)
{

    int ret;
    
    ret = fecharArquivo(contArquivo);
	
    return (ret);
	
}

/*****************************************************************************************/

void PX_DOSLerArquivo(int conteudo_arquivo_a_ser_lido, void *dados, tamanho_t bytes, tamanho_t *byteslidos)
{

    unsigned char *p;
    tamanho_t x = 0;

    if (conteudo_arquivo_a_ser_lido < 3)
    {
	
        p = (unsigned char *)dados;
		
        for (; x < bytes; x++)
        {
		
            int escaniar;
            int ascii;
            
            PXDOS_LerCaracteredoTeclado(&escaniar, &ascii);
			
            p[x] = ascii;
			
            PXDOS_ImprimirTexto(0, ascii, 0);
			
            if (ascii == '\r')
            {
			
                PXDOS_ImprimirTexto(0, '\n', 0);
              
                x++;
				
                p[x] = '\n';
				
                x++;
				
                break;
				
            }
        }
    }
	
    else
    {
	
        x = lerArquivo(conteudo_arquivo_a_ser_lido, dados, bytes);
    
	}
	
    *byteslidos = x;
	
    return;
	
}

/*****************************************************************************************/

int PX_DOSEscreverArquivo(int conteudo_arquivo_a_ser_lido, const void *dados, tamanho_t tamanho)
{

  unsigned char *p;
  tamanho_t x;

    if (conteudo_arquivo_a_ser_lido < 3)
    {
        p = (unsigned char *)dados;
		
        for (x = 0; x < tamanho; x++)
        {
		
            PXDOS_ImprimirTexto(0, p[x], 0);
			
        }
    }
	
    else
    {
	
        tamanho = escreverArquivo(conteudo_arquivo_a_ser_lido, dados, tamanho);
		
    }
	
    return (tamanho);
	
}

/*****************************************************************************************/

static int escreverArquivo(int nome_do_arquivo_a_ser_lido, void *buf, tamanho_t tamanho_buffer)
{
    tamanho_t ret;

    ret = EscreverArquivoFat(manipulador_arquivo[nome_do_arquivo_a_ser_lido].ponteiroFat, &manipulador_arquivo[nome_do_arquivo_a_ser_lido].arquivofat, buf, tamanho_buffer);
	
    return (ret);
	
}

/*****************************************************************************************/

int PX_DOSDeletarArquivo(const char *arnome)
{

    int x;
    const char *p;
    int drive;
    int rc;
    char despejoTemporario[NUM_ARQUIVOS_MAX];

    strcpy(despejoTemporario, arnome);
	
    Ir_Para_Maiusculo(despejoTemporario);
	
    arnome = despejoTemporario;
	
    p = strchr(arnome, ':');
	
    if (p == NULL)
    {
	
        p = arnome;
		
        drive = drivecorrente;
		
    }
	
    else
    {
	
        drive = *(p - 1);
		
        drive = paramaiusculo(drive) - 'A';
		
        p++;
		
    }

    rc = DeletarArquivoFat(&discos[drive].fat, p);
	
    if (rc != 0) return (-1);
	
    return (rc);

	
}

/*****************************************************************************************/

int deletarArquivo(const char *nome)
{
    char nomearquivo[NUM_ARQUIVOS_MAX];
    int ret;
    char *origem;

    origem = nome;

    if(nome[1] == ':')
    {
        nome += 2;
    }
    if ((nome[0] == '\\') || (nome[0] == '/'))
    {
        ret = PX_DOSDeletarArquivo(origem);
    }
    else
    {
        strcpy(nomearquivo, DiretorioDeTrabalhoAtual);
        strcat(nomearquivo, "\\");
        strcat(nomearquivo, nome);
        ret = PX_DOSDeletarArquivo(nomearquivo);
    }
    if (ret < 0)
    {
        return(ret);
    }
    return (ret);
}

/*****************************************************************************************/

static void Ir_Para_Maiusculo(char *str)
{
    int x;

    for (x = 0; str[x] != '\0'; x++)
    {
	
        str[x] = paramaiusculo(str[x]);
		
    }
	
    return;
	
}

/*****************************************************************************************/

long PX_DOSMoverPonteirodoArquivo(int manuseador, long deslocamento, int deonde)
{

    return (0);
	
}

/*****************************************************************************************/

int PX_DOSObterInformacoesdoDispositivo(int manuseador, unsigned int *devinfo)
{

    *devinfo = 0;
	
    if (manuseador < 3)
    {
	
        if (manuseador == 0)
        {
		
            *devinfo |= (1 << 0); // Dispositivo de entrada 
        
		}
		
        else if ((manuseador == 1) || (manuseador == 2))
        {
		
            *devinfo |= (1 << 1); // Dispositivo de sa�da
			
        }
		
        *devinfo |= (1 << 7); // Dispositivo 
    }
	
    else
    {
       
	   // Dispositivo de Bloco 
  
  }
    
	return (0);
	
}

/*****************************************************************************************/

void IniciarImpressora(unsigned int impressora){

PXDOS_IniciarImpressora(impressora);

return (0);

}

/*****************************************************************************************/

void EnviarSerial(unsigned int porta, int caractere)
{

PXDOS_IniciarSerial(porta);
PXDOS_EnviarSerial(porta, caractere);

return (0);


}

/*****************************************************************************************/

void IniciarSerial(unsigned int porta){

PXDOS_IniciarSerial(porta);

return (0);

}

/*****************************************************************************************/

int PX_DOSDispositivodeBlocoRemovivel(int drive)
{

    if (drive < 2)
    {
	
        return (0);
		
    }
	
    else
    {
	
        return (1);
		
    }
	
}

/*****************************************************************************************/

int PX_DOSObterDiretorioAtual(int drive, char *buf)
{

    if (drive == 0)
    {
	
        drive = drivecorrente;
		
    }
	
    else
    {
	
        drive--;
		
    }
    
	strcpy(buf, discos[drive].diretorio_de_trabalho_atual);
    
	return (0);
	
}

/*****************************************************************************************/

void *PX_DOSAlocarMemoriaRAM(unsigned int tamanho)
{

    return (gerenciamentomemoriaAlocar(&gerenciamentomemoria, tamanho, 0)); 
	
}

/*****************************************************************************************/

void *PX_DOSAlocarMem_Paginas(unsigned int paginas, unsigned int *paginasmaximas)
{
   
   void *p;
    
   p = PXDOSAlocarPaginas(&gerenciamentomemoria, paginas, 0);
   
    if (p == NULL)
    {
	
        *paginasmaximas = gerenciamentomemoriaTamanhoMaximo(&gerenciamentomemoria);
		
    }
	
    return (p);
	
}

/*****************************************************************************************/

int PX_DOSMemoriaLivre(void *ptr)
{

    gerenciamentomemoriaLivre(&gerenciamentomemoria, ptr);
	
    return (0);
	
}

/*****************************************************************************************/

int PX_DOSRealocarPaginas(void *ptr, unsigned int novaspaginas, unsigned int *maximo_de_paginas)
{

    int ret;

    ret = pxdosrealrealocarpaginas(&gerenciamentomemoria, ptr, novaspaginas);
	
    if (ret != 0)
    {
       
	   *maximo_de_paginas = gerenciamentomemoriaTamanhoMaximo(&gerenciamentomemoria);
        ret = 8;
		
    }
	
    return (ret);
	
}

/*****************************************************************************************/

void PX_DOSExec(char *prog, void *blocodeParametros)
{

    BLOCO_DE_PARAMETROS *parametros = blocodeParametros;
    char temporario[MAXIMO_ARQUIVOS];
	char APP[MAXIMO_ARQUIVOS];
    
    if ((strchr(prog, '\\') == NULL) && (strchr(prog, '/') == NULL))
    {
	
	    strcpy(APP, prog);
		ParaMaiusculo(APP);
		
        memcpy(APPS[contAPP].NomeAPP, APP, sizeof APP); // Copia o nome do APP para o registro
	   
	    strcpy(temporario, diretorio_de_trabalho_atual);
		
        strcat(temporario, "\\");
        strcat(temporario, prog);
		
    }
	
    else
    {
	
        strcpy(temporario, prog);
		
    }
	
    ParaMaiusculo(temporario);
	
    prog = temporario;
	
    carregarExe(prog, parametros, 0);
	
    return;
	
}

/*****************************************************************************************/

void PX_DOSTerminar(int rc)
{

    chamareignorar(rc);
	
	// Altera��o direta do registro
	
	removerAPP(contAPP);
	
    return;
	
}
  
/*****************************************************************************************/
  
int PX_DOSObterCodigodeRetorno(void)
{

    return (ultimoCodigo);
	
}

/*****************************************************************************************/

int PX_DOSObterAtributos(const char *nome_arq,int *atributo)
{
    int ret;

    ret=ObterAtributos(nome_arq,atributo);
	
    return (ret);
}

/*****************************************************************************************/

int PX_DOSEncontrarPrimeiro(char *pronto, int atributo)
{

    int ret;
    
    atrib = atributo;
	
    memset(ATD, '\0', 0x15); // Limpar �rea reservada 
	
    PXDOS_FAZER(pronto);
	
    manipulador_ff = abrirArquivo(ff_pasta);
	
    if (manipulador_ff < 0)
    {
	
        return (3);
		
    }
	
    ret = PXDOS_PROCURAR();
	
    if (ret == 0x12)
    {
	
        ret = 2;
		
    }
	
    return (ret);
	
}

/*****************************************************************************************/

int PX_DOSEncontrarProximo(void)
{

    return (PXDOS_PROCURAR());
	
}

/*****************************************************************************************/

// Fun��es a serem implementadas completamente no futuro

static int ObterAtributos(const char *nome_arq,int *atributos)
{
    int x;
    const char *p;
    int drive;
    int rc;
    char despejoTemporario[MAXIMO_ARQUIVOS];

    strcpy(despejoTemporario, nome_arq);
	
    Ir_Para_Maiusculo(despejoTemporario);
	
    nome_arq = despejoTemporario;
	
    p = strchr(nome_arq, ':');
	
    if (p == NULL)
    {
	
        p = nome_arq;
		
        drive = drivecorrente;
		
    }
	
    else
    {
	
        drive = *(p - 1);
		
        drive = paramaiusculo(drive) - 'A';
		
        p++;
		
    }

    rc = ObterAtributosFat(&discos[drive].fat, p,atributos);
	
    return (rc);
	
}

/*****************************************************************************************/

int PX_DOSCriarArquivo(const char *nome_a_ser_criado, int atributo_a_ser_criado, int *manipulador_de_arquivo_a_ser_criado)
{
    char nome_arquivo_a_ser_Criado[NUM_PASTAS_MAX];
    int nome_a_criar;
    char *nome_original;

    nome_original = nome_a_ser_criado;
	
    if (nome_a_ser_criado[1] == ':')
    {
	
        nome_a_ser_criado += 2;
		
    }
	
    if ((nome_a_ser_criado[0] == '\\') || (nome_a_ser_criado[0] == '/'))
    {
	
        nome_a_criar = CriarArquivo(nome_original, atributo_a_ser_criado);
		
    }
	
    else
    {
	
        strcpy(nome_arquivo_a_ser_Criado, DiretorioDeTrabalhoAtual);
		
        strcat(nome_arquivo_a_ser_Criado, "\\");
		
        strcat(nome_arquivo_a_ser_Criado, nome_a_ser_criado);
		
        nome_a_criar = CriarArquivo(nome_arquivo_a_ser_Criado, atributo_a_ser_criado);
		
    }
	
    if (nome_a_criar < 0)
    {
	
        *manipulador_de_arquivo_a_ser_criado = -nome_a_criar;
		
    }
	
    else
    {
	
        *manipulador_de_arquivo_a_ser_criado = nome_a_criar;
		
    }
	
    return (nome_a_criar);
	
}

/*****************************************************************************************/

static int CriarArquivo(const char *nome_criar, int atributo_criar)
{
    int x;
    const char *p;
    int drive;
    int rc;
    char despejoTemporario[NUM_ARQUIVOS_MAX];

    strcpy(despejoTemporario, nome_criar);
	
    Ir_Para_Maiusculo(despejoTemporario);
	
    nome_criar = despejoTemporario;
	
    p = strchr(nome_criar, ':');
	
    if (p == NULL)
    {
	
        p = nome_criar;
		
        drive = drivecorrente;
		
    }
	
    else
    {
	
        drive = *(p - 1);
		
        drive = paramaiusculo(drive) - 'A';
		
        p++;
		
    }
	
    for (x = 0; x < NUM_ARQUIVOS_MAX; x++)
    {
	
        if (!manipulador_arquivo[x].emuso)
        {
		
            manipulador_arquivo[x].emuso = 1;
			
            manipulador_arquivo[x].especial = 0;
			
            break;
			
        }
		
    }
	
    if (x == NUM_ARQUIVOS_MAX) return (-4); 
	
    rc = criararquivofat(&discos[drive].fat, p, &manipulador_arquivo[x].arquivofat, atributo_criar);
	
    if (rc < 0) return (rc);
	
    manipulador_arquivo[x].ponteiroFat = &discos[drive].fat;
	
    return (x);
	
}

/*****************************************************************************************/

int PX_DOSRenomear_Arquivo(const char *antigo, const char *novo)
{

    int x;
    const char *p;
    const char *q;
    int drive;
    int rc;
    char despejoTemporario1[NUM_ARQUIVOS_MAX];
    char despejoTemporario2[NUM_ARQUIVOS_MAX];

    strcpy(despejoTemporario1, antigo);
	
    strcpy(despejoTemporario2, novo);
	
    Ir_Para_Maiusculo(despejoTemporario1);
	
    Ir_Para_Maiusculo(despejoTemporario2);
	
    antigo = despejoTemporario1;
	
    novo = despejoTemporario2;
	
    p = strchr(antigo, ':');
	
    if (p == NULL)
    {
	
        p = antigo;
		
        drive = drivecorrente;
		
    }
	
    else
    {
	
        drive = *(p - 1);
		
        drive = paramaiusculo(drive) - 'A';
		
        p++;
		
    }

    rc = RenomearArquivoFat(&discos[drive].fat, p,novo);
	
    if (rc != 0) return (-1);
	
    return (rc);
	
	
}

/*****************************************************************************************/

void PX_DOSImprimirInteiro(int x)
{

    printf("Inteiro = %x\n", x);

    return;
	
}

/*****************************************************************************************/

void PX_DOSReiniciar(void)
{

    int *PX_DOS_Tipo_R = (int *)0x472;
	
    void (*PX_DOS_Funcao_R)(void) = (void (*)(void))MK_FP(0xffff, 0x0000);
    
    *PX_DOS_Tipo_R = 0x1234;
	
    PX_DOS_Funcao_R();
	
    return;
	
}

/*****************************************************************************************/

static void PXDOS_FAZER(char *pronto)
{

    char *p;
    
    strcpy(ff_pronto, pronto);
	
    ParaMaiusculo(ff_pronto);
	
    PXDOS_TRITURAR(ff_pasta, ff_pronto);
	
    p = strrchr(ff_pasta, '\\');
	
    if (p != NULL)
    {
	
        strcpy(ff_pronto, p + 1);
		
        *p = '\0';
		
    }
	
    else
    {
        p = strrchr(ff_pasta, ':');
		
        if (p != NULL)
        {
           
		   strcpy(ff_pronto, p + 1);
		   
            *(p + 1) = '\0';
			
        }
		
        else
        {
            
			strcpy(ff_pronto, ff_pasta);
			
            ff_pasta[0] = '\0';
			
        }
    }
	
    if (strcmp(ff_pronto, "") == 0)
    {
        
		strcpy(ff_pronto, "*.*");
		
    }
	
    return;
	
}

/*****************************************************************************************/

static void PXDOS_TRITURAR(char *destino, char *novo)
{

    char *meudiretorio_de_trabalho_atual;
    char *p;
    int drive;

    strcpy(destino, "");    
    meudiretorio_de_trabalho_atual = diretorio_de_trabalho_atual;
	
    if (strchr(novo, ':') != NULL)
    {
       
	   drive = *novo - 'A';
		
        if (drive < 2)
        {
		
            acessardisco(drive);
        }
		
        meudiretorio_de_trabalho_atual = discos[drive].diretorio_de_trabalho_atual;
		
        memcpy(destino, novo, 2);
		
        destino[2] = '\0';
        novo += 2;
    }
	
    if ((*novo == '/') || (*novo == '\\'))
    {
	
        strcat(destino, novo);
		
    }
	
    else if ((strchr(novo, '\\') != NULL) || (strchr(novo, '/') != NULL))
    {
      
    	strcat(destino, "\\");
		
        strcat(destino, novo);
		
    }
	
    else
    {
       
	   strcat(destino, meudiretorio_de_trabalho_atual);
      
	  while (memcmp(novo, "..", 2) == 0)
        {
            
			p = strrchr(destino, '\\');
			
            if (p != NULL)
            {
			
                *p = '\0';
           
		   }

            else
            {
			
                strcpy(destino, "\\");
            }
			
            
			novo += 3;
        
		}
		
        strcat(destino, "\\");
        strcat(destino, novo);
		
    }
	
    return;
	
}

/*****************************************************************************************/

static int PXDOS_PROCURAR(void)
{

    int ret;
    unsigned char buf[32];
    char arquivo[13];
    char *p;
    
    ret = lerArquivo(manipulador_ff, buf, sizeof buf);
	
    while ((ret == sizeof buf) && (buf[0] != '\0'))
    
	{
        if (buf[0] != 0xe5)
        {
            
			memcpy(arquivo, buf, 8);
			
            arquivo[8] = '\0';
			
            p = strchr(arquivo, ' ');
			
            if (p != NULL)
            {
			
                *p = '\0';
				
            }
			
			
            else
            {
			
                p = arquivo + tamanhostring(arquivo);
				
            }
			
            *p++ = '.';
			
            memcpy(p, buf + 8, 3);
			
            p[3] = '\0';
			
            p = strchr(arquivo, ' ');
           
		   if (p != NULL)
            {
			
                *p-- = '\0';
				
            }
			
            if (patmat(arquivo, ff_pronto) 
            
              
                && (((buf[0x0b] & 0x10) == 0)
                    || ((atrib & 0x10) != 0))
                    
              
                && (((buf[0x0b] & 0x08) == 0)
                    || ((atrib & 0x08) != 0))
                    
               )
			   
            {
                if ((p != NULL) && (*p == '.')) *p = '\0';
				
                ATD[0x15] = buf[0x0b]; // Atributo 
				
                memcpy(ATD + 0x16, buf + 0x16, 2); // Tempo 
				
                memcpy(ATD + 0x18, buf + 0x18, 2); // Tempo 
				
                memcpy(ATD + 0x1a, buf + 0x1c, 4); // Tamanho do arquivo 
				
                /*
				
				memcpy(ATD + 0x1e, buf, 11);
                ATD[0x1e + 11] = '\0';
				
				*/
				
                memset(ATD + 0x1e, '\0', 13);
				
                strcpy(ATD + 0x1e, arquivo);
				
                return (0);
            }
        }
		
        ret = lerArquivo(manipulador_ff, buf, sizeof buf);
		
    }
	
    fecharArquivo(manipulador_ff);
	
    return (0x12);
	
}

/*****************************************************************************************/

void int90(unsigned int *pontregistradores, 
           unsigned int es, 
           unsigned int ds,
           unsigned int di,
           unsigned int si,
           unsigned int dx,
           unsigned int cx,
           unsigned int bx,
           unsigned int cflag,
           unsigned int ax)
		   {
		   
		   static union REGS regsentrada;
           static union REGS regssaida;
           static struct SREGS sregs;
		   
	regsentrada.x.ax = ax;
    regsentrada.x.bx = bx;
    regsentrada.x.cx = cx;
    regsentrada.x.dx = dx;
    regsentrada.x.si = si;
    regsentrada.x.di = di;
    regsentrada.x.cflag = 0;
    sregs.ds = ds;
    sregs.es = es;
	
    memcpy(&regssaida, &regsentrada, sizeof regssaida);
	
    manipular_int90h(&regsentrada, &regssaida, &sregs);
	
    pontregistradores[0] = sregs.es;
    pontregistradores[1] = sregs.ds;
    pontregistradores[2] = regssaida.x.di;
    pontregistradores[3] = regssaida.x.si;
    pontregistradores[4] = regssaida.x.dx;
    pontregistradores[5] = regssaida.x.cx;
    pontregistradores[6] = regssaida.x.bx;
    pontregistradores[7] = regssaida.x.cflag;
    pontregistradores[8] = regssaida.x.ax;
   
   return;
		   
}

/*****************************************************************************************/

void int0(unsigned int *pontregistradores, 
           unsigned int es, 
           unsigned int ds,
           unsigned int di,
           unsigned int si,
           unsigned int dx,
           unsigned int cx,
           unsigned int bx,
           unsigned int cflag,
           unsigned int ax)
		   {
		   
		   static union REGS regsentrada;
           static union REGS regssaida;
           static struct SREGS sregs;
		   
	regsentrada.x.ax = ax;
    regsentrada.x.bx = bx;
    regsentrada.x.cx = cx;
    regsentrada.x.dx = dx;
    regsentrada.x.si = si;
    regsentrada.x.di = di;
    regsentrada.x.cflag = 0;
    sregs.ds = ds;
    sregs.es = es;
	
    memcpy(&regssaida, &regsentrada, sizeof regssaida);
	
    manipular_divisaoporzero(&regsentrada, &regssaida, &sregs);
	
    pontregistradores[0] = sregs.es;
    pontregistradores[1] = sregs.ds;
    pontregistradores[2] = regssaida.x.di;
    pontregistradores[3] = regssaida.x.si;
    pontregistradores[4] = regssaida.x.dx;
    pontregistradores[5] = regssaida.x.cx;
    pontregistradores[6] = regssaida.x.bx;
    pontregistradores[7] = regssaida.x.cflag;
    pontregistradores[8] = regssaida.x.ax;
   
   return;
		   
}

/*****************************************************************************************/

void intctrlc(unsigned int *pontregistradores, 
           unsigned int es, 
           unsigned int ds,
           unsigned int di,
           unsigned int si,
           unsigned int dx,
           unsigned int cx,
           unsigned int bx,
           unsigned int cflag,
           unsigned int ax)
		   {
		   
		   static union REGS regsentrada;
           static union REGS regssaida;
           static struct SREGS sregs;
		   
	regsentrada.x.ax = ax;
    regsentrada.x.bx = bx;
    regsentrada.x.cx = cx;
    regsentrada.x.dx = dx;
    regsentrada.x.si = si;
    regsentrada.x.di = di;
    regsentrada.x.cflag = 0;
    sregs.ds = ds;
    sregs.es = es;
	
    memcpy(&regssaida, &regsentrada, sizeof regssaida);
	
    manipular_ctrlc(&regsentrada, &regssaida, &sregs);
	
    pontregistradores[0] = sregs.es;
    pontregistradores[1] = sregs.ds;
    pontregistradores[2] = regssaida.x.di;
    pontregistradores[3] = regssaida.x.si;
    pontregistradores[4] = regssaida.x.dx;
    pontregistradores[5] = regssaida.x.cx;
    pontregistradores[6] = regssaida.x.bx;
    pontregistradores[7] = regssaida.x.cflag;
    pontregistradores[8] = regssaida.x.ax;
   
   return;
		   
}

/*****************************************************************************************/
		   		   
void int20(unsigned int *pontregistradores,
           unsigned int es,
           unsigned int ds,
           unsigned int di,
           unsigned int si,
           unsigned int dx,
           unsigned int cx,
           unsigned int bx,
           unsigned int cflag,
           unsigned int ax)
{

    static union REGS regsentrada;
    static union REGS regssaida;
    static struct SREGS sregs;

    regsentrada.x.ax = ax;
    regsentrada.x.bx = bx;
    regsentrada.x.cx = cx;
    regsentrada.x.dx = dx;
    regsentrada.x.si = si;
    regsentrada.x.di = di;
    regsentrada.x.cflag = 0;
    sregs.ds = ds;
    sregs.es = es;
	
    memcpy(&regssaida, &regsentrada, sizeof regssaida);
	
    PX_DOSTermSemRC();
	
    pontregistradores[0] = sregs.es;
    pontregistradores[1] = sregs.ds;
    pontregistradores[2] = regssaida.x.di;
    pontregistradores[3] = regssaida.x.si;
    pontregistradores[4] = regssaida.x.dx;
    pontregistradores[5] = regssaida.x.cx;
    pontregistradores[6] = regssaida.x.bx;
    pontregistradores[7] = regssaida.x.cflag;
    pontregistradores[8] = regssaida.x.ax;
   
   return;
}

/*****************************************************************************************/

void int21(unsigned int *pontregistradores,
           unsigned int es,
           unsigned int ds,
           unsigned int di,
           unsigned int si,
           unsigned int dx,
           unsigned int cx,
           unsigned int bx,
           unsigned int cflag,
           unsigned int ax)
{

    static union REGS regsentrada;
    static union REGS regssaida;
    static struct SREGS sregs;

    regsentrada.x.ax = ax;
    regsentrada.x.bx = bx;
    regsentrada.x.cx = cx;
    regsentrada.x.dx = dx;
    regsentrada.x.si = si;
    regsentrada.x.di = di;
    regsentrada.x.cflag = 0;
    sregs.ds = ds;
    sregs.es = es;
	
    memcpy(&regssaida, &regsentrada, sizeof regssaida);
	
    manipular_int21h(&regsentrada, &regssaida, &sregs);
	
    pontregistradores[0] = sregs.es;
    pontregistradores[1] = sregs.ds;
    pontregistradores[2] = regssaida.x.di;
    pontregistradores[3] = regssaida.x.si;
    pontregistradores[4] = regssaida.x.dx;
    pontregistradores[5] = regssaida.x.cx;
    pontregistradores[6] = regssaida.x.bx;
    pontregistradores[7] = regssaida.x.cflag;
    pontregistradores[8] = regssaida.x.ax;
   
   return;
}

/*****************************************************************************************/

static void carregarPXConfig(void)
{

    tamanho_t ret;
    unsigned char buf[512];
    int x;
    int conteudo_arquivo_a_ser_lido;

    conteudo_arquivo_a_ser_lido = abrirArquivo("PXCONFIG.SIS"); // Neste arquivo podem
                                                                // constar informa��es
	                                                            // de um Driver a ser
																// carregado ou o Shell
																// alternativo � ser 
																// utilizado.
	
    if (conteudo_arquivo_a_ser_lido >= 0)
    {
        do
        {
            ret = lerArquivo(conteudo_arquivo_a_ser_lido, buf, sizeof buf);
           
		   for (x = 0; x < ret; x++)
            {
 
//*****************************************************************************
 
			   if (memcmp(buf, "SHELL=", 6) == 0)
                {
                    char *p;
                    
                    memcpy(shell, buf + 6, sizeof shell);
					
                    shell[sizeof shell - 1] = '\0';
					
                    p = strchr(shell, '\r');
					
                    if (p != NULL)
                    {
                       
					   *p = '\0';
					   
                    }
					
                }

//*****************************************************************************
						   
				if (memcmp(buf, "DRIVER=", 7) == 0)
                {
                    char *p;
                    
                    memcpy(driver, buf + 7, sizeof driver);
					
                    driver[sizeof driver - 1] = '\0';
					
                    p = strchr(driver, '\r');
					
                    if (p != NULL)
                    {
                       
					   *p = '\0';
					   
                    }
					
					
                }

//*****************************************************************************				
				
            }
        } 
		
		while (ret == 0x200);
		
        fecharArquivo(conteudo_arquivo_a_ser_lido);
		
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Procedimentos para manipula��o da lista de execut�veis carregados na mem�ria

Fun��o para:
 
  * Adicionar um Aplicativo na lista de execu��o;
  * Remover um Aplicativo da lista de execu��o;
  * Adicionar um Driver de Dispositivo PX-DOS� na lista de execu��o;
  * Remover um Driver de Dispositivo PX-DOS� da lista, removendo-o da mem�ria;
  * Configurar informa��es de execu��o do Driver de Dispositivo PX-DOS� ap�s registro;

*/

static int registrarAPP(char *nomeAPP[12], int tipo, int modelo, int versao, int subversao,
                        unsigned char *endereco, int arquitetura, int salvar, long tamanho, int carregado){

if (salvar == 1){
	
	memcpy(APPS[contAPP].NomeAPP, nomeAPP, 11); // Copia o nome do APP para o registro
	
	}

	if (contAPP >= 1 && APPS[contAPP].Carregado == 0) // A posi��o 1 deve ser a do Shell, primeiro aplicativo carregado
	{ 
	
	contAPP--;
	
	}
	
	if (contAPP >= 0 && APPS[contAPP].Carregado == 1){
	
	contAPP++;
	
	}
	
	APPS[contAPP].TipoAPP=tipo;              // Tipo de execut�vel (APPX ou MZ)
	APPS[contAPP].ModeloAPP=arquitetura;     // Arquitetura para que o aplicativo foi desenvolvido
	APPS[contAPP].VersaoAPP=versao;          // Vers�o m�nima do Sistema necess�ria para a execu��o
	APPS[contAPP].SubversaoAPP=subversao;    // Subers�o m�nima do Sistema necess�ria para a execu��o
	APPS[contAPP].TamanhoAPP=tamanho;        // Tamanho do execut�vel
	APPS[contAPP].Carregado=carregado;       // Indica o status de carregamento do execut�vel

}

/*****************************************************************************************/

static int registrarDRIVER(char *nomeDriver[11], int numerointerrupcao, int tipo, int modelo, int versao, 
                           int subversao, int carregado){


	DRIVER[contDriver].NumeroInterrupcao=numerointerrupcao; // Define o n�mero da interup��o para o registro
	DRIVER[contDriver].TipoDriver=tipo;                     // Define o tipo de Driver para o registro
	DRIVER[contDriver].ModeloDriver=modelo;                 // Define o modelo de Driver para o registro
	DRIVER[contDriver].VersaoDriver=versao;                 // Define a vers�o do Driver para o registro
	DRIVER[contDriver].SubversaoDriver=subversao;           // Define a subvers�o do Driver para o registro
	DRIVER[contDriver].Carregado=carregado;                 // Define o Driver como carregado no registro
   
    memcpy(DRIVER[contDriver].NomeDriver, nomeDriver, 11); // Copia o nome l�gico obtido
	                                                                      // do Driver para o registro


}

/*****************************************************************************************/

static void removerAPP(int ID){

APPS[ID].Carregado=0;

}

/*****************************************************************************************/

static void removerDRIVER(int ID){

DRIVER[ID].Carregado=0;

}

/*****************************************************************************************/

static void configurarDRIVER(int SS, int SP, long tamanho, unsigned char *endereco){

    DRIVER[contDriver].EnderecoDriver=*(char *)&endereco;
    DRIVER[contDriver].TamanhoDriver=tamanho;
	DRIVER[contDriver].SS=SS;
	DRIVER[contDriver].SP=SP;

    contDriver++; // Incrementa o contador

}

/*****************************************************************************************/

static void executarPXConfig(void) // Executa as instru��es encontradas no PXConfig.SIS
{

    static BLOCO_DE_PARAMETROS p = { 0, "\x2/p\r", NULL, NULL };
    static BLOCO_DE_PARAMETROS parametro_pxconfig = { 0, "\x0\r", NULL, NULL };

	carregarExe("CRGDRV.SIS", &p, 1); // Carrega o m�dulo de Drivers do PX-DOS
	
//*****************************************************************************

    if (strcmp(driver, "") != 0)
	{
	
	 validarDriver(driver, &p);  // Verifica se o Driver tem a assinatura PX-DOS
	 
	}

//*****************************************************************************
	
	if (strcmp(shell, "") == 0)
    {
	    
		//memcpy(APPS[contAPP].NomeAPP, "COMANDO.EPX", 11); // Copia o nome do APP para o registro
		//APPS[contAPP].Carregado=1;
        carregarExe("COMANDO.EPX", &p, 1); // Carrega o Shell padr�o
		
    }
	
    else
    {
	    
		//memcpy(APPS[contAPP].NomeAPP, shell, sizeof shell); // Copia o nome do APP para o registro
        carregarExe(shell, &parametro_pxconfig, 1); // Carrega o Shell presente no 
                                                 // 'PXCONFIG.SIS'
		
    }

//*****************************************************************************

	
    return;
	
}

/*


validarDriver - Verifica a presen�a da assinatura do PX-DOS no Driver, o
que indica que o driver foi desenvolvido para o PX-DOS. Caso o Driver
n�o possua essa assinatura ou apresente uma outra, como MZ, de
execut�vel .EXE, ele ser� rejeitado e descarregado da mem�ria.
Essa assinatura deve estar presente no in�cio do Driver, para 
provar a validade do mesmo.


*/

/*****************************************************************************************/

static void validarDriver(char *driver, BLOCO_DE_PARAMETROS *blocodeParametros)

{

   unsigned char primeirobit[20]; // Ir� armazenar o cabe�alho do Driver
   unsigned char nomeDriver[11]; // Ir� armazenar o nome l�gico do arquivo de Driver
   int contArquivo; // Lugar onde o arquivo ser� armazenado em mem�ria
   int tipoDriver; // Tipo de Driver, dentro das especifica��es de desenvolvimento
   static int versaoDriver;
   static int subversaoDriver;
   int numeroInterrupcao; // N�mero da interrup��o a ser utilizada para contatar
                          // o Driver
	
   contArquivo = abrirArquivo(driver); // Abre o arquivo indicado
	
   if (contArquivo < 0) return; // Se n�o p�de ser aberto, finalize a opera��o
	
   lerArquivo(contArquivo, primeirobit, sizeof primeirobit); // Abre o arquivo e l�
                                                               // seu in�cio
  
   if (memcmp(primeirobit, "PX", 2) == 0) // Verifica se o arquivo possui a assinatura de 
	                                       // Driver PX-DOS
    {
	
	// Agora informa��es importantes ser�o extra�das do cabe�alho do Driver
	
	tipoDriver = *(int *)&primeirobit[2];        // Obt�m o tipo do Driver
	versaoDriver = *(int *)&primeirobit[15];     // Obt�m a vers�o do Driver
	subversaoDriver = *(int *)&primeirobit[17];  // Obt�m a subvers�o do Driver
	numeroInterrupcao = *(int *)&primeirobit[4]; // Obt�m o n�mero da interrup��o
	                                             // que dever� ser usada para o contato
												 // com o Driver j� carregado.
    memcpy(nomeDriver, primeirobit + 6, 11);     // Obt�m o nome l�gico do Driver
	
	if (tipoDriver > 12){
	
	 printf("\nO Driver '%s' nao e valido para esta versao do PX-DOS.\n\n", driver);
	 
	 return;
	 
	 }
	 
	 if (tipoDriver == 9){ // Armazena informa��es sobre subsistemas
	 
	 contador_subsistemas+=1;
	 
	 subsistemas[contador_subsistemas]=driver;
	 
	 }
	 
	 
	if (tipoDriver == 8 && hal_instalada != 0){
	
	printf("\nImpossivel carregar dois Drivers deste tipo reservado.\n");
	
	return;
	
	}
	
	driversCarregados += 1; // Aumenta o n�mero de Drivers carregados em 1 na lista
	
	if (tipoDriver == 8){ // Isso indica que um pedido de carregamento da HAL foi pedido
	                      // ao Kernel
	
	hal_instalada= 1;   // Ent�o, nenhuma outra HAL poder� ser carregada. Essa linha
	                      // bloquear� adiante o carregamento de outro Driver com o
						  // c�digo da HAL
						  
	}

    // Realizar registro do Driver na lista
	
	registrarDRIVER(nomeDriver, numeroInterrupcao, tipoDriver, 2, versaoDriver, subversaoDriver, 1);
	
	carregarDriver(driver, &blocodeParametros, contDriver); // Inicia a execu��o do Driver
		
    } 
	
    else 
    {

    printf("\nO arquivo '%s' nao e um Driver valido do PX-DOS.\n\n", driver);

    return;

}

}
 
/*****************************************************************************************/
 
/* 

carregarDriver - Ap�s a verifica��o de compatibilidade, usando a
assinatura de Driver, o mesmo ser� carregado e executado.

O Driver s� ser� executado se cumprir todos os requis�tos
de assinatura e valida��o.


*/ 
 
static void carregarDriver(char *driver, BLOCO_DE_PARAMETROS *blocodeParametros, int contadorDRV)
{
  
    unsigned char primeirobit[10];   
    unsigned int TamanhoCabecalho;
    unsigned char *cabecalho;
    unsigned char *ponteiroAmbiente;
    unsigned long TamanhoDriver;
    unsigned char *tid;
    unsigned char *tidOriginal;
    unsigned char *ambienteOriginal;
    unsigned char *InicioDriver;
    unsigned int ss;
    unsigned int sp;
    unsigned char *EntradaDriver;
    unsigned int PaginasMaximas;
    int contArquivo;
    int ret;
    char *antigoATD;
	
    contArquivo = abrirArquivo(driver);
	
    if (contArquivo < 0) return;
	
    lerArquivo(contArquivo, primeirobit, sizeof primeirobit);
	
   

    ponteiroAmbiente = gerenciamentomemoriaAlocar(&gerenciamentomemoria, tamanhostring(driver) + 9, 0);



    ambienteOriginal = ponteiroAmbiente;
	
    ponteiroAmbiente = (unsigned char *)FP_NORM(ponteiroAmbiente);
	 
   
    memcpy(ponteiroAmbiente, "A=B\0\0", 5);
	
    *((unsigned int *)(ponteiroAmbiente + 5)) = 1; 
	
    memcpy(ponteiroAmbiente + 7, driver, tamanhostring(driver) + 1);

    TamanhoDriver = 0x10000;
	
    PaginasMaximas = gerenciamentomemoriaTamanhoMaximo(&gerenciamentomemoria);
	
    if ((long)PaginasMaximas * 16 < TamanhoDriver)
    {
       
    PXDOS_TelaAzul(14);
		
    printf("\n\nMemoria RAM insuficiente para carregar o Driver indicado.\n");
    printf("\nMemoria necessaria: %ld. Memoria disponivel: %ld.\n\n",
               TamanhoDriver, (long)PaginasMaximas * 16);
			   
    gerenciamentomemoriaLivre(&gerenciamentomemoria, cabecalho);
		
    gerenciamentomemoriaLivre(&gerenciamentomemoria, ponteiroAmbiente);
		
    return;
		
    }
	
    tid = PXDOSAlocarPaginas(&gerenciamentomemoria, PaginasMaximas, 0);
	
    tidOriginal = tid;
	
    tid = (unsigned char *)FP_NORM(tid);

    if (tid == NULL)
    {
	
    printf("\nMemoria RAM insuficiente para carregar o Driver\n");
	printf("de Dispositivo indicado.\n\n");
		
    gerenciamentomemoriaLivre(&gerenciamentomemoria, cabecalho);
		
    gerenciamentomemoriaLivre(&gerenciamentomemoria, ponteiroAmbiente);
		
        return;
		
    }    

    /* Tabela de Informa��es de Driver (TID)
	
	A Tabela de Informa��es de Driver (TID) carregar� consigo informa��es pertinentes
	e blocos em branco que no futuro poder�o ser utilizados pelo conjunto de Drivers
	desenvolvido para o PX-DOS �. Essas informa��es dizem respeito a maneira que o
	Driver ir� operar, quais opera��es executar e informa��es retornar.
	
	*/    
	
    tid[0] = 0xcd;
	
    tid[1] = 0x20;
	

    *(unsigned int *)&tid[0x2c] = FP_SEGMENTO(ponteiroAmbiente);
	
    //	Configura para 640 Kb 
	
    *(unsigned int *)(tid + 2) = FP_SEGMENTO(tid) + PaginasMaximas;
	
    if (blocodeParametros != NULL)
    {
    
	// 1 para contar e 1 para retorno 
	
    memcpy(tid + 0x80, blocodeParametros->fimcmd, blocodeParametros->fimcmd[0] + 1 + 1);
   
   }
    
	InicioDriver = tid + 0x000;
	

    InicioDriver = (unsigned char *)FP_NORM(InicioDriver);
		
    memcpy(InicioDriver, primeirobit, sizeof primeirobit);
		
    lerArquivo(contArquivo, InicioDriver + sizeof primeirobit, 32768);
		
    lerArquivo(contArquivo, FP_NORM(InicioDriver + sizeof primeirobit + 32768), 32768);

    fecharArquivo(contArquivo);


    ss = FP_SEGMENTO(tid);
	
    sp = 0xfffe;
	
    *(unsigned int *)MK_FP(ss, sp) = 0;
		
	EntradaDriver = tid + 0x000; // A origem de um Driver deve ser sempre 0
	
    antigoATD = ATD;
	
    ATD = tid + 0x80;
	
	configurarDRIVER(ss, sp, TamanhoDriver, EntradaDriver); // Configura informa��es complementares do Driver
	
    ret = chamarcompsp(EntradaDriver, tid, ss, sp);
	
    ATD = antigoATD;
	
    tid = tidOriginal;
	
    ponteiroAmbiente = ambienteOriginal;
	
    ultimoCodigo = ret;
	
    gerenciamentomemoriaLivre(&gerenciamentomemoria, tid);
	
    gerenciamentomemoriaLivre(&gerenciamentomemoria, ponteiroAmbiente);
	
    return;
	
	
}

/*****************************************************************************************/

/*
carregarExe - Carrega um execut�vel dentro da mem�ria e o executa
 
 1. L� o cabe�alho e os 10 primeiros bytes 
 2. Aloca espa�o para o cabe�alho 
 3. Carrega o cabe�alho 
 4. Aloca espa�o para vari�veis 
 5. Aloca espa�o para o execut�vel 
 6. Carrega o execut�vel 
 7. Concerta o aplicativo usando a tabela  
 8. Extrai SS + SP do cabe�alho 
 9. Chama execut�vel

 */


static void carregarExe(char *prog, BLOCO_DE_PARAMETROS *blocodeParametros, int SalvarNome)
{
  
    unsigned char primeirobit[24];    
    unsigned int TamanhoCabecalho;
    unsigned char *cabecalho;
    unsigned char *ponteiroAmbiente;
    unsigned long TamanhoExe;
    unsigned char *psp;
    unsigned char *pspOriginal;
    unsigned char *ambienteOriginal;
    unsigned char *InicioExe;
    unsigned int numReloc;
    unsigned long *InicioRelocacao;
    unsigned int relocI;
    unsigned int *CorrigirExe;
    unsigned int adicionarSeg;
    unsigned int ss;
    unsigned int sp;
    unsigned char *EntradaExe;
    unsigned int PaginasMaximas;
    int contArquivo;
    int ret;
	int formato=0;
	int arquiteturaSuportada=0;
	int versaoMinima=0;
	int subversaoMinima=0;
	int tipoPrograma=0;
	int offsetPrograma;
	int alocacaoMinima;
	int alocacaoMaxima;
	int paginasPrograma;
	int valorSS;
	int valorSP;
	int valorIP;
    unsigned char *bss;
    int eExe = 0;
    char *antigoATD;
	
	
    contArquivo = abrirArquivo(prog);
	
    if (contArquivo < 0) return;	                                          
    lerArquivo(contArquivo, primeirobit, sizeof primeirobit);
	
/***********************/
	
    if (memcmp(primeirobit, "MZ", 2) == 0) // Verifica se o arquivo possui a assinatura de 
	                                       // execut�vel DOS
    {
	
	formato=2; // Formato MZ
	
    eExe = 1; // Define que o bin�rio � um execut�vel no formato MZ
		
    TamanhoCabecalho = *(unsigned int *)&primeirobit[8]; // Obt�m o tamanho do cabe�alho
		
    TamanhoCabecalho *= 16; // O multiplica por 6
		
    cabecalho = gerenciamentomemoriaAlocar(&gerenciamentomemoria, TamanhoCabecalho, 0);
		
    memcpy(cabecalho, primeirobit, sizeof primeirobit);  
		
    lerArquivo(contArquivo, cabecalho + sizeof primeirobit, TamanhoCabecalho - sizeof primeirobit);

   
   }   // Fim do reconhecimento e decodifica��o  do formato "MZ"
 
/***********************/
 
   if (memcmp(primeirobit, "APPX", 4) == 0) // Verifica se o arquivo possui a assinatura de
                                            // execut�vel APPX do PX-DOS
    {
    
	formato=1; // Formato APPX
	
	arquiteturaSuportada = *(int *)&primeirobit[4]; // Obt�m a arquitetura de destino
	versaoMinima = *(int *)&primeirobit[5];         // Obt�m a vers�o m�nima requerida
	subversaoMinima = *(int *)&primeirobit[7];      // Obt�m a subvers�o m�nima
	                                         	    // requerida
	valorSS = *(int *)&primeirobit[8];              // Obt�m o valor inicial de SS
    valorSP = *(int *)&primeirobit[10];	            // Obt�m o valor inicial de SP
	valorIP = *(int *)&primeirobit[12];             // Obt�m o valor inicial de IP
	tipoPrograma = *(int *)&primeirobit[14];        // Obt�m o tipo do programa
	offsetPrograma = *(int *)&primeirobit[16];      // Obt�m o offset do programa
	paginasPrograma = *(int *)&primeirobit[18];     // Obt�m o n�mero de p�ginas usadas
	                                                // pelo programa 	  
	alocacaoMinima = *(int *)&primeirobit[20];      // Obt�m a aloca��o m�nima
	alocacaoMaxima = *(int *)&primeirobit[22];      // Obt�m a aloca��o m�xima
	
/*

Agora os dados obtidos do cabe�alho ser�o verificados para garantir a compatibilidade
e os requis�tos dos sistema exigidos pelo programa no formato APPX

*/

	if (arquiteturaSuportada > 7 ){
	
	printf("\nDesculpe, mas o aplicativo APPX nao foi feito para ser executado nesta\n");
	printf("arquitetura. Procure a versao correta.\n");
	
	return;
	
	}
	
	if (versaoMinima > VERSAOPXDOS | subversaoMinima > SUBVERSAOPXDOS) {
	
	printf("\nDesculpe, mas parece que este aplicativo nao foi feito para ser executado\n");
	printf("nesta versao do PX-DOS.\n");
	
	return;
	
	}

	if (tipoPrograma != 144){ // Isso significa que o programa executado n�o
	                          // � o m�dulo principal do pacote. Ent�o, ele n�o
							  // deve ser executado diretamente pelo sistema.
							  
	printf("\nDesculpe, mas este arquivo nao parece ser o aplicativo principal do\n");
	printf("pacote. Procure o aplicativo principal e o execute.\n");
	
	return;
	
	}
	
	if (valorIP != 256) { // Isso significa que o IP foi ajustado apra um valor
	                      // diferente do suportado. A fun��o de IP personalizado ainda
						  // entrar� em fase de testes em pr�ximas vers�es.
						  
    printf("\nDesculpe, mas o arquivo solicitado nao e um aplicativo valido para PX-DOS.\n");
    

    return;

    }	
	
    } // Fim do reconhecimento e decodifica��o do formato "APPX"
   
/***********************/

    ponteiroAmbiente = gerenciamentomemoriaAlocar(&gerenciamentomemoria, tamanhostring(prog) + 9, 0);



    ambienteOriginal = ponteiroAmbiente;
	
    ponteiroAmbiente = (unsigned char *)FP_NORM(ponteiroAmbiente);
   
    memcpy(ponteiroAmbiente, "A=B\0\0", 5);
	
    *((unsigned int *)(ponteiroAmbiente + 5)) = 1; 
	
    memcpy(ponteiroAmbiente + 7, prog, tamanhostring(prog) + 1);


    if (eExe) // Caso seja um arquivo no formato Execut�vel DOS
    {
	
    TamanhoExe = *(unsigned int *)&cabecalho[4];
	
    TamanhoExe = (TamanhoExe + 1) * 512 - TamanhoCabecalho;        
   
   }
   
    else // Caso seja um execut�vel APPX ou bin�rio puro
    {
	
    TamanhoExe = 0x10000;
		
    }
	
    PaginasMaximas = gerenciamentomemoriaTamanhoMaximo(&gerenciamentomemoria);
	
    if ((long)PaginasMaximas * 16 < TamanhoExe)
    {
       
        PXDOS_TelaAzul(7);
		
        printf("\n\nMemoria RAM insuficiente para carregar este programa.\n");
        printf("\nMemoria necessaria: %ld. Memoria disponivel: %ld.\n\n",
               TamanhoExe, (long)PaginasMaximas * 16);
			   
        gerenciamentomemoriaLivre(&gerenciamentomemoria, cabecalho);
		
        gerenciamentomemoriaLivre(&gerenciamentomemoria, ponteiroAmbiente);
		
        return;
		
    }
	
    psp = PXDOSAlocarPaginas(&gerenciamentomemoria, PaginasMaximas, 0);
	
    pspOriginal = psp;
	
    psp = (unsigned char *)FP_NORM(psp);

    if (psp == NULL)
    {
	
        printf("\nMemoria RAM insuficiente para carregar este programa.\n");
		
        gerenciamentomemoriaLivre(&gerenciamentomemoria, cabecalho);
		
        gerenciamentomemoriaLivre(&gerenciamentomemoria, ponteiroAmbiente);
		
        return;
		
    }    

    /* Configura valores para psp */  
	
    psp[0] = 0xcd;
	
    psp[1] = 0x20;
	

    *(unsigned int *)&psp[0x2c] = FP_SEGMENTO(ponteiroAmbiente);
	
    
    //	Configura para 640 Kb 
	
    *(unsigned int *)(psp + 2) = FP_SEGMENTO(psp) + PaginasMaximas;
	
    if (blocodeParametros != NULL)
    {
	
     // 1 para contar e 1 para retorno 
	
        memcpy(psp + 0x80, blocodeParametros->fimcmd, blocodeParametros->fimcmd[0] + 1 + 1);
   
   }
    

    InicioExe = psp + 0x100;
	
    InicioExe = (unsigned char *)FP_NORM(InicioExe);
	
	
    if (eExe)
    {
	
        unsigned int maximaleitura = 32768;
		
        unsigned long leituraTotal = 0;
		
        char *IrPara;
        
        while (leituraTotal < TamanhoExe)
        {
		
            if ((TamanhoExe - leituraTotal) < maximaleitura)
            {
			
                maximaleitura = TamanhoExe - leituraTotal;
            }
			
            IrPara = AbsolutoParaEndereco(EnderecoParaAbsoluto(InicioExe) + leituraTotal);
            IrPara = FP_NORM(IrPara);
			
            lerArquivo(contArquivo, IrPara, maximaleitura);
			
            leituraTotal += maximaleitura;
			
        }
		
    }
	
    else
    {
	
        memcpy(InicioExe, primeirobit, sizeof primeirobit);
		
        lerArquivo(contArquivo, InicioExe + sizeof primeirobit, 32768);
        lerArquivo(contArquivo, FP_NORM(InicioExe + sizeof primeirobit + 32768), 32768);
  
  }

    fecharArquivo(contArquivo);



    if (eExe)
    {
	
        numReloc = *(unsigned int *)&cabecalho[6];
		
        InicioRelocacao = (unsigned long *)(cabecalho + *(unsigned int *)&cabecalho[0x18]);
		
        adicionarSeg = FP_SEGMENTO(InicioExe);
       
	   for (relocI = 0; relocI < numReloc; relocI++)
        {
            
            CorrigirExe = (unsigned int *)
                     ((unsigned long)InicioExe + InicioRelocacao[relocI]);
					 
            *CorrigirExe = *CorrigirExe + adicionarSeg;
			
        }
    
        ss = *(unsigned int *)&cabecalho[0xe];
        ss += adicionarSeg;
        sp = *(unsigned int *)&cabecalho[0x10];

        
        EntradaExe = (unsigned char *)((unsigned long)InicioExe 
                                     + *(unsigned long *)&cabecalho[0x14]);
									 
    }
	
    else // Carregar arquivo bin�rio puro (.com)
    {
	
        ss = FP_SEGMENTO(psp);
		
        sp = 0xfffe;
		
        *(unsigned int *)MK_FP(ss, sp) = 0;
	
        EntradaExe = psp + 0x100; // A origem de um execut�vel bin�rio puro come�a
		                          // em 100h, logo ap�s o PSP.
			
    }

    /*
	
	printf("Entrada do Executavel: %lx, psp: %lx, ss: %x, sp: %x\n", EntradaExe, psp, ss, sp); 
	
	*/
	

    antigoATD = ATD;
	
    ATD = psp + 0x80; // �rea de Transfer�ncia de dados ap�s deslocamento 80h.
	
	// Registro do execut�vel
	
 registrarAPP(prog, formato, tipoPrograma, versaoMinima, subversaoMinima, EntradaExe, arquiteturaSuportada, SalvarNome, TamanhoExe, 1);	
	
    ret = chamarcompsp(EntradaExe, psp, ss, sp);
	
    ATD = antigoATD;
	
    psp = pspOriginal;
	
    ponteiroAmbiente = ambienteOriginal;

    ultimoCodigo = ret;
	
    gerenciamentomemoriaLivre(&gerenciamentomemoria, psp);
	
    gerenciamentomemoriaLivre(&gerenciamentomemoria, ponteiroAmbiente);
	
    return;
	
}

/*****************************************************************************************/

static int biosPARAdrive(int bios)
{

    int drive;
    
    if (bios >= 0x80)
    {
     
	 drive = bios - 0x80 + 2;
   
   }
    
	else
    {
     
	 drive = bios;
   
   }
   
   return (drive);
   
}

/*****************************************************************************************/

static int abrirArquivo(const char *arquivo_nome)
{
    int x;
    const char *p;
    int drive;
    int rc;
    char arquivo_temporario[MAXIMO_ARQUIVOS];

    strcpy(arquivo_temporario, arquivo_nome);
	
    ParaMaiusculo(arquivo_temporario);
	
    arquivo_nome = arquivo_temporario;
	
    p = strchr(arquivo_nome, ':');
	
    if (p == NULL)
    {
     
	 p = arquivo_nome;
    
    drive = drivecorrente;
   
   }
   
    else
    {
    
    drive = *(p - 1);
    
    drive = paramaiusculo(drive) - 'A';
   
   p++;
  
  }
  
    for (x = 0; x < NUM_ARQUIVOS_MAX; x++)
    {
     
	 if (!manipulador_arquivo[x].emuso)
        {
		
            manipulador_arquivo[x].emuso = 1;
			
            manipulador_arquivo[x].especial = 0;
			
            break;
			
        }
    }
	
    if (x == NUM_ARQUIVOS_MAX) return (-1);
	
    rc = abrirarquivofat(&discos[drive].fat, p, &manipulador_arquivo[x].arquivofat);
    
	if (rc != 0) return (-1);
    
	manipulador_arquivo[x].ponteiroFat = &discos[drive].fat;
    
	return (x);
	
}

/*****************************************************************************************/

static int fecharArquivo(int contArquivo)
{

    manipulador_arquivo[contArquivo].emuso = 0;    
    return (0);
	
}

/*****************************************************************************************/

static int lerArquivo(int contArquivo, void *buf, tamanho_t tamanhobuffer)
{

    tamanho_t ret;

    ret = lerarquivofat(manipulador_arquivo[contArquivo].ponteiroFat, &manipulador_arquivo[contArquivo].arquivofat, buf, tamanhobuffer);
	
    return (ret);
	
}

/*****************************************************************************************/

static void acessardisco(int drive)
{

    unsigned char buf[512];
    int rc;
    int setores = 1;
    int trilha = 0;
    int cabeca = 0;
    int setor = 1;
    unsigned char *bpb;

    rc = lerabs(buf, 
                 setores, 
                 drive, 
                 trilha,
                 cabeca,
                 setor);
				 
    if (rc != 0)
    {
    
    return;
    
	}
	
    bpb = buf + 11;
	
    if (discos[drive].acessado)
    {
	
        TerminarFat(&discos[drive].fat);
    
	}
	
    analisarBpb(&discos[drive], bpb);
	
    discos[drive].lba = 0;
	
    padraofat(&discos[drive].fat);
	
    IniciarFat(&discos[drive].fat, bpb, lersetlogico, escreversetlogico, &discos[drive]);
	
    strcpy(discos[drive].diretorio_de_trabalho_atual, "");
	
    discos[drive].acessado = 1;
	
    return;
	
}

/*****************************************************************************************/

static void ParaMaiusculo(char *palavra)
{
    int x;
    
    for (x = 0; palavra[x] != '\0'; x++)
    {
     
	 palavra[x] = paramaiusculo(palavra[x]);
    
	}
	
    return;
	
}

/*****************************************************************************************/

void despejolongo(unsigned long x)
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
	
    despejarbuffer(buf, 8);
	
    return;
	
}

/*****************************************************************************************/

void despejarbuffer(unsigned char *buf, int tamanho)
{

    int x;

    for (x = 0; x < tamanho; x++)
    {
    
    PXDOS_ImprimirTexto(0, buf[x], 0);
    
	}
	
    return;
	
}

/*****************************************************************************************/

static void lersetlogico(void *PonteiroDeDisco, long setor, void *buf)
{

    int trilha;
    int cabeca;
    int set;
    INFODisco *discoinfo;
    int ret;

    discoinfo = (INFODisco *)PonteiroDeDisco;
	
    setor += discoinfo->oculto;
	
    trilha = setor / discoinfo->setores_por_cilindro;
	
    cabeca = setor % discoinfo->setores_por_cilindro;
	
    set = cabeca % discoinfo->setores_por_trilha + 1;
	
    cabeca = cabeca / discoinfo->setores_por_trilha;
	
    if (discoinfo->lba)
    {
    
    ret = lerLBA(buf, 1, discoinfo->drive, setor);
    
	}
    
	else
    {
    
    ret = lerabs(buf, 1, discoinfo->drive, trilha, cabeca, set);
    
	}
    
	if (ret != 0)
    {
	
        PXDOS_TelaAzul(0);

        printf("Falha ao ler setor %ld do disco rigido - Erro no sistema\n", setor);
        printf("\n\nPor favor utilize a combinacao Ctrl+Alt+Del para reiniciar o sistema.\n");
		
        for (;;) ;
    
	}
	
    return;
	
}

/*****************************************************************************************/

static int lerabs(void *buf, 
                   int setores, 
                   int drive, 
                   int trilha, 
                   int cabeca, 
                   int set)
{

    int rc;
    int ret = -1;
    int tentativas;

    void *lerbuffer = buf;


    naousado(setores);
    tentativas = 0;
	
    while (tentativas < 5)
    {
	
        rc = PXDOS_LerSetornoDisco(lerbuffer, 1, drive, trilha, cabeca, set);
        
		if (rc == 0)
        {   
    
            ret = 0;
            break;
			
        }
		
        PXDOS_ResetarDisco(drive);
        tentativas++;
		
    }
	
    return (ret);
	
}

/*****************************************************************************************/

static void PXDOS_TelaAzul(int codigo_erro) 
{

int codigo_erro_final=codigo_erro*(250/16);


 static char *erros_gerais[]= {
 
 "Erro durante a leitura ou gravacao no disco do sistema.",
 "Erro durante o carregamento do sistema.",
 "Erro durante o carregamento de um aplicativo ou arquivo do sistema.",
 "Erro ao acessar o disco do sistema.",
 "Erro ao inicializar algum componente do computador.",
 "Falha geral ao iniciar os discos rigidos disponiveis.",
 "Falha ao acessar algum dispositivo removivel.",
 "Nao existe Memoria RAM livre para ser utilizada.",
 "Processador de comandos do PX-DOS nao encontrado ou incompativel.",
 "Sistema de Arquivos nao suportado pelo sistema.",
 "O aplicativo solicitou uma chamada invalida.",
 "Uma falha desconhecida ocorreu durante a execucao do sistema.",
 "Impossivel executar o aplicativo PX-DOS solicitado.",
 "Aplicativo ou funcao incompativel com o PX-DOS.",
 "Sem memoria para carregar o Driver de Dispositivo indicado.", 
 "O Driver de Dispositivo nao pode ser inicializado devido a incompatibilidades.",
 "O Driver de Dispositivo fez uma chamada invalida e foi finalizado.",
 "Falha na instalacao da implementacao APM do PX-DOS(R).",
 "A interface APM nao esta disponivel para uso no momento.",
 "Computador sem suporte a protocolo APM 1.2.",
 "Falha no gerenciamento APM.",
 "Falha desconhecida no suporte APM."
 
 };

telaazul(); // Inicia a tela na cor azul

        printf("\n\n                         Sistema Operacional PX-DOS");        

        printf("\n\n                     Erro durante a execucao do Sistema\n\n");
    
	
	   printf("\n\nCodigo do Erro: 0x%x\n\n", codigo_erro_final);
	
       printf("Detalhamento do erro:\n"); 
	   
	   printf("\n\n%s\n\n",erros_gerais[codigo_erro]);



}

/*****************************************************************************************/

static void escreversetlogico(void *PonteiroDeDisco, long setor, void *buf)
{
    int trilha;
    int cabeca;
    int setor_escrever;
    INFODisco *infoDisco;
    int ret;

    infoDisco = (INFODisco *)PonteiroDeDisco;
	
    setor += infoDisco->oculto;
	
    trilha = setor / infoDisco->setores_por_cilindro;
	
    cabeca = setor % infoDisco->setores_por_cilindro;
	
    setor_escrever = cabeca % infoDisco->setores_por_trilha + 1;
	
    cabeca = cabeca / infoDisco->setores_por_trilha;
	
    if (infoDisco->lba)
    {
	
        ret = escreverLBA(buf, 1, infoDisco->drive, setor);
		
    }
	
    else
    {
	
        ret = escreverabs(buf, 1, infoDisco->drive, trilha, cabeca, setor_escrever);
  
  }
  
    if (ret != 0)
    {
	
        printf("Falha ao escrever no setor %ld\n", setor);
        for (;;) ;
		
    }
	
    return;
	
}

/*****************************************************************************************/

static int escreverabs(void *buf,
                    int setores,
                    int drive,
                    int trilha,
                    int cabeca,
                    int setor_escrever)
{
    int rc;
    int ret = -1;
    int tentativas;

    void *buf_escrita = buf;

    naousado(setores);

    tentativas = 0;
	
    while (tentativas < 5)
    {
        rc = PXDOS_EscreverSetornoDisco(buf_escrita, 1, drive, trilha, cabeca, setor_escrever);
		
        if (rc == 0)
        {
		
            ret = 0;
			
            break;
			
        }
		
        PXDOS_ResetarDisco(drive);
		
        tentativas++;
    }
	
    return (ret);
	
}

/*****************************************************************************************/

static int escreverLBA(void *buf,
                    int setores,
                    int drive,
                    unsigned long setor)
{
    int rc;
    int ret = -1;
    int tentativas;

    void *writebuf = buf;

    naousado(setores);

    tentativas = 0;
	
    while (tentativas < 5)
    {
        rc = PXDOS_EscreverSetorLBA(writebuf, 1, drive, setor, 0);
		
        if (rc == 0)
		
        {
            ret = 0;
			
            break;
			
        }
		
        PXDOS_ResetarDisco(drive);
		
        tentativas++;
		
    }
	
    return (ret);
	
}


static int lerLBA(void *buf, int setores, int drive, unsigned long setor)
{

    int rc;
    int ret = -1;
    int tentativas;

    void *lerbuffer = buf;


    naousado(setores);
    tentativas = 0;
	
    while (tentativas < 5)
    {
       
	   rc = PXDOS_LerSetoremLBA(lerbuffer, 1, drive, setor, 0);
        
		if (rc == 0)
        {
	 
   
            ret = 0;
            break;
			
        }
       
	   PXDOS_ResetarDisco(drive);
        tentativas++;
		
    }
	
    return (ret);
	
}

/*****************************************************************************************/

static void analisarBpb(INFODisco *discoinfo, unsigned char *bpb)
{

    discoinfo->drive = bpb[25];
	
    discoinfo->numero_cabecas = bpb[15];
	
    discoinfo->oculto = bpb[17]
                       | ((unsigned long)bpb[18] << 8)
                       | ((unsigned long)bpb[19] << 16)
                       | ((unsigned long)bpb[20] << 24);
					   
    discoinfo->setores_por_trilha = (bpb[13] | ((unsigned int)bpb[14] << 8));
	
    discoinfo->setores_por_cilindro = discoinfo->setores_por_trilha 
                                     * discoinfo->numero_cabecas;
									 
    return;
	
}

/*****************************************************************************************/

static void *pxdosalocarmemoria(GERMEMORIA *gerenciamentomemoria, tamanho_t bytes, int id)
{

    tamanho_t paginas;
    
    
    paginas = bytes / 16 + ((bytes & 0x0f) != 0 ? 1 : 0);
	
    return (PXDOSAlocarPaginas(gerenciamentomemoria, paginas, id));
	
}

/*****************************************************************************************/

static void gerenciar_memlivre_PXDOS_16(GERMEMORIA *gerenciamentomemoria, void *ptr)
{

    unsigned long abs;

    abs = EnderecoParaAbsoluto(ptr);
	
    abs -= 0x10000UL;
	
    abs -= (unsigned long)memoriainicial_pxdos * 16;
	
    abs /= 16;
	
    // Ignora requisi��es livres 
	
    if (abs > 0x6000U)
    {
	
        return;
		
    }
	
    abs += (unsigned long)memoriainicial_pxdos * 16;
	
    ptr = AbsolutoParaEndereco(abs);
	
    (gerenciamentomemoriaLivre)(gerenciamentomemoria, ptr);
	
    return;
	
}

/*****************************************************************************************/

static void *PXDOSAlocarPaginas(GERMEMORIA *gerenciamentomemoria, tamanho_t paginas, int id)
{
    void *ptr;
    unsigned long abs;

    // Ponteiro para evitar falhas quando o aplicativo pedir paginas=0 
	
    if (paginas == 0)
    {
	
        paginas = 1;
		
    }
	
    ptr = (gerenciamentomemoriaAlocar)(gerenciamentomemoria, paginas, id);
	
    if (ptr == NULL)
    {
	
        return (ptr);
		
    }
	
    abs = EnderecoParaAbsoluto(ptr);
    
   
    abs -= (unsigned long)memoriainicial_pxdos * 16;
	
    abs *= 16;
	
    abs += (unsigned long)memoriainicial_pxdos * 16;
	
    abs += 0x10000UL;
	
    ptr = AbsolutoParaEndereco(abs);
	
    ptr = FP_NORM(ptr);
	
    return (ptr);
	
}

/*****************************************************************************************/

static void gerenciar_memlivre_16_Paginas(GERMEMORIA *gerenciamentomemoria, tamanho_t pagina)
{

    void *ptr;
    
    ptr = MK_FP(pagina, 0);
	
    gerenciar_memlivre_PXDOS_16(gerenciamentomemoria, ptr);
	
    return;
	
}

/*****************************************************************************************/

static int pxdosrealrealocarpaginas(GERMEMORIA *gerenciamentomemoria, void *ptr, tamanho_t novaspaginas)
{

    unsigned long abs;
    int ret;

    abs = EnderecoParaAbsoluto(ptr);
	
    abs -= 0x10000UL;
	
    abs -= (unsigned long)memoriainicial_pxdos * 16;
	
    abs /= 16;
	
    abs += (unsigned long)memoriainicial_pxdos * 16;
	
    ptr = AbsolutoParaEndereco(abs);
	
    ret = (gerenciamentomemoriaRealocar)(gerenciamentomemoria, ptr, novaspaginas);
	
    return (ret);
	
}


