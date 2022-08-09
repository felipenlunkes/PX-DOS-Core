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
/*             PXDOS.h - Interface para o uso do BIOS                */
/*                                                                   */
/*********************************************************************/

int PXDOS_ImprimirTela(void); /* 5: */

int PXDOS_AlterarModoVideo(unsigned int modo); /* 10:0 */

int PXDOS_AlterarTipoCursor(int topo, int fundo); /* 10:1 */

int PXDOS_DefinirPosicaoCursor(int pagina, int linha, int coluna); /* 10:2 */

int PXDOS_LerPosicaoCursor(int pagina, /* 10:3 */
                          int *InicioCursor,
                          int *FimCursor,
                          int *linha,
                          int *coluna);
						  
int PXDOS_LerCanetaDeLuz(int *gatilho, /* 10:4 */
                    unsigned int *pcolunas,
                    unsigned int *plinha1,
                    unsigned int *plinha2,
                    int *clinha,
                    int *ccoluna);
					
int PXDOS_DefinirPaginaDisplayAtiva(int pagina); /* 10:5 */
                    
int PXDOS_RolarJanelaCima(int numeroLinhas, /* 10:6 */
                      int atributo,
                      int inicioLinha,
                      int inicioColuna,
                      int fimLinha,
                      int fimColuna);
					  
int PXDOS_RolarJanelaBaixo(int numeroLinhas, /* 10:7 */
                        int atributo,
                        int inicioLinha,
                        int inicioColuna,
                        int fimLinha,
                        int fimColuna);
						
int PXDOS_LerAtributoChar(int pagina, int *ch, int *atributo); /* 10:8 */

int PXDOS_EscreverAtributoChar(int pagina, int ch, int atributo, unsigned int num); /* 10:9 */

int PXDOS_ImprimirCursorChar(int pagina, int ch, int coluna, unsigned int num); /* 10:A */

int PXDOS_DefinirPaletadeCores(int id, int val); /* 10:B */

int PXDOS_ImprimirPixel(int pagina, /* 10:C */
                          int cor, 
                          unsigned int linha,
                          unsigned int coluna);
						  
int PXDOS_LerPixel(int pagina, /* 10:D */
                         unsigned int linha,
                         unsigned int coluna,
                         int *cor);
						 
int PXDOS_ImprimirTexto(int pagina, int ch, int cor); /* 10:E */

int PXDOS_ObterModoVideo(int *colunas, int *modo, int *pagina); /* 10:F */

int PXDOS_ResetarDisco(unsigned int drive); /* 13:0 */

int PXDOS_StatusdoDisco(unsigned int drive, unsigned int *status); /* 13:1 */

int PXDOS_LerSetornoDisco(void         *buffer, /* 13:2 */
                      unsigned int setores,
                      unsigned int drive,
                      unsigned int trilha,
                      unsigned int cabeca,
                      unsigned int setor);

int PXDOS_EscreverSetornoDisco(void         *buffer, /* 13:3 */
                       unsigned int  setores,
                       unsigned int  drive,
                       unsigned int  trilha,
                       unsigned int  cabeca,
                       unsigned int  setor);

					   int PXDOS_EscreverSetorLBA(void         *buffer,
                      unsigned int setores,
                      unsigned int drive,
                      unsigned long setor,
                      unsigned long setorOculto);
					  
					  
int PXDOS_ParametrosdoDisco(unsigned int drive, /* 13:8 */
                  unsigned int *trilhas,
                  unsigned int *setores,
                  unsigned int *cabecas,
                  unsigned int *atacado,
                  unsigned char **tabelaparametros,
                  unsigned int *tipodedrive);

int PXDOS_FixarStatusdoDisco(unsigned int drive); /* 13:10 */

int PXDOS_LerSetoremLBA(void *buffer, /* 13:42 */
                      unsigned int setores,
                      unsigned int drive,
                      unsigned long setor,
                      unsigned long setorSuperior);

long PXDOS_TamanhoMemoriaExtendida(void); /* 15:88 */

int PXDOS_LerCaracteredoTeclado(int *EscaniarCodigo, int *ascii); /* 16:0 */

int PXDOS_IniciarImpressora(unsigned int impressora);

int PXDOS_IniciarSerial(unsigned int porta);

int PXDOS_EnviarSerial(unsigned int porta, int caractere);

void PXDOS_NovoBoot(void); /* 19: */
