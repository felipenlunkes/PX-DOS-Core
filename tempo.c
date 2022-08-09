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

#include <time.h>

#include "tempo.h"


// Converte para formato DOS

void tempo_para_dos(tempo_t agora, void *dos)
{
    struct tm *ts;
    unsigned long x;
    unsigned char *arr = (unsigned char *)dos;

    ts = horalocal(&agora);
    x = ts->tm_ano - 80;
    x = (x << 4) | (ts->tm_mes + 1);
    x = (x << 5) | ts->tm_diames;
    x = (x << 5) | ts->tm_hora;
    x = (x << 6) | ts->tm_minuto;
    x = (x << 5) | (ts->tm_segundo / 2);
    arr[0] = (unsigned char)((x >> 16) & 0xff);
    arr[1] = (unsigned char)((x >> 24) & 0xff);
    arr[2] = (unsigned char)(x & 0xff);
    arr[3] = (unsigned char)((x >> 8) & 0xff);
}

/*****************************************************************************************/

tempo_t dos_para_tempo(void *dos)
{
    struct tm tms;
    tempo_t tt;
    unsigned long x;
    unsigned char *arr = (unsigned char *)dos;

    x = ((unsigned long)arr[0]) | ((unsigned long)arr[1] << 8) 
        | ((unsigned long)arr[2] << 16) | ((unsigned long)arr[3] << 24);
		
    tms.tm_segundo = (x & 0x1f) * 2;
	
    x >>= 5;
	
    tms.tm_minuto = x & 0x3f;
	
    x >>= 6;
	
    tms.tm_hora = x & 0x1f;
	
    x >>= 5;
	
    tms.tm_diames = x & 0x1f;
	
    x >>= 5;
	
    tms.tm_mes = (x & 0x0f) - 1;
	
    x >>= 4;
	
    tms.tm_ano = (x & 0x7f) + 80;
	
    tms.tm_eHorarioDeVerao = -1;
	
    tt = fazertempo(&tms);
	
    return (tt);
}

