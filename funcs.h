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
// Funções em baixo nível 

int PXDOS_LerSetornoDisco(void *buffer, 
                      unsigned long setores,
                      unsigned long drive,
                      unsigned long trilha,
                      unsigned long cabeca,
                      unsigned long setor);

int PXDOS_ParametrosdoDisco(unsigned long drive,
                  unsigned long *trilhas,
                  unsigned long *setores,
                  unsigned long *cabecas,
                  unsigned long *atacado,
                  unsigned char **tabelaparametros,
                  unsigned long *tipodedrive);

int PXDOS_ResetarDisco(unsigned long drive);

int PXDOS_StatusdoDisco(unsigned long drive, unsigned long *status);

int PXDOS_ImprimirTela(void);

int PXDOS_AlterarModoVideo(unsigned long modo);

int PXDOS_AlterarTipoCursor(int topo, int fundo);

int PXDOS_DefinirPosicaoCursor(int pagina, int linha, int coluna);

int PXDOS_LerPosicaoCursor(int pagina, 
                          int *InicioCursor,
                          int *FimCursor,
                          int *linha,
                          int *coluna);
						  
int PXDOS_LerCanetaDeLuz(int *gatilho,
                    unsigned long *pcolunas,
                    unsigned long *plinha1,
                    unsigned long *plinha2,
                    int *clinha,
                    int *ccoluna);
					
int PXDOS_RolarJanelaCima(int numeroLinhas,
                      int atributo,
                      int inicioLinha,
                      int inicioColuna,
                      int fimLinha,
                      int fimColuna);
					  
int PXDOS_RolarJanelaBaixo(int numeroLinhas,
                        int atributo,
                        int inicioLinha,
                        int inicioColuna,
                        int fimLinha,
                        int fimColuna);
						
int PXDOS_LerAtributoChar(int pagina, int *ch, int *atributo);

int PXDOS_EscreverAtributoChar(int pagina, int ch, int atributo, unsigned long num);

int PXDOS_ImprimirCursorChar(int pagina, int ch, int coluna, unsigned long num);

int PXDOS_DefinirPaletadeCores(int id, int val);

int PXDOS_ImprimirPixel(int pagina, 
                          int cor, 
                          unsigned long linha,
                          unsigned long coluna);
						  
int PXDOS_LerPixel(int pagina, 
                         unsigned long linha,
                         unsigned long coluna,
                         int *cor);
						 
int PXDOS_ImprimirTexto(int pagina, int ch, int cor);

int PXDOS_ObterModoVideo(int *colunas, int *modo, int *pagina);
