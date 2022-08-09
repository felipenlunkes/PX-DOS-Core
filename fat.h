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

#ifdef PXLOADER // Caso o m�dulo seja usado pela PXLoader, apenas as fun��es extremamente
                // necess�rias ser�o inclu�das, como iniciar o sistema de arquivos,
				// abrir o arquivo do Kernel, l�-lo e o execut�-lo.

#include <stddef.h>

#define NUM_SETORES_MAX 512

typedef struct {

    unsigned long numero_de_trilhas;
    unsigned int numero_de_cilindros;
    unsigned int numero_de_cabecas;
    unsigned int setores_por_trilha;
    unsigned int tamanho_setor;
    unsigned long setores_por_cilindro;
    unsigned long setores_por_disco;
    unsigned int setores_por_cluster;
    unsigned int numfats;
    unsigned int inicioboot;
    unsigned int inicio_da_fat;
    unsigned int inicioroot;
    unsigned int inicio_arquivo;
    unsigned int drive;
    unsigned char pbuf[NUM_SETORES_MAX];
    unsigned char *buf;
    unsigned int entradasprincipais;
    unsigned int tamanhodirprincipal;
    unsigned int tamanho_fat;
    int fat16;
    unsigned long oculto;
    int naoencontrado;
    void (*lerlogico)(void *ponteirododisco, long setor, void *buf);
    void *parametro;
	
} FAT;

typedef struct {

    int root;
    unsigned int cluster;
    unsigned long TamanhoArquivo;
    unsigned UltimosSetores;
    unsigned UltimosBytes;
    unsigned int ClusterAtual;
    unsigned char datahora[4];
    unsigned int atributo;
    unsigned int ContarSetor;
    unsigned long InicioSetor;
    unsigned int proximoCluster;
    unsigned int BytePara;
    unsigned int SetorPara;
    int dir;
	
} ARQUIVOFAT;

void padraofat(FAT *fat);

void IniciarFat(FAT *fat, 
             unsigned char *bpb,
             void (*lerlogico)(void *ponteirododisco, long setor, void *buf),
             void *parametro);
			 
void TerminarFat(FAT *fat);

int AbrirArquivoFat(FAT *fat, const char *arquivo_nome, ARQUIVOFAT *ARQUIVOFAT);

tamanho_t LerArquivoFat(FAT *fat, ARQUIVOFAT *ARQUIVOFAT, void *buf, tamanho_t tamanhobuffer);


#else // Caso o m�dulo seja usado pelo Kernel do PX-DOS, todas as fun��es de manipula��o
      // do sistema de arquivos devem ser inclu�das


#include <stddef.h>

#define Numero_Maximo_Setores 512
#define ARQUIVO_NAO_ENCONTRADO 0x02
#define DEL 0xE5
#define NUM_ARQUIVOS_MAX 40

typedef struct {
    int root;
    unsigned int cluster;
    unsigned long TamanhoArquivo;
    unsigned int UltimosSetores; 
    unsigned int UltimosBytes; 
    unsigned int ClusterAtual; 
    unsigned char datahora[4];
    unsigned int atributo;
    unsigned int ContadordeSetor; 
    unsigned long InicioSetor; 
    unsigned int ProximoCluster; 
    unsigned int ParaByte;
    unsigned int ParaSetor;
    unsigned long SetorDir; 
    unsigned int OffsetDiretorio;
    unsigned long BytesTotais;
    int dir; 
} ARQUIVOFAT;

typedef struct {
    unsigned char nome_arquivo[8];   
    unsigned char extensao[3]; 
    unsigned char atributos;    
    unsigned char atributos_extras;  
    unsigned char primeirocaractere;       
                                      
    unsigned char hora_criacao[2];    
    unsigned char data_criacao[2];  
    unsigned char ultimo_acesso[2];   
    unsigned char direitos_acesso[2];
    unsigned char hora_modificacao[2];   
    unsigned char data_modificacao[2];  
    unsigned char inicio_cluster[2];  
    unsigned char tamanho_arquivo[4];      
	
}ENTRADADIR;

typedef struct {
    unsigned long numero_de_trilhas;
    unsigned int numero_de_cilindros;
    unsigned int numero_de_cabecas;
    unsigned int setores_por_trilha;
    unsigned int tamanho_do_setor;
    unsigned long setores_por_cilindro;
    unsigned long setores_por_disco;
    unsigned int setores_por_cluster;
    unsigned int bytes_por_cluster;
    unsigned int NumerodeTabelasFAT;
    unsigned int InicioBOOT;
    unsigned int InicioFAT;
    unsigned int inicioRaiz;
    unsigned int InicioArquivo;
    unsigned int drive;
    unsigned char pbuf[Numero_Maximo_Setores];
    unsigned char *buf;
    unsigned int Entradas_na_raiz;
    unsigned int tamanho_da_raiz;
    unsigned int tamanho_da_fat;
    int fat16;
    unsigned long oculto;
    int naoencontrado;
    int operacao; 
    int clustercorrente;
	ENTRADADIR *ed;
    ARQUIVOFAT *arquivoFATcorrente;
    void (*lerLogico)(void *ponteiro_do_disco, long setor, void *buf);
    void (*escreverLogico)(void *ponteiro_do_disco, long setor, void *buf);
    void *parametro;
    char novo_arquivo[12]; 
	unsigned long SetorDIR;
	unsigned char *dbuf;
	const char *maior_que;
	char procura[11];
	int passado;
} FAT;

void padraofat(FAT *fat);
void IniciarFat(FAT *fat,
             unsigned char *bpb,
             void (*lerLogico)(void *ponteiro_do_disco, long setor, void *buf),
             void (*escreverLogico)(void *ponteiro_do_disco, long setor, void *buf),
             void *parametro);
void TerminarFat(FAT *fat);
int criararquivofat(FAT *fat, const char *nome_arquivo_fat, ARQUIVOFAT *arquivo_FAT, int atributo_arquivo_fat);
int AbrirArquivoFat(FAT *fat, const char *nome_arquivo_fat, ARQUIVOFAT *arquivo_FAT);
tamanho_t LerArquivoFat(FAT *fat, ARQUIVOFAT *arquivo_FAT, void *buf, tamanho_t tamanho_buffer);
tamanho_t EscreverArquivoFat(FAT *fat, ARQUIVOFAT *arquivo_FAT, void *buf, tamanho_t tamanho_buffer);
int DeletarArquivoFat(FAT *fat,const char *nome_arquivo_fat); 
int RenomearArquivoFat(FAT *fat,const char *antigo,const char *novo);
int ObterAtributosFat(FAT *fat,const char *nome_arquivo_fat,int *atributo);

#endif