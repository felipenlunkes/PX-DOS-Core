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

// Fun��es em baixo n�vel 

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
