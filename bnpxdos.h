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


#define ESCByteNaPorta(x, y) wportb(x, y)

#define LERByteNaPorta(x) rportb(x)

#define PararInterrupcoes() desativar()

#define HabilitarInterrupcoes() ativar()

#define DOSPREF

int DOSPREF obterLongo(long endereco);

void DOSPREF colocarLongo(long endereco, unsigned ch);

int DOSPREF rportb(int porta);

void DOSPREF wportb(int porta, unsigned ch);

void DOSPREF desativar(void);

void DOSPREF ativar(void);

void DOSPREF chamarlongo(unsigned long endereco);

int DOSPREF chamarcompsp(char *endereco,
                        char *psp,
                        unsigned int ss_novo,
                        unsigned int sp_novo);
						
void DOSPREF chamareignorar(int codretorno);

void DOSPREF A20_ativar(void);

void DOSPREF reiniciar(void);

int DOSPREF obterAbs(long endereco);

void DOSPREF colocarAbs(long endereco, unsigned ch);

int DOSPREF desligar(void);

