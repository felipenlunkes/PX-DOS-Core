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
/*                                                                   */
/*              Interpretador de Comandos do PX-DOS                  */
/*                                                                   */
/*********************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "util.px"
#include "PX_DOS.h"
#include "tempo.h"


#define ATRIBUTO_DIRETORIO 0x10 // Define o retorno do tipo de entrada no FAT12 para Diretório


//***************************************************************************


void __exec(char *cmd, void *ambiente); // Chama a função de executar do PX-DOS presente na biblioteca C

void putch(int a);

void sino(void);

void obter_seguro(char *buffer, int tamanho);

void limpar_saida(void);

static void deletararquivo(char *nome_de_arquivo);

static void passarargumentos(int argc, char **argv);

static void processarentrada(void);

static void mostrarPrompt(void);

static void exibir(char *arquivo);

static void dir(char *diretorio);

static void alterardir(char *para);

static void alterardisco(int drive);

static int compararString(char *um, char *dois);

static int compararStringeTamanho(char *um, char *dois, tamanho_t tamanho);

static void lerArquivoEmLotes(char *nome);

static void semPrompt(void);

static void copiararquivo(char *nome);

//***************************************************************************


static char buf[200];

static char data[10];

static unsigned char cmdt[140];

static tamanho_t tamanho;

ARQUIVO *arquivo;

char *segundo_parametro;

char *terceiro_parametro;

char *quarto_parametro;

char *quinto_parametro;

char *dispositivo_saida;

static int contador=0;

static char *variaveis[14];

static char *valores_variaveis[14];

static int contador_variaveis=0;

unsigned int disp_saida;

char *operacoes_parametros;
	
static char drive[2] = "A";

static char diretorio_de_trabalho_atual[65];

static char prompt[50] = ">";

static int umcomando = 0;

static int primeiro = 0;

static int terminal = 0;

static int nencontrado=0;

static int tipo_de_echo = 0;



//***************************************************************************


static struct {

    int ambiente;
	
    unsigned char *fimcmd;
	
    char *parametro1;
	
    char *parametro2;
	
} blocodeParametros = { 0, cmdt, NULL, NULL };


//***************************************************************************


static char *opcoes_execucao[]= {  // Esta estrutura (vetor) armazena a extenção dos 
                                   // arquivos executáveis suportados pelo sistema. 
								   
".epx", // Extensão comum de arquivo executável no formato PX-DOS
".com", // Extensão de arquivo executável binário puro
".col"  // Extensão de arquivo de execução em lotes do PX-DOS

}; 


//***************************************************************************

int main(int argc, char **argv)
{


    passarargumentos(argc, argv);
	
    if (umcomando)
    {
	
        processarentrada();
        return (0);
		
    }
	
    if (primeiro)
    {
	
	 
        printf("\n\nBem Vindo ao PX-DOS\n");
        printf("Copyright (C) 2013-2016 Felipe Miguel Nery Lunkes\n\n");
		
		
        lerArquivoEmLotes("AUTOEXEC.COL");
		
    }
	
    else
    {
	
        printf("\n\nBem Vindo ao PX-DOS. Insira sair para finalizar.\n");
		
    }
	
    while (!terminal)
    {
	
        mostrarPrompt();
		
        obter_seguro(buf, sizeof buf);

        processarentrada();
    }
	
    printf("\nObrigado por usar o PX-DOS\n");
	
    return (0);
}


//***************************************************************************

static void passarargumentos(int argc, char **argv)
{
    int x;
    
    if (argc > 1)
    {
	
        if ((argv[1][0] == '-') || (argv[1][0] == '/'))
        {
		
            if ((argv[1][1] == 'C') || (argv[1][1] == 'c'))
            {
			
                umcomando = 1;
				
            }        
			
            if ((argv[1][1] == 'P') || (argv[1][1] == 'p'))
            {
			
                primeiro = 1;
				
            }      
			
        }
		
    }
	
    if (umcomando)
    {
        strcpy(buf, "");
		
        for (x = 2; x < argc; x++)
        {
		
            strcat(buf, *(argv + x));
            strcat(buf, " ");
			
        }
        tamanho = tamanhostring(buf);
		
        if (tamanho > 0)
        {
		
            buf[tamanho - 1] = '\0';
			
        }
    }
	
    return;
}

//***************************************************************************

static void processarentrada(void)
{
    char *p;
	
	char *variavel;
	
	nencontrado=0;

    tamanho = tamanhostring(buf);
	
    if ((tamanho > 0) && (buf[tamanho - 1] == '\n'))
    {
	
        tamanho--;
        buf[tamanho] = '\0';
		
    }
	
    p = strchr(buf, ' ');


	
    if (p != NULL)
    {
	
        *p++ = '\0';
		
    }


	
    else
    {
	
        p = buf + tamanho;
		
    }
	
    tamanho -= (tamanho_t)(p - buf);
	
    // Agora, o segundo parâmetro deve ser separado do primeiro.
	
	segundo_parametro = strchr(p,' ');
    *segundo_parametro++ = '\0';
	
	// Agora, o terceiro parâmetro deve ser separado do segundo.
	 
	terceiro_parametro = strchr(segundo_parametro,' ');
    *terceiro_parametro++ = '\0';
	
	// Agora, o quarto parâmetro deve ser separado do terceiro.
	 
	quarto_parametro = strchr(terceiro_parametro,' ');
    *quarto_parametro++ = '\0';
	
	// Agora, o quinto parâmetro deve ser separado do quarto.
	 
	quinto_parametro = strchr(quarto_parametro,' ');
    *quinto_parametro++ = '\0';
	

//***************************************************************************	

/*
	
Comandos internos do Interpretador de Comandos do PX-DOS	
	
	
*/
	
//***************************************************************************
	
    if (compararString(buf, "sair") == 0)
    {
	
	if (compararString(p, "/?") == 0){
	
	printf("\nSAIR\n");
	printf("\n\nUse este comando para fechar uma instancia do\n");
	printf("interpretador de comandos do PX-DOS aberta.\n\n");
	printf("Voce nao podera fechar a primeira instancia aberta\n");
	printf("durante a iniciacao do sistema.\n\n");
	
	
	} 
	
	else{
	
	
        if (!primeiro)
        {
		
            terminal = 1;
			
        }
		
    }
	
	}
	
//***************************************************************************
	
	else if (compararString(buf, "definir") == 0)
	{
	
	limpar_saida();
	
	if (compararString(p, "/?") == 0){
	
	printf("\DEFINIR\n");
	printf("\n\nUse este comando para definir uma variavel de ambiente.\n\n");
	printf("Exemplo de uso:\n\n");
	printf("definir SISTEMA = PX-DOS\n\n");
	printf("O uso sem parametros exibe as variaveis definidas.\n\n");
	
	}
	
	else if (compararString(p, "") == 0)
	{
	
	for (contador = 0; contador <14; contador++){
	
	printf("\n%s=%s", variaveis[contador], valores_variaveis[contador]);
	
	}
	
	printf("\n\n");
	
	}
	
	else if (compararString(p, "") != 0){
	
				
			if (contador_variaveis <=14) {
			
			variaveis[contador_variaveis]=p;
			
			// Segundo parâmetro é igual a "="
			
			valores_variaveis[contador_variaveis]=terceiro_parametro;
			
			contador_variaveis++;
			
			}	
				
			if (contador_variaveis >= 15 ){
			
			printf("\n\nNumero maximo de variaveis ja definidas.\n\n");
			
			}
	
	}
	
	}
	
//***************************************************************************
	
    else if (compararString(buf, "exibir") == 0)
    {
	
	if (compararString(p, "/?") == 0) {
	
	printf("\nEXIBIR\n");
	printf("\n\nUse este comando para exibir o conteudo de um arquivo.\n\n");
	printf("Exemplo de uso:\n\n");
	printf("exibir oi.txt\n\n");
	printf("Caso o arquivo oi.txt exista, seu conteudo sera exibido.\n\n");
	
	} 
	
	else if (compararString(p, "") == 0 ){
	
	printf("\nUm nome de arquivo para exibir e necessario para este comando.\n");
	
	}
	
	else if (compararString(p, "") != 0 )
	{
	
        exibir(p);
		
		}
		
    }

//***************************************************************************
	
    else if (compararString(buf, "deletar") == 0)
    {
	
	if (compararString(p, "/?") == 0) {
	
	printf("\nDELETAR\n");
	printf("\n\nUse este comando para excluir algum arquivo.\n\n");
	printf("Exemplo de uso:\n\n");
	printf("deletar oi.txt\n\n");
	printf("Caso o arquivo oi.txt exista, sera excluido.\n\n");
	
	} 
	
	else if (compararString(p, "") == 0 ){
	
	printf("\nUm nome de arquivo para deletar e necessario para este comando.\n");
	
	}
	
	else if (compararString(p, "") != 0 )
	{
	
        deletararquivo(p);
		
		}
		
    }

//***************************************************************************

	else if (compararString(buf, "comen") == 0)
	{
	
	if (compararString(p, "/?") == 0 )
	{
	
	printf("\nCOMEN\n");
	printf("\n\nUse este comando para incluir um comentario em um arquivo\n");
	printf("de comandos em lote do PX-DOS (.col).\n\n");
	
	}
	
	else if (compararString(p, "") != 0 ) 
	{
	
	// Nenhuma ação deverá ser executada para um comentário.
	
	}
	
	}
	
//***************************************************************************
	
	else if (compararString(buf, "chamar") == 0)
	{
	
	if (compararString(p, "/?") == 0)
	{
	
	printf("\nCHAMAR\n");
	printf("\n\nUse este comando para chamar um arquivo de comandos em lote\n");
	printf("durante a execucao de outro arquivo de comandos em lote do PX-DOS.\n\n");
	printf("Uso deste comando:\n\n");
	printf("chamar proximo.col\n\n");
	
	}
	
	else if (compararString(p, "") != 0){
	
	lerArquivoEmLotes(p);
	
	}
	
	else if (compararString(p, "") == 0)
	{
	
	printf("\nO nome de um arquivo de comandos em lote do PX-DOS deve ser passado.\n");
	
	}
	
	
	}
	
//***************************************************************************
	
	else if (compararString(buf, "infodata") == 0){
	
	if (compararString(p, "/?") == 0){
	
	printf("\nINFODATA\n");
	printf("\n\nUse este comando para definir ou exibir a data atual.\n\n");
	printf("Exemplo de uso:\n\n");
	printf("infodata <parametro>\n\n");
	printf("Em que o <parametro> pode ser:\n");
	printf("\n/d - Definir uma data.");
	printf("\n/? - Exibe esta ajuda.");
	printf("\n\nSem parametros, a data atual e exibida.\n\n");
		
	}
	
	else if (compararString(p, "/d") == 0 ){
	
	printf("\n\nInsira a data atual: ");
	limpar_saida();
	obter_seguro(data, sizeof data); // Obtêm a data vinda do usuário e a armazena
	printf("\n\nData definida como: %s\n\n", data);
	
	}
	
	else if (compararString(p, "") == 0 ){
	
	if (compararString(data, "") == 0){
	
	printf("\n\nNenhuma data definida previamente.\n\n");
	
	}
	
	else {
	
	printf("\n\nData atual: %s\n\n", data);
	
	}
	
	}
	
	}

//***************************************************************************
	
	else if (compararString(buf, "pausa") == 0)
    {
	
	if (compararString(p, "silencio") == 0){
	
	asm {
	 
	mov ax, 0
	int 16h
	 
	}
	
	} 
	
	else if (compararString(p, "") == 0){
        
     printf("\nPressione qualquer tecla para continuar...\n");
	 
	asm {
	 
	mov ax, 0
	int 16h
	 
	}
		
    } 
	
	else if (compararString(p, "/?") == 0) {
	
	printf("\nPAUSA\n");
	printf("\n\nUse este comando para paralizar a execucao de um arquivo\n");
	printf("em lotes do PX-DOS (*.col)\n\n");
	printf("Exemplo de uso:\n\n");
	printf("pausa <parametro>\n\n");
	printf("Em que o <parametro> pode ser:\n");
	printf("\nsilencio - Nao exibe nenhuma mensagem de pausa.");
	printf("\n<mensagem> - Exibe uma mensagem personalizada.");
	printf("\n/? - Exibe esta ajuda.\n\n");
	
	}
	
	else if (compararString(p, "") != 0){
	
	printf("\n%s\n",p);
	
	asm {
	 
	mov ax, 0
	int 16h
	 
	}
	
	} 
	
	}

//***************************************************************************
	
	else if (compararString(buf, "echo") == 0)
	{
	
	// Agora, o segundo parâmetro deve ser separado do primeiro.
	
    segundo_parametro = strchr(p,' ');
    *segundo_parametro++ = '\0';
	
	// Agora, verificando o dispositivo de saída, passado após o comando
	// '>>' de redirecionamento.
	
	dispositivo_saida = strchr(segundo_parametro,' ');
	*dispositivo_saida++ = '\0';
	
	// Agora, verificação do primeiro parâmetro
	
	if (compararString(p, "/?") == 0 ){
	
	printf("\nECHO\n");
	printf("\nEste comando exibe uma mensagem personalizada.\n");
	printf("\n\nExemplo de uso:\n\n");
	printf("echo <parametro> ou <mensagem>");
	printf("\n\nO <parametro> ou <mensagem> podem ser:\n\n");
	printf("ativar - Ativa o echo.");
	printf("\ndesativar - Desativa o echo.");
	printf("\n<sem parametros> - Exibe se o echo esta ligado ou nao.");
	printf("\n<mensagem> - Exibe a mensagem desejada.\n\n");
	
	}
	
	else if (compararString(p, "desativar") == 0)
     {
		
            tipo_de_echo = 1;
			
     }
		
    else if (compararString(p, "ativar") == 0)
     {
		
            tipo_de_echo = 0;
			
     }
		
	else if (compararString(p, "") == 0){
		
		if (tipo_de_echo == 1 ){
		
		printf("\nO echo esta desabilitado.\n");
		
	} 
		
		if (tipo_de_echo == 0 ){
		
		printf("\nO echo esta habilitado.\n");
		
		}
		
		}
		
	else if (compararString(segundo_parametro, ">>") == 0)
	{
	
	if (compararString(dispositivo_saida, "lpt1") == 0)
	{
	
	printf("\nEnviando dados a impressora...\n");
	
	disp_saida = 0x00;
	
	PX_DOSIniciarImpressora(disp_saida);
	
	}
	
	else if (compararString(dispositivo_saida, "lpt2") == 0)
	{
	
	printf("\nEnviando dados a impressora...\n");
	
	disp_saida = 0x1;
	
	PX_DOSIniciarImpressora(disp_saida);
	
	}
	
	else if (compararString(dispositivo_saida, "com1") == 0)
	{
	
	printf("\nEnviando dados a porta serial...\n");
	
	disp_saida = 0x0;
	
	PX_DOSEnviarSerial(disp_saida,(int)segundo_parametro,0x01);
	
	}
	
	else if (compararString(dispositivo_saida, "com2") == 0)
	{
	
	printf("\nEnviando dados a porta serial...\n");
	
	disp_saida = 0x1;
	
	PX_DOSEnviarSerial(disp_saida,(int)segundo_parametro,0x01);
	
	}
   
    }	
        else
        {
		
            printf("\n%s\n",p);
			
        }
	   	
	}

//***************************************************************************
	
	else if (compararString(buf, "echo.") == 0)
	{
	
	if (compararString(p, "/?") == 0) {
	
	printf("\nECHO.\n");
	printf("\n\nEste comando pula uma linha na tela.\n\n");
	
	}
	
	else if (compararString(p, "") == 0){
	
	
	    printf("\n");
		
		}
		
	}

//***************************************************************************
	
    else if (compararString(buf, "dir") == 0)
    {
	
	if (compararString(p, "/?") == 0 )
	{
	
	printf("\nDIR\n");
	printf("\nUtilize este comando para exibir, em forma de lista, os arquivos\n");
	printf("e diretorios presentes em seu computador. Voce pode utilizar este comando");
	printf("\nsem parametros para exibir todo o conteudo de uma pasta ou com parametros");
	printf("\npara definir qual arquivo deseja verificar. Exemplo:\n\n");
	printf("dir oi.txt\n\n");
	printf("Caso o arquivo oi.txt exista, ele sera exibido na lista. Voce tambem pode\n");
	printf("usar o nome de uma pasta como parametro.\n\n");
	
	} 
	
	else
	{
	
        dir(p);
		
	}
	
    }

//***************************************************************************
	
	else if (compararString(buf, "ver") == 0)
	{
	
	
    if (compararString(p, "/?") ==0) 
	{
	
	printf("\nVER\n");
	printf("\nEste comando exibe informacoes de versao do sistema.\n\n");
	printf("Use ver <parametro> para saber mais informacoes sobre o sistema.\n");
	printf("\nO parametro <parametro> pode ser:\n\n");
	printf("/c - Exibe informacoes de copyright do sistema.\n");
	printf("/d - Exibe informacoes do desenvolvedor do sistema.\n");
	printf("/? - Exibe esta ajuda.\n\n");
	
	}
	
	else if (compararString(p, "/c") == 0){
	
	printf("\nCOPYRIGHT\n");
	printf("\n\nCopyright (C) 2013-2016 Felipe Miguel Nery Lunkes\n");
	printf("\nTodos os direitos reservados.\n\n");
	printf("O produto 'PX-DOS' e todos os seus componentes sao");
	printf("\nprotegidos com direitos autorais.\n");
	printf("\nA distribuicao, locacao e emprestimo, assim como\n");
    printf("engenharia reversa e quebra de direitos autorais sao\n");
	printf("estritamente proibidos sob pena de punicao legal.\n\n"); 
	
	}
	
	else if (compararString(p, "/d") == 0){
	
	printf("\nDESENVOLVEDOR\n");
	printf("\n\nEste sistema foi integralmente desenvolvido por\n");
	printf("Felipe Miguel Nery Lunkes.\n\n");
	printf("\n\nCopyright (C) 2013-2016 Felipe Miguel Nery Lunkes\n");
	printf("Todos os direitos reservados.\n\n");
	
	}
	
	else if (compararString(p, "") == 0){
	
	    printf("\nSistema Operacional PX-DOS\n");
		printf("\nVersao do PX-DOS: 0.9.0\n");
		printf("Sistema Operacional de: 16 Bits\n");
		printf("Sistema de Arquivos suportado: FAT12, FAT16\n");
        printf("\nCopyright (C) 2013-2016 Felipe Miguel Nery Lunkes\n");
		printf("\nTodos os direitos reservados.\n");
	
	}
		
	}

//***************************************************************************
	
	else if (compararString(buf, "titulo") == 0)
	{
	
	if (compararString(p, "/?") == 0 )
	
	{
	
	printf("\nTITULO\n");
	printf("\nCom este comando voce pode definir um titulo que sera\n");
	printf("exibido no topo da tela durante a execucao de um Arquivo de\n");
	printf("Comando em Lotes do PX-DOS.\n\n");
	printf("Uso deste comando:\n\n");
	printf("titulo Meu Programa\n\n");
	
	
	}
	
	else if (compararString(p, "") != 0 ){
	
	int tamanho_titulo=0;
	int contador_titulo=0;
	
	tamanho_titulo=tamanhoString(p);
	
	limpartela();
	
	printf("%s\n", p);
	
	for (contador_titulo=0;contador_titulo<=tamanho_titulo;contador_titulo++){
	
	printf("=");
	
	}
	
	printf("\n");
	

	}
	
	}
	
//***************************************************************************

    else if (compararString(buf, "tamanho") == 0 ){
	
	if (compararString(p, "/?") == 0){
	
    printf("\nTAMANHO\n");
	printf("\n\nUse este comando para descobrir o tamanho de uma\n");
	printf("palavra ou frase, em quantidade de letras.\n");
	printf("Aviso! Os espacos entre palavras sao contabilizados\n");
	printf("durante a contagem.\n\n");
	printf("Uso deste comando:\n\n");
	printf("tamanho Palavra\n\n");
	
	
	}
	
	else if (compararString(p, "") !=0 ) {
	
	int tamanhoFrase=0;
	
	tamanhoFrase=tamanhoString(p);
	
	printf("\n\nO texto possui %d letras.\n\n", tamanhoFrase);
	
	
	}
	
	}

//***************************************************************************
	
    else if (compararString(buf, "cd") == 0)
    {
	
	if (compararString(p, "/?") == 0 ) {
	
	printf("\nCD\n");
	printf("\n\nUse este comando para acessar uma pasta em seu computador.\n\n");
	printf("\nExemplo de uso:\n\n");
    printf("\n\ncd <pasta>\n\n");
	
	}
	
	else if (compararString(p, "") == 0 ){
	
	printf("\nUm argumento <pasta> e necessario para este comando.\n");
	
	}
	
	else if (compararString(p, "") != 0 ){
	
        alterardir(p);
		
		}
		
		
    }

//***************************************************************************
	
    else if (compararString(buf, "copiar") == 0)
    {
	
	if (compararString(p, "/?") == 0 ) {
	
	printf("\nCOPIAR\n");
	printf("\n\nUse este comando para copiar arquivos.\n\n");
	printf("\nExemplo de uso:\n\n");
    printf("\n\ncopiar <arquivo1> <arquivo2>\n\n");
	
	}
	
	
	else if (compararString(p, "") != 0 ){
	
        copiararquivo(p);
		
		}
		
		
    }

//***************************************************************************
	
	else if (compararString(buf, "cls") == 0)
    {
	
	if (compararString(p, "/?") == 0) {
	
	printf("\nCLS\n");
	printf("\n\nEste comando limpa a tela do sistema.\n\n");
	
	} 
	
	else if (compararString(p, "") == 0 ){
	
	
        limpartela();
		
		return;
		
		}
		
    }

//***************************************************************************
	
    else if (compararStringeTamanho(buf, "cd.", 3) == 0)
    {
	
        alterardir(buf + 2);
		
    }

//***************************************************************************
	
    else if (compararStringeTamanho(buf, "cd\\", 3) == 0)
    {
	
        alterardir(buf + 2);
		
    }

//***************************************************************************
	
	else if (compararString(buf, "exec") == 0 )
	{
	
	if (compararString(p, "/?") == 0 ){
	
	printf("\nEXEC\n");
	printf("\n\nExibe informacoes da ordem de busca e execucao\n");
	printf("de aplicativos pelo Interpretador de Comandos\n");
	printf("do PX-DOS. Use este comando sem parametros para mais\n");
	printf("informacoes.\n\n");
	printf("Use defexec /? para saber mais informacoes sobre esta ordem.\n\n");
	
	} 
	
	else if (compararString(p, "") == 0 ){
	
	printf("\n\nPrimeira opcao: %s\n", opcoes_execucao[0]);
	printf("Segunda opcao: %s\n", opcoes_execucao[1]);
	printf("Terceira opcao: %s\n\n", opcoes_execucao[2]);
	
	} 
	
	else 
	
	{
	
	printf("\nOpcao desconhecida e invalida '%s' inserida.\n", p);
	
	}
	
	
	}

//***************************************************************************	
	
	else if (compararString(buf, "defexec") == 0) {
	
	
	if (compararString(p, ".epx") == 0 ){
	
	opcoes_execucao[0]=".epx";
	opcoes_execucao[1]=".com";
	
	}
	
	else if (compararString(p, ".com") == 0 ){
	
	opcoes_execucao[0]=".com";
	opcoes_execucao[1]=".epx";
	
	}
	
	else if (compararString(p, "/?") == 0 ) {
	
	printf("\nORDEM DE EXECUCAO\n");
	printf("\n\nVoce pode mudar a ordem em que os aplicativos serao\n");
	printf("executados.\n\n");
	printf("Exemplo de uso:\n\n");
	printf("defexec <parametro>\n\n");
	printf("O <parametro> pode ser:\n\n");
	printf(".com - Aplicativos com essa extensao serao procurados primeiro.\n");
	printf(".epx - Aplicativos com essa extensao serao procurados primeiro.\n\n");
	
	}
	
	else if (compararString(p, "") == 0 ){
	
	printf("\nUm parametro e necessario para este comando.\n");
	
	} else 
	
	{
	
	printf("\nOpcao desconhecida e invalida '%s' inserida.\n", p);
	
	}
	
	
	}

//***************************************************************************	

	
    else if (compararString(buf, "reiniciar") == 0)
    {
	
	if (compararString(p, "/?") == 0){
	
	printf("\nREINICIAR\n");
	printf("\n\nEste comando reinicia seu computador em caso de erros ou instabilidade\n");
	printf("no sistema.\n\n");
	printf("Por seguranca, o computador so sera reiniciado se o parametro 'agora'\n");
	printf("for passado.\n\n");
	printf("Uso do comando:\n");
	printf("reiniciar agora\n\n");
	
	
	}
	
	else if (compararString(p, "") == 0)
	{
	
	printf("\nUm parametro e necessario para continuar.\n");
	printf("Use 'reiniciar /?' para mais informacoes.\n");
	
	}
	
	else if (compararString(p, "agora") == 0){
	
        asm{
		
		mov ah, 04h
		mov bx, 1h
		int 90h
		
		}
		
		}
		
    }

//***************************************************************************
	
    else if ((tamanhostring(buf) == 2) && (buf[1] == ':'))
    {
	
        alterardisco(buf[0]);
		
    }

//***************************************************************************

	
	else if (compararString(buf, "") == 0){
	
	// Nada será feito, porque nenhum comando foi inserido.
	
	}
	
//***************************************************************************	
   
	
//***************************************************************************	
/*	
	
	               Área de execução de programas externos
				   
- À seguir, o sistema tentará encontrar arquivos binários executáveis compatíveis				
  através das funções abaixo.				
	
	
*/

    else {
	
		char *ExtencaoDoExecutavel;
        
        cmdt[0] = (unsigned char)tamanho;
		
        memcpy(cmdt + 1, p, tamanho);
		
        memcpy(cmdt + tamanho + 1, "\r", 2);
		
        ExtencaoDoExecutavel = buf + tamanhostring(buf);
		
		
//***************************************************************************
		
 // Verifica se existe o aplicativo digitado que tenha uma extenção válida definida
 // pelo usuário. 
 
		strcpy(ExtencaoDoExecutavel, opcoes_execucao[0]); // Define a extenção como a determinada pelo usuário
		
		 arquivo = abrir( buf, "r" ); // Abre o programa digitado com a extenção 
		
	if (arquivo != NULL){                 // Se o arquivo não for zero, ou seja, existir
	
        __exec(buf, &blocodeParametros);  // Execute o programa 
		
		return;
    } 
	
	if (arquivo == NULL){                 // Se o arquivo não existir, ou seja, ser zero
	
    nencontrado=1;                     	  // Incremente a variável em 1
	
	}	

//***************************************************************************
		
 // Verifica se existe o aplicativo digitado que tenha uma extenção válida definida
 // pelo usuário.
 
        strcpy(ExtencaoDoExecutavel, opcoes_execucao[1]);  // Define a extenção como a determinada pelo usuário
		
	    arquivo = abrir( buf, "r" );  // Abre o programa digitado com extenção 
		
	if (arquivo != NULL){                 // Se o arquivo não for zero, ou seja, existir
	
        __exec(buf, &blocodeParametros);  // Execute o programa
		
    } 
	
    if (arquivo == NULL){                 // Se o arquivo não existir, ou seja, ser zero
	
    nencontrado+=1;                        // Incremente a variável em 1
	
    }		

//***************************************************************************
		
 // Verifica se existe algum script batch digitado que tenha extenção ".col"
 
	strcpy(ExtencaoDoExecutavel, ".col");                   // Define a extenção como ".col"
		
	    arquivo = abrir( buf, "r" );      // Abre o programa digitado com extenção ".col"
		
	if (arquivo != NULL){                 // Se o arquivo não for zero, ou seja, existir
	
       lerArquivoEmLotes(buf);            // Execute o script
    } 
	
    if (arquivo == NULL){                 // Se o arquivo não existir, ou seja, ser zero
	
    nencontrado+=1;                       // Incremente a variável em 1
	
    }		

//***************************************************************************

	
        if (nencontrado == 3){  // Se a variável for igual a 3, 
		                        // isso quer dizer que o aplicativo
		                        // ".com" ou ".epx" não foi encontrado, assim como
								// o script ".col".
		
		printf("\nComando interno ou aplicativo PX-DOS nao encontrado.\n");
		
		}

 		} // Fim do else responsável pela execução deste bloco
		
		
//***************************************************************************
/*
	 
	        Fim das funções de carregamento de arquivos binários.
				
	 
*/
	 
    return;
}

//***************************************************************************

static void mostrarPrompt(void)
{
    int d;
	
	if (tipo_de_echo == 0 ) // Se o echo do prompt estiver ativado
	{
    
    d = PX_DOSObterDrivePadrao();
    drive[0] = d + 'A';
    PX_DOSObterDiretorioAtual(0, diretorio_de_trabalho_atual);
    printf("\n%s:\\%s%s", drive, diretorio_de_trabalho_atual, prompt);
    limpar_saida();
	
	}
	
	else 
	
	{
	
	printf("\n");   // Se o echo não estiver ativado
	
	}
	
	
    return;
}

//***************************************************************************

static void exibir(char *arquivo)
{
    ARQUIVO *ArquivoEmDisco;
    
    ArquivoEmDisco = abrir(arquivo, "r");
	
    if (ArquivoEmDisco != NULL)
    {
	
        while (fgets(buf, sizeof buf, ArquivoEmDisco) != NULL)
        {
		
            fputs(buf, stdout);
			
        }
    }
	
    else
    {
	
       printf("Arquivo '%s' nao encontrado para ser exibido.\n", arquivo);
	   
    }
	
    return;
}

//***************************************************************************

static void dir(char *diretorio)
{
    char *dados;
	
	int numeroArquivos=0;
	
    int ret;
	
    char *p;
	
    tempo_t DataArquivo;
	
    struct tm *DataCriacaoArquivo;
	
    printf("\nNome            Tamanho           Data da criacao\n");
    printf("----            -------           ---------------\n");
	
    dados = PX_DOSObterATD();
	
    if (*diretorio == '\0')
    {
	
        p = "*.*";
		
    }
	
	else {
	
	p = diretorio;
	
	}
	
    ret = PX_DOSEncontrarPrimeiro(p, 0x10); // Avisa que primeiro devem ser encontradas as
	                                        // pastas
	
    while (ret == 0)
    {
	
        DataArquivo = dos_para_tempo(dados + 0x16);
		
        DataCriacaoArquivo = horalocal(&DataArquivo);
		
        printf("%-13s %9ld %07s %02d-%02d-%02d %02d:%02d:%02d\n", 
               dados + 0x1e,
               *(long *)(dados + 0x1a),
               dados[0x15] & ATRIBUTO_DIRETORIO? "<PASTA>":"     ",
               DataCriacaoArquivo->tm_diames,
               DataCriacaoArquivo->tm_mes + 1,
               DataCriacaoArquivo->tm_ano + 1900,
               DataCriacaoArquivo->tm_hora,
               DataCriacaoArquivo->tm_minuto,
               DataCriacaoArquivo->tm_segundo);
			   
        ret = PX_DOSEncontrarProximo();
		
		numeroArquivos += 1;
		
    }
	
	printf("\nTotal de arquivos: %d arquivo(s).\n", numeroArquivos);
	
    return;
}

//***************************************************************************

static void alterardir(char *para)
{
    PX_DOSAlterarDir(para);
	
    return;
}

//***************************************************************************

static void alterardisco(int drive)
{
    PX_DOSSelecionarDisco(paramaiusculo(drive) - 'A');
	
    return;
}

//***************************************************************************

static void copiararquivo(char *nome)
{
    char armazenamento[512];
    char *fonte;
    char *destino;
    ARQUIVO *arquivofonte;
    ARQUIVO *arquivodestino;
    int bytes_lidos = 0; 
    
    fonte = nome;
    destino = strchr(nome,' ');
     
    if (destino == NULL)
    {
        printf("\nDois nomes sao necessarios para a copia.\n\n");
        return;
    }
    
    *destino++ ='\0';
        
    printf("\nCopiando '%s' como '%s'...\n", fonte, destino);
       
    arquivofonte = abrir(fonte,"rb");
    
    if (arquivofonte != NULL)
    {
	
        arquivodestino = abrir(destino,"wb");
        
        if (arquivodestino != NULL)
        {
		
        while((bytes_lidos = ArquivoLer(armazenamento,1,sizeof(armazenamento),arquivofonte)) != 0)
            {
			
                escArquivo(armazenamento,1,bytes_lidos,arquivodestino);
				
            }
            
            fechar(arquivodestino);  
			
        }
        
        else
        {
		
            printf("\nArquivo de destino '%s' nao encontrado.\n", destino);
			
        }
        
        fechar(arquivofonte);
    }
    
    else
    {
        printf("\nArquivo fonte '%s' nao encontrado.\n", fonte);
    }
      
    return;
    
}

//***************************************************************************

static void deletararquivo(char *nome_de_arquivo)
{    
    int ret;
    Bloco_Arquivo *atd;

    atd = PX_DOSObterATD();
	
    if(*nome_de_arquivo == '\0')
    {
	
        printf("\nUm nome de arquivo e necessario.\n");
		
        return;
		
    }
	
    ret = PX_DOSEncontrarPrimeiro(nome_de_arquivo,0x10);
    
    if(ret == 2)
    {
	
        printf("\nArquivo nao encontrado.\n");
		
        return;
		
    }
	
    while(ret == 0)
    {          

        PX_DOSDeletarArquivo(atd->nome_arquivo); 
		
        ret = PX_DOSEncontrarProximo();
		
		printf("\n\nArquivo '%s' deletado.\n\n", nome_de_arquivo);
		
    }
	
    return;
	
}

//***************************************************************************

static int compararString(char *um, char *dois)
{
    while (paramaiusculo(*um) == paramaiusculo(*dois))
    {
	
        if (*um == '\0')
        {
		
            return (0);
			
        }
		
        um++;
        dois++;
		
    }
	
    if (paramaiusculo(*um) < paramaiusculo(*dois))
    {
	
        return (-1);
		
    }
	
    return (1);
}

//***************************************************************************

static int compararStringeTamanho(char *um, char *dois, tamanho_t tamanho)
{
    tamanho_t x = 0;
    
    if (tamanho == 0) return (0);
	
    while ((x < tamanho) && (paramaiusculo(*um) == paramaiusculo(*dois)))
    {
	
        if (*um == '\0')
        {
		
            return (0);
			
        }
		
        um++;
		
        dois++;
		
        x++;
    }
	
    if (x == tamanho) return (0);
	
    return (paramaiusculo(*um) - paramaiusculo(*dois));
	
}

//***************************************************************************

static void lerArquivoEmLotes(char *nome)
{
    ARQUIVO *ArquivoEmDisco;
    
    ArquivoEmDisco = abrir(nome, "r");
	

    if (ArquivoEmDisco != NULL)
    {        
	
        while (fgets(buf, sizeof buf, ArquivoEmDisco) != NULL)
        {
		
            processarentrada();
			
        }
		
        fechar(ArquivoEmDisco);
    }
	
    return;
	
}

//***************************************************************************

void putch(int a)
{

    putchar(a);
	
    fflush(stdout);
	
}

//***************************************************************************

void sino(void)
{

    putch('\a');
	
}

//***************************************************************************

void obter_seguro(char *buffer, int tamanho)
{
    int a;
    int shift;
    int i = 0;

    while (1)
    {

        a = PX_DOSObterCharSemEcho();


        if (i == tamanho)
        {
		
            buffer[tamanho] = '\0';
      
            if ((a == '\n') || (a == '\r'))
            return;

            sino();
			
        }

        if ((i == 0) && (a == '\b'))
        continue;

        if (i < tamanho)
        {

            if ((a == '\n') || (a == '\r'))
            {
			
                putch('\n');

                buffer[i] = '\0';

                return;
				
            }


            if (a == '\b')
            {
        
                if (i == 0)
                continue;
        
                else
                i--;

                putch('\b');
                putch(' ');
                putch('\b');

                continue;
				
            }
			
            else
			
            putch(a);


            if (eprint((unsigned char)a))
            {
			
                buffer[i] = a;
                i++;
				
            }
			
            else sino();

        }
		
        else sino();

    }

}

//***************************************************************************

void limpar_saida(void){

fflush(stdout);

}

//***************************************************************************