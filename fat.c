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
//

#ifdef PXLOADER // Caso o módulo seja usado pela PXLoader, apenas as funções extremamente
                // necessárias serão incluídas, como iniciar o sistema de arquivos,
				// abrir o arquivo do Kernel, lê-lo e o executá-lo.


#include <string.h>

#include "fat.h"
#include "bios.h"
#include "naousado.h"

static int fimclusterfat(FAT *fat, unsigned int cluster);

static void inicioclusterfat(FAT *fat, const char *arquivo_nome, ARQUIVOFAT *arquivofat);

static void novaprocurafat(FAT *fat, char *procurar, const char **irpara, int *ultimo);

static void procuraprincipalfat(FAT *fat, char *procurar, ARQUIVOFAT *arquivofat);

static void procurardirfat(FAT *fat, char *procurar, ARQUIVOFAT *arquivofat);

static void analisarclusterfat(FAT *fat,
                              unsigned int cluster,
                              unsigned long *InicioCluster,
                              unsigned int *proximoCluster);
							  
static void procurarsetorfat(FAT *fat,
                               char *procurar,
                               ARQUIVOFAT *arquivofat,
                               unsigned long InicioCluster,
                               int numsetores);
							   
static void lerlogicofat(FAT *fat, long setor, void *buf);


/*****************************************************************************************/

/*

Função "padraofat" - Padroniza os valores a serem utilizados

*/
 
void padraofat(FAT *fat)
{

    fat->setores_por_cilindro = 1;
    fat->numero_de_cabecas = 1;
    fat->setores_por_trilha = 1;
    fat->inicioboot = 0;
    fat->inicio_da_fat = 1;
    fat->buf = fat->pbuf;
	
    return;
	
}

/*****************************************************************************************/

/*
 
Função "IniciarFat" - Inicializa o FAT

*/
 
void IniciarFat(FAT *fat, 
             unsigned char *bpb,
             void (*lerlogico)(void *ponteirododisco, long setor, void *buf),
             void *parametro)
{
    fat->lerlogico = lerlogico;
	
    fat->parametro = parametro;
	
    fat->drive = bpb[25];
	
    fat->tamanho_setor = bpb[0] | ((unsigned int)bpb[1] << 8);
	
    fat->setores_por_cluster = bpb[2];
	
    fat->numfats = bpb[5];
	
    fat->tamanho_fat = bpb[11] | ((unsigned int)bpb[12] << 8);
	
    fat->setores_por_disco = bpb[8] | ((unsigned int)bpb[9] << 8);
	
    fat->numero_de_cabecas = bpb[15];
	
    if (fat->setores_por_disco == 0)
    {
	
        fat->setores_por_disco = 
            bpb[21]
            | ((unsigned int)bpb[22] << 8)
            | ((unsigned long)bpb[23] << 16)
            | ((unsigned long)bpb[24] << 24);
			
    }
	
    fat->oculto = bpb[17]
                  | ((unsigned long)bpb[18] << 8)
                  | ((unsigned long)bpb[19] << 16)
                  | ((unsigned long)bpb[20] << 24);
				  
    fat->inicioroot = fat->tamanho_fat * fat->numfats + fat->inicio_da_fat;
	
    fat->entradasprincipais = bpb[6] | ((unsigned int)bpb[7] << 8);
	
    fat->tamanhodirprincipal = fat->entradasprincipais / (fat->tamanho_setor / 32);
	
    fat->setores_por_trilha = (bpb[13] | ((unsigned int)bpb[14] << 8));
	
    fat->inicio_arquivo = fat->inicioroot + fat->tamanhodirprincipal;
	
    fat->numero_de_trilhas = fat->setores_por_disco / fat->setores_por_trilha;
	
    fat->numero_de_cilindros = (unsigned int)(fat->numero_de_trilhas / fat->numero_de_cabecas);
	
    fat->setores_por_cilindro = fat->setores_por_trilha * fat->numero_de_cabecas;
	
    if ((fat->setores_por_disco / fat->setores_por_cluster) < 4087)
    {
	
        fat->fat16 = 0;
		
    }
	
    else
    {
	
        fat->fat16 = 1;
		
    }
	
    return;
}

/*****************************************************************************************/

/*

Função "TerminarFat" - Finaliza a estrutura do Sistema de Arquivos

*/ 

void TerminarFat(FAT *fat)
{

    naousado(fat);
    return;
	
}

/*****************************************************************************************/

static int fimclusterfat(FAT *fat, unsigned int cluster)
{
    if (fat->fat16)
    {
	
        if (cluster >= 0xfff8U)
        {
		
            return (1);
			
        }
    }
	
    else
    {
	
        if (cluster >= 0xff8U)
        {
		
            return (1);
			
        }
    }
	
    return (0);
	
}

/*****************************************************************************************/

int abrirarquivofat(FAT *fat, const char *arquivo_nome, ARQUIVOFAT *arquivofat)
{

    fat->naoencontrado = 0;
	
    if ((arquivo_nome[0] == '\\') || (arquivo_nome[0] == '/'))
    {
	
        arquivo_nome++;
    }
	
    if (strcmp(arquivo_nome, "") == 0)
    {
	
        arquivofat->root = 1;
        arquivofat->proximoCluster = 0xffff;
        arquivofat->ContarSetor = fat->tamanhodirprincipal;
        arquivofat->InicioSetor = fat->inicioroot;
        arquivofat->UltimosBytes = 0;
        arquivofat->UltimosSetores = fat->tamanhodirprincipal;
        arquivofat->cluster = 0;
        arquivofat->dir = 1;
    }
	
    else
    {
	
        arquivofat->root = 0;
        inicioclusterfat(fat, arquivo_nome, arquivofat);
		
        if (fat->naoencontrado) return (-1);
		
        arquivofat->UltimosBytes = (unsigned int)
                             (arquivofat->TamanhoArquivo 
                              % (fat->setores_por_cluster
                                 * fat->tamanho_setor));
								 
        arquivofat->UltimosSetores = arquivofat->UltimosBytes / fat->tamanho_setor;
		
        arquivofat->UltimosBytes = arquivofat->UltimosBytes % fat->tamanho_setor;
		
        arquivofat->dir = (arquivofat->TamanhoArquivo == 0);
		
        analisarclusterfat(fat, 
                          arquivofat->cluster,
                          &arquivofat->InicioSetor,
                          &arquivofat->proximoCluster);
        arquivofat->ContarSetor = fat->setores_por_cluster;
    }
	
    arquivofat->ClusterAtual = arquivofat->cluster;
	
    arquivofat->SetorPara = 0;
	
    arquivofat->BytePara = 0;
	
    if (fat->naoencontrado)
    {
	
        return (-1);   
		
    }
	
    return (0);
	
}

/*****************************************************************************************/

/*

Função "lerarquivofat" - Leitura em um arquivo já aberto

*/
 
tamanho_t lerarquivofat(FAT *fat, ARQUIVOFAT *arquivofat, void *buf, tamanho_t tamanhobuffer)
{
    tamanho_t BytesLidos = 0;
    static unsigned char bbuf[NUM_SETORES_MAX];
    int SetoresDisponiveis;
    int BytesDisponiveis;
    
    BytesDisponiveis = fat->tamanho_setor;
    SetoresDisponiveis = arquivofat->ContarSetor;
	
    while (!fimclusterfat(fat, arquivofat->ClusterAtual))
    {
        SetoresDisponiveis = arquivofat->ContarSetor;
        
		if (fimclusterfat(fat, arquivofat->proximoCluster) && !arquivofat->dir)
        {
		
            SetoresDisponiveis = arquivofat->UltimosSetores + 1;
			
        }
		
        while (arquivofat->SetorPara != SetoresDisponiveis)
        {
		
            BytesDisponiveis = fat->tamanho_setor;
			
            if (fimclusterfat(fat, arquivofat->proximoCluster) && !arquivofat->dir)
            {
			
                if (arquivofat->SetorPara == arquivofat->UltimosSetores)
                {
				
                    BytesDisponiveis = arquivofat->UltimosBytes;
					
                }
            }
			
            while (arquivofat->BytePara != BytesDisponiveis)
            {
			
                lerlogicofat(fat,
                               arquivofat->InicioSetor + arquivofat->SetorPara, 
                               bbuf);
							   
                if ((BytesDisponiveis - arquivofat->BytePara)
                    > (tamanhobuffer - BytesLidos))
                {
				
                    memcpy((char *)buf + BytesLidos, 
                           bbuf + arquivofat->BytePara,
                           tamanhobuffer - BytesLidos);
						   
                    arquivofat->BytePara += (tamanhobuffer - BytesLidos);
					
                    BytesLidos = tamanhobuffer;
					
                    return (BytesLidos);
					
                }
				
                else
                {
				
                    memcpy((char *)buf + BytesLidos,
                           bbuf + arquivofat->BytePara,
                           BytesDisponiveis - arquivofat->BytePara);
						   
                    BytesLidos += (BytesDisponiveis - arquivofat->BytePara);
					
                    arquivofat->BytePara += (BytesDisponiveis - arquivofat->BytePara);
					
                }
            }
			
            arquivofat->SetorPara++;
            arquivofat->BytePara = 0;
        }
		
        arquivofat->ClusterAtual = arquivofat->proximoCluster;
		
        analisarclusterfat(fat, 
                          arquivofat->ClusterAtual, 
                          &arquivofat->InicioSetor,
                          &arquivofat->proximoCluster);
						  
        arquivofat->SetorPara = 0;
    }
	
    return (BytesLidos);
	
}

/*****************************************************************************************/

/*

Função "inicioclusterfat" - Obtêm o nome e o cluster de início

*/

static void inicioclusterfat(FAT *fat, const char *arquivo_nome, ARQUIVOFAT *arquivofat)
{
    char procurar[11];
    const char *irpara = arquivo_nome;
    int ultimo;

    arquivofat->cluster = 0;
	
    novaprocurafat(fat, procurar, &irpara, &ultimo);
	
    if (fat->naoencontrado) return;
	
    procuraprincipalfat(fat, procurar, arquivofat);
	
    while (!ultimo)
    {
	
        novaprocurafat(fat, procurar, &irpara, &ultimo);
		
        if (fat->naoencontrado) return;
		
        procurardirfat(fat, procurar, arquivofat);
		
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Função "novaprocurafat" - Obtêm o próximo setor do arquivo a ser acessado
 
 * Entradas:
 * fat - ponteiro para o objeto Fat utilizado
 * *irpara - o bit de nome para procurar o arquivo
 * 
 * Saídas:
 * *irpara - O bit com o nome pra procurar mais tarde
 * procurar - porção do nome encontrado no diretório root
 *
 * ex: caso o nome seja \FELIPE\SHEYLA\JOAO.TXT e
 * *irpara aponte para SHEYLA... a procura será por
 * "SHEYLA            " e depois *irpara apontará para "JOAO...".
 
 */

static void novaprocurafat(FAT *fat, char *procurar, const char **irpara, int *ultimo)
{
    const char *p;
    const char *q;

    naousado(fat);    
    p = strchr(*irpara, '\\');
    q = strchr(*irpara, '/');
	
    if (p == NULL)
    {
	
        p = q;
		
    }
	
    else if (q != NULL)
    {
	
        if (q < p)
        {
		
            p = q;
			
        }
    }
	
    if (p == NULL)
    {
	
        p = *irpara + tamanhostring(*irpara);
        *ultimo = 1;
		
    }
	
    else
    {
	
        *ultimo = 0;
		
    }
	
    if ((p - *irpara) > 12)
    {
	
        fat->naoencontrado = 1;
        return;
		
    }
	
    q = memchr(*irpara, '.', p - *irpara);
   
   if (q != NULL)
    {
	
        if ((q - *irpara) > 8)
        {
		
            fat->naoencontrado = 1;
            return;
			
        }
		
        if ((p - q) > 4)
        {
		
            fat->naoencontrado = 1;
            return;
			
        }
		
        memcpy(procurar, *irpara, q - *irpara);
        memset(procurar + (q - *irpara), ' ', 8 - (q - *irpara));
        memcpy(procurar + 8, q + 1, p - q - 1);
        memset(procurar + 8 + (p - q) - 1, ' ', 3 - ((p - q) - 1));
		
    }
	
    else
    {
	
        memcpy(procurar, *irpara, p - *irpara);
        memset(procurar + (p - *irpara), ' ', 11 - (p - *irpara));
		
    }
	
    if (*ultimo)
    {
	
        *irpara = p;
		
    }
	
    else
    {
	
        *irpara = p + 1;
		
    }
	
    return;
	
}

/*****************************************************************************************/

/*
Função "procuraprincipalfat" - Procura pela entrada no diretório root do disco

*/
 
static void procuraprincipalfat(FAT *fat, char *procurar, ARQUIVOFAT *arquivofat)
{

    procurarsetorfat(fat, procurar, arquivofat, fat->inicioroot, fat->tamanhodirprincipal);
	
    return;
	
}

/*****************************************************************************************/

/*

Função "procurardirfat" - Procura no diretório o arquivo

*/
 
static void procurardirfat(FAT *fat, char *procurar, ARQUIVOFAT *arquivofat)
{
    unsigned long InicioCluster;
    unsigned int proximoCluster;
    
    analisarclusterfat(fat, arquivofat->cluster, &InicioCluster, &proximoCluster);
    procurarsetorfat(fat, 
                       procurar, 
                       arquivofat, 
                       InicioCluster, 
                       fat->setores_por_cluster);
					   
    while (arquivofat->cluster == 0)    /* Não encontrado. Processo não terminado */
    {
        if (fimclusterfat(fat, proximoCluster))
        {
		
            fat->naoencontrado = 1;
            return;
			
        }
		
        arquivofat->cluster = proximoCluster;
		
        analisarclusterfat(fat, arquivofat->cluster, &InicioCluster, &proximoCluster);
		
        procurarsetorfat(fat, 
                           procurar, 
                           arquivofat,
                           InicioCluster,
                           fat->setores_por_cluster);
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Função "analisarclusterfat" - Obtêm o número do cluster e calcula o início 
       do cluster na cadeia
	   
*/
 
static void analisarclusterfat(FAT *fat,
                              unsigned int cluster,
                              unsigned long *InicioCluster,
                              unsigned int *proximoCluster)
{
    unsigned int SetorFat;
    static unsigned char buf[NUM_SETORES_MAX];
    int deslocamento;
    
    *InicioCluster = (cluster - 2) * (long)fat->setores_por_cluster
                   + fat->inicio_arquivo;
    if (fat->fat16)
    {
	
        SetorFat = fat->inicio_da_fat + (cluster * 2) / fat->tamanho_setor;
        lerlogicofat(fat, SetorFat, buf);
        deslocamento = (cluster * 2) % fat->tamanho_setor;
        *proximoCluster = buf[deslocamento] | ((unsigned int)buf[deslocamento + 1] << 8);
		
    }
	
    else
    {
	
        SetorFat = fat->inicio_da_fat + (cluster * 3 / 2) / fat->tamanho_setor;
		
        lerlogicofat(fat, SetorFat, buf);
		
        deslocamento = (cluster * 3 / 2) % fat->tamanho_setor;
		
        *proximoCluster = buf[deslocamento];
		
        if (deslocamento == (fat->tamanho_setor - 1))
        {
		
            lerlogicofat(fat, SetorFat + 1, buf);
            deslocamento = -1;
			
        }
		
        *proximoCluster = *proximoCluster | ((unsigned int)buf[deslocamento + 1] << 8);
       
	   if ((cluster * 3 % 2) == 0)
        {
		
            *proximoCluster = *proximoCluster & 0xfffU;
			
        }
		
        else
        {
		
            *proximoCluster = *proximoCluster >> 4;
			
        }
    }
	
    return;
	
}                   

/*****************************************************************************************/

/*

Função "procurarsetorfat" - Procura por um setor específico

*/

static void procurarsetorfat(FAT *fat,
                               char *procurar,
                               ARQUIVOFAT *arquivofat,
                               unsigned long InicioCluster,
                               int numsetores)
{
    int x;
    unsigned char buf[NUM_SETORES_MAX];
    unsigned char *p;
    
    for (x = 0; x < numsetores; x++)
    {
	
        lerlogicofat(fat, InicioCluster + x, buf);
        
		for (p = buf; p < buf + fat->tamanho_setor; p += 32)
        {            
           
		   if (memcmp(p, procurar, 11) == 0)
            {
               
			   arquivofat->cluster = p[0x1a] | (p[0x1a + 1] << 8);
			   
                arquivofat->TamanhoArquivo = p[0x1c]
                                    | ((unsigned int)p[0x1c + 1] << 8)
                                    | ((unsigned long)p[0x1c + 2] << 16)
                                    | ((unsigned long)p[0x1c + 3] << 24);
									
                arquivofat->atributo = p[0x0b];
				
                memcpy(arquivofat->datahora, &p[0x16], 4);
				
                return;
            }
			
            else if (*p == '\0')
            {
			
                fat->naoencontrado = 1;
                return;
				
            }
        }
    }
	
    arquivofat->cluster = 0;
    return;
	
}

/*****************************************************************************************/

/*

Função "lerlogicofat" - Lê um setor lógico do disco

*/
 
static void lerlogicofat(FAT *fat, long setor, void *buf)
{

    fat->lerlogico(fat->parametro, setor, buf);
	
    return;
	
}


#else // Caso o módulo seja usado pelo Kernel do PX-DOS, todas as funções de manipulação
      // do sistema de arquivos devem ser incluídas


#include <string.h>

#include "fat.h"
#include "bios.h"
#include "naousado.h"

#define DELETAR_FAT 1
#define RENOMEAR_FAT 2

static int fimclusterfat(FAT *fat, unsigned int cluster);

static void inicioclusterfat(FAT *fat, const char *nome_arquivo_fat);

static void novaprocurafat(FAT *fat, char *pesquisar, const char **pesquisa, int *Ultimo);

static void procuraprincipalfat(FAT *fat, char *pesquisar);

static void procuradirfat(FAT *fat, char *pesquisar);

static void atualizarraizfat(FAT *fat, char *pesquisar, ARQUIVOFAT *arquivo_FAT);

static void analisarclusterfat(FAT *fat,
                              unsigned int cluster,
                              unsigned long *InicioSetor,
                              unsigned int *ProximoCluster);
							  
static void encontrarsetordirfat(FAT *fat,
                               char *pesquisar,
                               unsigned long InicioSetor,
                               int Numero_de_Setores);
							   
static void atualizarsetordirfat(FAT *fat,
                               char *pesquisar,
                               ARQUIVOFAT *arquivo_FAT,
                               unsigned long InicioSetor,
                               int Numero_de_Setores);
							   
static void lerlogicofat(FAT *fat, long setor, void *buf);

static void escreverlogicofat(FAT *fat, long setor, void *buf);

static void marcarclusterfat(FAT *fat, unsigned int cluster);

static unsigned int encontrarclusterlivre(FAT *fat);

static void cadeiafat(FAT *fat, ARQUIVOFAT *arquivo_FAT);

static void ZerarClusterfat(FAT *fat, unsigned int cluster);

/*****************************************************************************************/

/*

Função "padraofat" - Inicia o sistema de arquivos diante do sistema
 
*/

void padraofat(FAT *fat)
{
    fat->setores_por_cilindro = 1;
    fat->numero_de_cabecas = 1;
    fat->setores_por_trilha = 1;
    fat->InicioBOOT = 0;
    fat->InicioFAT = 1;
    fat->buf = fat->pbuf;
    fat->operacao=0;
	
    return;
}

/*****************************************************************************************/

/*

Função "IniciarFat" - Inicializa os manipuladores de arquivo
 
*/

void IniciarFat(FAT *fat,
             unsigned char *bpb,
             void (*lerLogico)(void *ponteiro_do_disco, long setor, void *buf),
             void (*escreverLogico)(void *ponteiro_do_disco, long setor, void *buf),
             void *parametro)
{

    fat->lerLogico = lerLogico;
	
    fat->escreverLogico = escreverLogico;
	
    fat->parametro = parametro;
	
    fat->drive = bpb[25];
	
    fat->tamanho_do_setor = bpb[0] | ((unsigned int)bpb[1] << 8);
	
    fat->setores_por_cluster = bpb[2];
	
    fat->bytes_por_cluster = fat->tamanho_do_setor * fat->setores_por_cluster;
	
    fat->NumerodeTabelasFAT = bpb[5];
	
    fat->tamanho_da_fat = bpb[11] | ((unsigned int)bpb[12] << 8);
    fat->setores_por_disco = bpb[8] | ((unsigned int)bpb[9] << 8);
    fat->numero_de_cabecas = bpb[15];
	
    if (fat->setores_por_disco == 0)
    {
	
        fat->setores_por_disco =
            bpb[21]
            | ((unsigned int)bpb[22] << 8)
            | ((unsigned long)bpb[23] << 16)
            | ((unsigned long)bpb[24] << 24);
			
    }
	
    fat->oculto = bpb[17]
                  | ((unsigned long)bpb[18] << 8)
                  | ((unsigned long)bpb[19] << 16)
                  | ((unsigned long)bpb[20] << 24);
				  
    fat->inicioRaiz = fat->tamanho_da_fat * fat->NumerodeTabelasFAT + fat->InicioFAT;
	
    fat->Entradas_na_raiz = bpb[6] | ((unsigned int)bpb[7] << 8);
	
    fat->tamanho_da_raiz = fat->Entradas_na_raiz / (fat->tamanho_do_setor / 32);
	
    fat->setores_por_trilha = (bpb[13] | ((unsigned int)bpb[14] << 8));
	
    fat->InicioArquivo = fat->inicioRaiz + fat->tamanho_da_raiz;
	
    fat->numero_de_trilhas = fat->setores_por_disco / fat->setores_por_trilha;
	
    fat->numero_de_cilindros = (unsigned int)(fat->numero_de_trilhas / fat->numero_de_cabecas);
	
    fat->setores_por_cilindro = fat->setores_por_trilha * fat->numero_de_cabecas;
	
    if ((fat->setores_por_disco / fat->setores_por_cluster) < 4087)
    {
	
        fat->fat16 = 0;
		
    }
	
    else
	
    {
	
        fat->fat16 = 1;
		
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Função "TerminarFat" - Finaliza o manipulador de arquivos
 
*/

void TerminarFat(FAT *fat)
{
    naousado(fat);
	
    return;
}

/*****************************************************************************************/

/*

Função "fimclusterfat" - Determina quando um cluster indica outro cluster
        para a continuação do arquivo ou que o arquivo terminou
 
*/

static int fimclusterfat(FAT *fat, unsigned int cluster)
{
    if (fat->fat16)
    {
	
        if (cluster >= 0xfff8U)
        {
		
            return (1);
			
        }
    }
	
    else
    {
	
        if ((cluster >= 0xff8U) || (cluster == 0xff0U))
        {
		
            return (1);
			
        }
    }
	
    return (0);
	
}

/*****************************************************************************************/

/* Os retornos podem ser negativos */

/*

Função "criararquivofat" - Cria um arquivo, adicionando seu índice na tabela

*/

int criararquivofat(FAT *fat, const char *nome_arquivo_fat, ARQUIVOFAT *arquivo_FAT, int atributo_arquivo_a_ser_criado)
{
    char pesquisar[11];
    const char *pesquisa;
    int Ultimo;
    int root = 0;

    fat->naoencontrado = 0;
    if ((nome_arquivo_fat[0] == '\\') || (nome_arquivo_fat[0] == '/'))
    {
	
        nome_arquivo_fat++;
		
    }
	
    pesquisa = nome_arquivo_fat;
	
    fat->clustercorrente = 0;
	
    novaprocurafat(fat, pesquisar, &pesquisa, &Ultimo);
	
    if (fat->naoencontrado) return (-3);
	
    if (!Ultimo)
    {
	
        procuraprincipalfat(fat, pesquisar);
        if (fat->naoencontrado) return (-3);
		
    }
	
    else
    {
	
        root = 1;
		
    }
	
    while (!Ultimo)
    {
        novaprocurafat(fat, pesquisar, &pesquisa, &Ultimo);
        if (fat->naoencontrado) return (-3);
		
        if (!Ultimo)
        {
		
            procuradirfat(fat, pesquisar);
            if (fat->naoencontrado) return (-3);
			
        }
    }
	
    if (root)
    {
	
        atualizarraizfat(fat, pesquisar, arquivo_FAT);
    
	}
    else
    {
        
    }
	
    return (0);
	
}

/*****************************************************************************************/

int AbrirArquivoFat(FAT *fat, const char *nome_arquivo_fat, ARQUIVOFAT *arquivo_FAT)
{
    fat->naoencontrado = 0;
    fat->arquivoFATcorrente=arquivo_FAT;
	
    if ((nome_arquivo_fat[0] == '\\') || (nome_arquivo_fat[0] == '/'))
    {
	
        nome_arquivo_fat++;
		
    }
	
    if (strcmp(nome_arquivo_fat, "") == 0)
    {
	
        arquivo_FAT->root = 1;
        arquivo_FAT->ProximoCluster = 0xffff;
        arquivo_FAT->ContadordeSetor = fat->tamanho_da_raiz;
        arquivo_FAT->InicioSetor = fat->inicioRaiz;
        arquivo_FAT->UltimosBytes = 0;
        arquivo_FAT->UltimosSetores = fat->tamanho_da_raiz;
        fat->clustercorrente = 0;
        arquivo_FAT->dir = 1;
        arquivo_FAT->atributo=0x10;
		
    }
	
    else
	
    {
	
        arquivo_FAT->root = 0;
        inicioclusterfat(fat, nome_arquivo_fat);
		
        if (fat->naoencontrado) return (2);
		
        arquivo_FAT->UltimosBytes = (unsigned int)
                             (arquivo_FAT->TamanhoArquivo
                              % (fat->setores_por_cluster
                                 * fat->tamanho_do_setor));
								 
        arquivo_FAT->UltimosSetores = arquivo_FAT->UltimosBytes / fat->tamanho_do_setor;
		
        arquivo_FAT->UltimosBytes = arquivo_FAT->UltimosBytes % fat->tamanho_do_setor;
		
        arquivo_FAT->dir = (arquivo_FAT->TamanhoArquivo == 0);
		
        analisarclusterfat(fat,
                          fat->clustercorrente,
                          &arquivo_FAT->InicioSetor,
                          &arquivo_FAT->ProximoCluster);
						  
        arquivo_FAT->ContadordeSetor = fat->setores_por_cluster;
		
    }
	
    arquivo_FAT->ClusterAtual = fat->clustercorrente;
	
    arquivo_FAT->ParaSetor = 0;
	
    arquivo_FAT->ParaByte = 0;
	
    if (fat->naoencontrado)
    {
	
        return (2);
		
    }
	
    return (0);
	
}

/*****************************************************************************************/

/*

Função "LerArquivoFat" - Ler de um arquivo já aberto pelo sistema
 
*/

tamanho_t LerArquivoFat(FAT *fat, ARQUIVOFAT *arquivo_FAT, void *buf, tamanho_t tamanho_buffer)
{
    tamanho_t bytesLidos = 0;
    static unsigned char buffer_contado[Numero_Maximo_Setores];
    int BytesDisponiveis;
    int SetoresDisponiveis;

  
    while (!fimclusterfat(fat, arquivo_FAT->ClusterAtual))
    {
       
        SetoresDisponiveis = arquivo_FAT->ContadordeSetor;

        if (fimclusterfat(fat, arquivo_FAT->ProximoCluster) && !arquivo_FAT->dir)
        {
            /* Último cluster */
			
            if ((arquivo_FAT->UltimosSetores == 0) && (arquivo_FAT->UltimosBytes == 0))
            {
               
			   
            }
			
            else
            {
                /* Reduzir os setores disponíveis */
				
                SetoresDisponiveis = arquivo_FAT->UltimosSetores + 1;
            }
        }

     
	 
        while (arquivo_FAT->ParaSetor != SetoresDisponiveis)
        {
         
		 
            BytesDisponiveis = fat->tamanho_do_setor;

           
            if (fimclusterfat(fat, arquivo_FAT->ProximoCluster) && !arquivo_FAT->dir)
            {
               
                if (arquivo_FAT->ParaSetor == arquivo_FAT->UltimosSetores)
                {
                  
                    if ((arquivo_FAT->UltimosSetores == 0)
                        && (arquivo_FAT->UltimosBytes == 0))
                    {
                      
                    }
                    else
                    {
                        BytesDisponiveis = arquivo_FAT->UltimosBytes;
                    }
                }
            }
           
            while (arquivo_FAT->ParaByte != BytesDisponiveis)
            {
            
                lerlogicofat(fat,
                               arquivo_FAT->InicioSetor + arquivo_FAT->ParaSetor,
                               buffer_contado);
              
                if ((BytesDisponiveis - arquivo_FAT->ParaByte)
                    > (tamanho_buffer - bytesLidos))
                {
                    memcpy((char *)buf + bytesLidos,
                           buffer_contado + arquivo_FAT->ParaByte,
                           tamanho_buffer - bytesLidos);
						   
                    arquivo_FAT->ParaByte += (tamanho_buffer - bytesLidos);
					
                    bytesLidos = tamanho_buffer;
					
                    return (bytesLidos);
					
                }
				
                else
                {
                    memcpy((char *)buf + bytesLidos,
                           buffer_contado + arquivo_FAT->ParaByte,
                           BytesDisponiveis - arquivo_FAT->ParaByte);
                    bytesLidos += (BytesDisponiveis - arquivo_FAT->ParaByte);
                    arquivo_FAT->ParaByte += (BytesDisponiveis - arquivo_FAT->ParaByte);
                }
            }
			
            arquivo_FAT->ParaSetor++;
            arquivo_FAT->ParaByte = 0;
			
        }
		
        arquivo_FAT->ClusterAtual = arquivo_FAT->ProximoCluster;
		
        analisarclusterfat(fat,
                          arquivo_FAT->ClusterAtual,
                          &arquivo_FAT->InicioSetor,
                          &arquivo_FAT->ProximoCluster);
						  
        arquivo_FAT->ParaSetor = 0;
    }
	
    return (bytesLidos);
	
}

/*****************************************************************************************/

/*

Função "EscreverArquivoFat" - Escrever em um arquivo já aberto
 
*/

tamanho_t EscreverArquivoFat(FAT *fat, ARQUIVOFAT *arquivo_FAT, void *buf, tamanho_t tamanho_buffer)
{
    static unsigned char buffer_contado[Numero_Maximo_Setores];
    tamanho_t bytes_disponiveis_no_setor; /* bytes disponíveis no setor */
    tamanho_t tamanho_temporario; /* tamanho temporário */
    tamanho_t pronto; 

   

    if (tamanho_buffer == 0) return (0);

    bytes_disponiveis_no_setor = fat->tamanho_do_setor - arquivo_FAT->UltimosBytes;

    
    if ((arquivo_FAT->UltimosBytes != 0) && (bytes_disponiveis_no_setor != 0))
    {
	
        lerlogicofat(fat,
                       arquivo_FAT->InicioSetor + arquivo_FAT->ParaSetor,
                       buffer_contado);
					   
    }

  
    if (tamanho_buffer <= bytes_disponiveis_no_setor)
    {
        memcpy(buffer_contado + arquivo_FAT->UltimosBytes,
               buf,
               tamanho_buffer);
			   
        escreverlogicofat(fat,
                        arquivo_FAT->InicioSetor + arquivo_FAT->ParaSetor,
                        buffer_contado);
						
        arquivo_FAT->UltimosBytes += tamanho_buffer;
        arquivo_FAT->BytesTotais += tamanho_buffer;
		
    }
	
    else
    {
      
        if (bytes_disponiveis_no_setor != 0)
        {
            memcpy(buffer_contado + arquivo_FAT->UltimosBytes,
                   buf,
                   bytes_disponiveis_no_setor);
				   
            escreverlogicofat(fat,
                            arquivo_FAT->InicioSetor + arquivo_FAT->ParaSetor,
                            buffer_contado);
							
        }
		
        pronto = bytes_disponiveis_no_setor; 
        tamanho_temporario = tamanho_buffer - bytes_disponiveis_no_setor;
        arquivo_FAT->ParaSetor++;
        
        if (arquivo_FAT->ParaSetor == fat->setores_por_cluster)
        {
		
            cadeiafat(fat, arquivo_FAT);
			
        }
        
        while (tamanho_temporario > fat->tamanho_do_setor)
        {
		
            memcpy(buffer_contado, (char *)buf + pronto, fat->tamanho_do_setor);
			
            escreverlogicofat(fat,
                            arquivo_FAT->InicioSetor + arquivo_FAT->ParaSetor,
                            buffer_contado);
							
            arquivo_FAT->ParaSetor++;

            /* Ir para um próximo cluster quando pronto */
			
            if (arquivo_FAT->ParaSetor == fat->setores_por_cluster)
            {
			
                cadeiafat(fat, arquivo_FAT);
				
            }
			
            tamanho_temporario -= fat->tamanho_do_setor;
            pronto += fat->tamanho_do_setor;
			
        }
		
        memcpy(buffer_contado, (char *)buf + pronto, tamanho_temporario);
		
        escreverlogicofat(fat,
                        arquivo_FAT->InicioSetor + arquivo_FAT->ParaSetor,
                        buffer_contado);
        
        arquivo_FAT->UltimosBytes = tamanho_temporario;

        
        arquivo_FAT->BytesTotais += tamanho_buffer;
		
    }

    
    lerlogicofat(fat,
                   arquivo_FAT->SetorDir,
                   buffer_contado);
				   
    buffer_contado[arquivo_FAT->OffsetDiretorio + 0x1c] = arquivo_FAT->BytesTotais & 0xff;
	
    buffer_contado[arquivo_FAT->OffsetDiretorio + 0x1c + 1]
        = (arquivo_FAT->BytesTotais >> 8) & 0xff;
		
    buffer_contado[arquivo_FAT->OffsetDiretorio + 0x1c + 2]
        = (arquivo_FAT->BytesTotais >> 16) & 0xff;
		
    buffer_contado[arquivo_FAT->OffsetDiretorio + 0x1c + 3]
        = (arquivo_FAT->BytesTotais >> 24) & 0xff;
		
    escreverlogicofat(fat,
                    arquivo_FAT->SetorDir,
                    buffer_contado);
					
    return (tamanho_buffer);
}

/*****************************************************************************************/

/*

Função "inicioclusterfat" - obtêm o nome e retorna o primeiro cluster e iguala
                            naoencontrado a quando não encontrado
 
*/

static void inicioclusterfat(FAT *fat, const char *nome_arquivo_fat)
{
    char pesquisar[11];
    const char *pesquisa = nome_arquivo_fat;
    int Ultimo;

    fat->clustercorrente = 0;
	
    novaprocurafat(fat, pesquisar, &pesquisa, &Ultimo);
	
    if (fat->naoencontrado) return;
	
    procuraprincipalfat(fat, pesquisar);
	
    while (!Ultimo)
	
    {
        novaprocurafat(fat, pesquisar, &pesquisa, &Ultimo);
		
        if (fat->naoencontrado) return;
		
        procuradirfat(fat, pesquisar);
		
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Função "novaprocurafat" - Obtêm o próximo setor do arquivo a ser acessado
 
 * Entradas:
 * fat - ponteiro para o objeto Fat utilizado
 * *irpara - o bit de nome para procurar o arquivo
 * 
 * Saídas:
 * *irpara - O bit com o nome pra procurar mais tarde
 * procurar - porção do nome encontrado no diretório root
 *
 * ex: caso o nome seja \FELIPE\SHEYLA\JOAO.TXT e
 * *irpara aponte para SHEYLA... a procura será por
 * "SHEYLA            " e depois *irpara apontará para "JOAO...".
 
 */

static void novaprocurafat(FAT *fat, char *pesquisar, const char **pesquisa, int *Ultimo)
{
    const char *p;
    const char *q;

    naousado(fat);
    p = strchr(*pesquisa, '\\');
    q = strchr(*pesquisa, '/');
	
    if (p == NULL)
    {
	
        p = q;
		
    }
	
    else if (q != NULL)
    {
	
        if (q < p)
        {
		
            p = q;
			
        }
    }
	
    if (p == NULL)
    {
	
        p = *pesquisa + tamanhostring(*pesquisa);
        *Ultimo = 1;
		
    }
	
    else
    {
	
        *Ultimo = 0;
		
    }
	
    if ((p - *pesquisa) > 12)
    {
       
	   fat->naoencontrado = 1;
       return;
		
    }
	
    q = memchr(*pesquisa, '.', p - *pesquisa);
	
    if (q != NULL)
    {
	
        if ((q - *pesquisa) > 8)
        {
		
            fat->naoencontrado = 1;
            return;
			
        }
		
        if ((p - q) > 4)
        {
		
            fat->naoencontrado = 1;
            return;
			
        }
		
        memcpy(pesquisar, *pesquisa, q - *pesquisa);
        memset(pesquisar + (q - *pesquisa), ' ', 8 - (q - *pesquisa));
        memcpy(pesquisar + 8, q + 1, p - q - 1);
        memset(pesquisar + 8 + (p - q) - 1, ' ', 3 - ((p - q) - 1));
    }
	
    else
    {
	
        memcpy(pesquisar, *pesquisa, p - *pesquisa);
        memset(pesquisar + (p - *pesquisa), ' ', 11 - (p - *pesquisa));
		
    }
	
    if (*Ultimo)
    {
	
        *pesquisa = p;
		
    }
	
    else
    {
	
        *pesquisa = p + 1;
		
    }
	
    return;
	
}

/*****************************************************************************************/

static void PosicaoFat(FAT *fat, const char *nome_arquivo)
{
    fat->maior_que = nome_arquivo;
    fat->clustercorrente = 0;
    novaprocurafat(fat, fat->procura, 0, &fat->maior_que);
	
    if (fat->naoencontrado) return;
	
    procuraprincipalfat(fat, fat->procura);
	
    while (!fat->passado)
    {
	
        novaprocurafat(fat, fat->procura, 0, &fat->maior_que);
		
        if (fat->naoencontrado) return;
		
        procuradirfat(fat, fat->procura);
		
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Função "procuraprincipalfat" - Procura pela entrada no diretório root do disco
 
*/
 

static void procuraprincipalfat(FAT *fat, char *pesquisar)
{

    encontrarsetordirfat(fat, pesquisar,fat->inicioRaiz, fat->tamanho_da_raiz);
	
    return;
	
}

/*****************************************************************************************/

/*

Função "procurardirfat" - Procura no diretório o arquivo
 
*/

static void procuradirfat(FAT *fat, char *pesquisar)
{
    unsigned long InicioSetor;
    unsigned int ProximoCluster;

    analisarclusterfat(fat, fat->clustercorrente, &InicioSetor, &ProximoCluster);
	
    encontrarsetordirfat(fat,
                       pesquisar,
                       InicioSetor,
                       fat->setores_por_cluster);
					   
    while (fat->clustercorrente == 0)    /* Não encontrado mas não é o fim */
    {
        if (fimclusterfat(fat, ProximoCluster))
        {
		
            fat->naoencontrado = 1;
            return;
			
        }
		
        fat->clustercorrente = ProximoCluster;
		
        analisarclusterfat(fat, fat->clustercorrente, &InicioSetor, &ProximoCluster);
		
        encontrarsetordirfat(fat,
                           pesquisar,
                           InicioSetor,
                           fat->setores_por_cluster);
						   
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Função "atualizarraizfat" - atualiza o diretório raiz quando um novo arquivo aparecer

*/

static void atualizarraizfat(FAT *fat, char *pesquisar, ARQUIVOFAT *arquivo_FAT)
{
    atualizarsetordirfat(fat, pesquisar, arquivo_FAT, fat->inicioRaiz, fat->tamanho_da_raiz);
	
    return;
	
}

/*****************************************************************************************/

/*

Função "analisarclusterfat" - Obtêm o número do cluster e calcula o início
                              do cluster na cadeia
 
 */

static void analisarclusterfat(FAT *fat,
                              unsigned int cluster,
                              unsigned long *InicioSetor,
                              unsigned int *ProximoCluster)
{
    unsigned int SetorFAT;
    static unsigned char buf[Numero_Maximo_Setores];
    int offset;

    *InicioSetor = (cluster - 2) * (long)fat->setores_por_cluster
                   + fat->InicioArquivo;
				   
    if (fat->fat16)
    {
	
        SetorFAT = fat->InicioFAT + (cluster * 2) / fat->tamanho_do_setor;
        lerlogicofat(fat, SetorFAT, buf);
        offset = (cluster * 2) % fat->tamanho_do_setor;
        *ProximoCluster = buf[offset] | ((unsigned int)buf[offset + 1] << 8);
		
    }
    else
    {
	
        SetorFAT = fat->InicioFAT + (cluster * 3 / 2) / fat->tamanho_do_setor;
        lerlogicofat(fat, SetorFAT, buf);
        offset = (cluster * 3 / 2) % fat->tamanho_do_setor;
        *ProximoCluster = buf[offset];
		
        if (offset == (fat->tamanho_do_setor - 1))
        {
		
            lerlogicofat(fat, SetorFAT + 1, buf);
            offset = -1;
			
        }
		
        *ProximoCluster = *ProximoCluster | ((unsigned int)buf[offset + 1] << 8);
		
        if ((cluster * 3 % 2) == 0)
        {
		
            *ProximoCluster = *ProximoCluster & 0xfffU;
       
	    }
	   
        else
        {
		
            *ProximoCluster = *ProximoCluster >> 4;
			
        }
		
    }
	
    return;
	
}

/*****************************************************************************************/

static void encontrarsetordirfat(FAT *fat,
                               char *pesquisar,
                               unsigned long InicioSetor,
                               int Numero_de_Setores)
{
    int x;
    unsigned char buf[Numero_Maximo_Setores];
    unsigned char *p;
    ARQUIVOFAT *arquivo_FAT = fat->arquivoFATcorrente;

    for (x = 0; x < Numero_de_Setores; x++)
    {
	
        lerlogicofat(fat, InicioSetor + x, buf);
		
        for (p = buf; p < buf + fat->tamanho_do_setor; p += 32)
        {
		
            if (memcmp(p, pesquisar, 11) == 0)
            {
			
                fat->clustercorrente = p[0x1a] | (p[0x1a + 1] << 8);
				
                if(fat->operacao==DELETAR_FAT)
                {
				
                    ZerarClusterfat(fat,fat->clustercorrente);
                    *p=0xe5;
                    escreverlogicofat(fat, InicioSetor + x, buf);
					
                    return;
					
                }
				
                else if(fat->operacao==RENOMEAR_FAT)
                {
				
                    memcpy(p, fat->novo_arquivo, 11);
                    escreverlogicofat(fat, InicioSetor + x, buf);
					
                    return;
					
                }
				
                else
                {
				
                    arquivo_FAT->cluster = fat->clustercorrente;
					
                    arquivo_FAT->TamanhoArquivo = p[0x1c]
                                    | ((unsigned int)p[0x1c + 1] << 8)
                                    | ((unsigned long)p[0x1c + 2] << 16)
                                    | ((unsigned long)p[0x1c + 3] << 24);
									
                    arquivo_FAT->atributo = p[0x0b];
                    memcpy(arquivo_FAT->datahora, &p[0x16], 4);
					
                    return;
					
                }
            }
			
            else if (*p == '\0')
            {
			
                fat->naoencontrado = 1;
				
                return;
				
            }
        }
    }
	
    fat->clustercorrente = 0;
	
    return;
	
}

/*****************************************************************************************/

static void atualizarsetordirfat(FAT *fat,
                               char *pesquisar,
                               ARQUIVOFAT *arquivo_FAT,
                               unsigned long InicioSetor,
                               int Numero_de_Setores)
{
    int x;
    unsigned char buf[Numero_Maximo_Setores];
    unsigned char *p;
    char dirent[32];
    int encontrado = 0; 

    for (x = 0; x < Numero_de_Setores && !encontrado; x++)
    {
        lerlogicofat(fat, InicioSetor + x, buf);
		
        for (p = buf; p < buf + fat->tamanho_do_setor; p += 32)
        {
            if (memcmp(p, pesquisar, 11) == 0)
            {
			
                fat->clustercorrente = p[0x1a + 1] << 8 | p[0x1a];
                ZerarClusterfat(fat, fat->clustercorrente);
                encontrado = 1;
				
            }
			
            if (encontrado || (*p == '\0'))
            {
			
                fat->clustercorrente = encontrarclusterlivre(fat);
				
                marcarclusterfat(fat, fat->clustercorrente);
				
                arquivo_FAT->InicioSetor = (fat->clustercorrente - 2)
                    * (long)fat->setores_por_cluster
                    + fat->InicioArquivo;

                arquivo_FAT->ParaSetor = 0;
                memset(p, '\0', 32);
                memcpy(p, pesquisar, 11);
                p[0x1a + 1] = (fat->clustercorrente >> 8) & 0xff;
                p[0x1a] = fat->clustercorrente & 0xff;
               
                arquivo_FAT->BytesTotais = 0;
                p[0x1c] = arquivo_FAT->BytesTotais;
                p[0x1c + 1] = 0;
                p[0x1c + 2] = 0;
                p[0x1c + 3] = 0;
                arquivo_FAT->SetorDir = InicioSetor + x;
                arquivo_FAT->OffsetDiretorio = (p - buf);
                arquivo_FAT->UltimosBytes = 0;
                p += 32;
				
                if (!encontrado)
                {
				
                    if (p != (buf + fat->tamanho_do_setor))
                    {
					
                        *p = '\0';
                        escreverlogicofat(fat, InicioSetor + x, buf);
						
                    }
					
                    else
                    {
					
                        escreverlogicofat(fat, InicioSetor + x, buf);
                        if ((x + 1) != Numero_de_Setores)
                        {
						
                            lerlogicofat(fat, InicioSetor + x + 1, buf);
                            buf[0] = '\0';
                            escreverlogicofat(fat, InicioSetor + x + 1, buf);
							
                        }
                    }
                }
				
                encontrado = 1;
				
                break;
				
            }
        }
    }
	
    if (!encontrado)
    {
	
        fat->clustercorrente = 0;
		
    }
	
    return;
	
}

/*****************************************************************************************/

/*

Função "lerlogicofat" - Lê um setor lógico do disco
 
*/

static void lerlogicofat(FAT *fat, long setor, void *buf)
{

    fat->lerLogico(fat->parametro, setor, buf);
	
    return;
	
}

/*****************************************************************************************/

/*

Função "escreverlogicofat" - Escrever em um setor no disco
 
*/

static void escreverlogicofat(FAT *fat, long setor, void *buf)
{
    fat->escreverLogico(fat->parametro, setor, buf);
	
    return;
	
}

/*****************************************************************************************/

/* 

Função "marcarclusterfat" - marca um cluster

*/

static void marcarclusterfat(FAT *fat, unsigned int cluster)
{
    unsigned long SetorFAT;
    static unsigned char buf[Numero_Maximo_Setores];
    int offset;

    
    if (fat->fat16)
    {
	
        SetorFAT = fat->InicioFAT + (cluster * 2) / fat->tamanho_do_setor;
        lerlogicofat(fat, SetorFAT, buf);
        offset = (cluster * 2) % fat->tamanho_do_setor;
        buf[offset] = 0xff;
        buf[offset + 1] = 0xff;
        escreverlogicofat(fat, SetorFAT, buf);
		
    }

    return;
	
}

/*****************************************************************************************/

/* 

Função "encontrarclusterlivre" - obter próximo cluster livre 

*/

static unsigned int encontrarclusterlivre(FAT *fat)
{
    static unsigned char buf[Numero_Maximo_Setores];
    unsigned long SetorFAT;
    int encontrado = 0;
    int x;
    unsigned int ret;

    
    if (fat->fat16)
    {
        for (SetorFAT = fat->InicioFAT;
             SetorFAT < fat->inicioRaiz;
             SetorFAT++)
        {
		
            lerlogicofat(fat, SetorFAT, buf);
			
            for (x = 0; x < Numero_Maximo_Setores; x += 2)
            {
			
                if ((buf[x] == 0) && (buf[x + 1] == 0))
                {
				
                    encontrado = 1;
                    break;
					
                }
            }
			
            if (encontrado) break;
        }
		
        if (encontrado)
        {
		
            ret = (SetorFAT-fat->InicioFAT)*Numero_Maximo_Setores/2 + x/2;
			
            return (ret);
			
        }
    }
	
    return (0);
	
}

/*****************************************************************************************/

/* 

Função "cadeiafat" - ir a novo cluster 

*/

static void cadeiafat(FAT *fat, ARQUIVOFAT *arquivo_FAT)
{
    unsigned long SetorFAT;
    static unsigned char buf[Numero_Maximo_Setores];
    int offset;
    unsigned int Novo_Cluster;
    unsigned int Antigo_Cluster;

    Antigo_Cluster = fat->clustercorrente;
    Novo_Cluster = encontrarclusterlivre(fat);
   
    if (fat->fat16)
    {
       
        SetorFAT = fat->InicioFAT + (Antigo_Cluster * 2) / fat->tamanho_do_setor;
       
        lerlogicofat(fat, SetorFAT, buf);
        offset = (Antigo_Cluster * 2) % fat->tamanho_do_setor;
		
        /* Ponteiro do novo para o velho cluster */
		
        buf[offset] = Novo_Cluster & 0xff;
        buf[offset + 1] = Novo_Cluster >> 8;
        escreverlogicofat(fat, SetorFAT, buf);

        
        marcarclusterfat(fat, Novo_Cluster);

        /* Atualizar para novo cluster */
		
        fat->clustercorrente = Novo_Cluster;

       
        arquivo_FAT->InicioSetor = (fat->clustercorrente - 2)
                    * (long)fat->setores_por_cluster
                    + fat->InicioArquivo;
        arquivo_FAT->ParaSetor = 0;
    }

    return;
	
}

/*****************************************************************************************/

/*

Função "DeletarArquivoFat" - Deletar um arquivo o procurando e 
                             zerando todos os seus setores
							 
*/

int DeletarArquivoFat(FAT *fat,const char *nome_arquivo_fat)
{

        fat->naoencontrado = 0;

    if ((nome_arquivo_fat[0] == '\\') || (nome_arquivo_fat[0] == '/'))
    {
	
        nome_arquivo_fat++;
		
    }
	
    PosicaoFat(fat,nome_arquivo_fat);

    if(fat->naoencontrado)
    {
	
        return(ARQUIVO_NAO_ENCONTRADO);
		
    }
	
    else
    {
	
        ZerarClusterfat(fat,fat->clustercorrente);
        fat->ed->nome_arquivo[0]=DEL;
        escreverlogicofat(fat, fat->SetorDIR, fat->dbuf);
		
        return(0);
		
    }
}

/*****************************************************************************************/

/*

Função "RenomearArquivoFat" - Renomear um arquivo do antigo para o novo nome
 
*/

int RenomearArquivoFat(FAT *fat,const char *antigo,const char *novo)
{
    ARQUIVOFAT arquivo_FAT;
    char nome_arquivo_fat[NUM_ARQUIVOS_MAX];
    const char *p;

    if ((antigo[0] == '\\') || (antigo[0] == '/'))
    {
	
        antigo++;
		
    }
	
    fat->arquivoFATcorrente = &arquivo_FAT;
    fat->naoencontrado=0;
    strcpy(nome_arquivo_fat,antigo);
    p=strchr(nome_arquivo_fat,'\\');
	
    if (p == NULL)
    {
	
        strcpy(nome_arquivo_fat, novo);
		
    }
	
    else
    {
	
        strcpy(p+1,novo);
		
    }

    inicioclusterfat(fat,nome_arquivo_fat);
	
    if(!fat->naoencontrado)
    {
	
        return(-2);
		
    }
	
    else
    {
	
        fat->naoencontrado=0;
		
        memset(fat->novo_arquivo, ' ', 11);
		
        p = strchr(novo, '.');
		
        if (p != NULL)
        {
		
            if ((p - novo) > 8)
            {
			
                return(-8);
				
            }
			
            else
            {
			
                memcpy(fat->novo_arquivo, novo, p - novo);
				
            }
			
            if (tamanhostring(p+1) > 3)
            {
			
                return(-4);
				
            }
			
            else
            {
			
                memcpy(fat->novo_arquivo + 8, p+1, tamanhostring(p+1));
				
            }
        }
		
        else if((tamanhostring(novo))<=8)
        {
		
            memcpy(fat->novo_arquivo,novo,tamanhostring(novo));
			
        }

        fat->operacao=RENOMEAR_FAT;
        inicioclusterfat(fat, antigo);
        fat->operacao=0;
		
        if(fat->naoencontrado)
        {
		
            return (-1);
			
        }
		
        else
        {
		
            return (0);
			
        }
    }
}

/*****************************************************************************************/

/*

Função "ObterAtributosFat" - Obtêm os atributos de um arquivo 

*/

int ObterAtributosFat(FAT *fat,const char *nome_arquivo_fat,int *atributo)
{
    int ret;
    ARQUIVOFAT arquivo_FAT;

    ret=AbrirArquivoFat(fat,nome_arquivo_fat,&arquivo_FAT);
	
    if(ret==0)
    {
	
       *atributo=arquivo_FAT.atributo;
	   
    }
	
    else
    {
	
       *atributo=0;
	   
    }
	
    return(ret);
	
}

/*****************************************************************************************/

/*

Função "ZerarClusterfat" - Deleta um arquivo zerando o cluster por completo

*/
 
static void ZerarClusterfat(FAT *fat, unsigned int cluster)
{
    unsigned long SetorFAT;
    static unsigned char buf[Numero_Maximo_Setores];
    int offset;
    int em_buffer = 0;

    while (!fimclusterfat(fat, cluster))
    {
      
        if (fat->fat16)
        {
		
            SetorFAT = fat->InicioFAT + (cluster * 2) / fat->tamanho_do_setor;
			
            if (em_buffer != SetorFAT)
            {
			
                if (em_buffer != 0)
                {
				
                    escreverlogicofat(fat, em_buffer, buf);
					
                }
				
                lerlogicofat(fat, SetorFAT, buf);
                em_buffer = SetorFAT;
				
            }
			
            offset = (cluster * 2) % fat->tamanho_do_setor;
            cluster = buf[offset + 1] << 8 | buf[offset];
            buf[offset] = 0x00;
            buf[offset + 1] = 0x00;
			
        }
    }
	
    if (em_buffer != 0)
    {
	
        escreverlogicofat(fat, em_buffer, buf);
		
    }
	
    return;
	
}

#endif