;;/*********************************************************************/
;;/*                                                                   */
;;/*                                                                   */
;;/*                                                                   */
;;/*                                                                   */
;;/*   #$$%%@!#$%                                                      */
;;/*   !!@#!$!$!$         Sistema Operacional PX-DOS                   */
;;/*   !@@#   #$%                                                      */
;;/*   #$$%   &*$                                                      */
;;/*   $#%@   @#&                                                      */
;;/*   #%$&*(@*@&                                                      */
;;/*   @#$@$#@$%$       2013-2022 (c) Felipe Miguel Nery Lunkes        */
;;/*   $%&*                Todos os direitos reservados                */
;;/*   @#&*                                                            */
;;/*   @&*%       Esse software se baseia em códigos disponíveis       */
;;/*   #&*@                     em domínio público                     */
;;/*                                                                   */
;;/*                                                                   */
;;/*********************************************************************/
;;/*
;;
;; Copyright (c) 2013-2022, Felipe Miguel Nery Lunkes
;; All rights reserved.
;;
;; Redistribution and use in source and binary forms, with or without
;; modification, are permitted provided that the following conditions are met:
;;
;; * Redistributions of source code must retain the above copyright notice, this
;;   list of conditions and the following disclaimer.
;;
;; * Redistributions in binary form must reproduce the above copyright notice,
;;   this list of conditions and the following disclaimer in the documentation
;;   and/or other materials provided with the distribution.
;;
;; * Neither the name of the copyright holder nor the names of its
;;   contributors may be used to endorse or promote products derived from
;;   this software without specific prior written permission.
;;
;; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;; AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;; IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
;; DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
;; FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
;; DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
;; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
;; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
;; OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
;; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;; */

;; Suporte em baixo n�vel para chamadas DOS

% .model memodel, c

assume cs:_TEXTO, ds:DGROUP

_DADOS   segment word public 'DATA'
_DADOS   ends
_BSS    segment word public 'BSS'
_BSS    ends

_TEXTO segment word public 'CODE'

public int86

;;*********************************************************************/

int86 proc uses ax bx cx dx si di ds es, \
           numeroint:word, registradoresentrada:ptr, registradoressaida:ptr

if @DataSize
  lds si, registradoresentrada
else
  mov si, registradoresentrada
endif

mov ax, word ptr [si + 0]
mov bx, word ptr [si + 2]
mov cx, word ptr [si + 4]
mov dx, word ptr [si + 6]
mov di, word ptr [si + 10]
mov si, word ptr [si + 8]

push bp

cmp numeroint, 08h
jne nao8
int 08h
jmp tentarint

;;*********************************************************************/

nao8:

cmp numeroint, 09h
jne nao9
int 09h
jmp tentarint

;;*********************************************************************/

nao9:

cmp numeroint, 010h
jne nao10
int 010h
jmp tentarint

;;*********************************************************************/

nao10:

cmp numeroint, 013h
jne nao13
int 013h
jmp tentarint

;;*********************************************************************/

nao13:

cmp numeroint, 015h
jne nao15
clc 
int 015h
jmp tentarint

;;*********************************************************************/

nao15:

cmp numeroint, 016h
jne nao16
int 016h
jmp tentarint

;;*********************************************************************/

nao16:

cmp numeroint, 020h
jne nao20
int 020h
jmp tentarint

;;*********************************************************************/

nao20:

cmp numeroint, 021h
jne nao21
int 021h
jmp tentarint

;;*********************************************************************/

nao21:

cmp numeroint, 090h
jne nao90
int 90h
jmp tentarint

;;*********************************************************************/

nao90:

tentarint:

pop bp
push si

if @DataSize
  lds si, registradoressaida
else
  mov si, registradoressaida
endif

mov [si + 0], ax
mov [si + 2], bx
mov [si + 4], cx
mov [si + 6], dx
mov [si + 10], di
pop ax 
mov [si + 8], ax
mov word ptr [si + 12], 0

jnc limparflag

mov word ptr [si + 12], 1

;;*********************************************************************/

limparflag:

pushf
pop ax
mov word ptr [si + 14], ax

ret

int86 endp


;;*********************************************************************/

public int86x

;;*********************************************************************/

int86x proc uses ax bx cx dx si di ds es, \
            numeroint:word, registradoresentrada:ptr, registradoressaida:ptr, sregistradores:ptr

push ds

if @DataSize
  lds si, sregistradores
else
  mov si, sregistradores
endif

mov es, [si + 6]
push es 

mov es, [si + 0]

if @DataSize
  lds si, registradoresentrada
else
  mov si, registradoresentrada
endif

mov ax, word ptr [si + 0]
mov bx, word ptr [si + 2]
mov cx, word ptr [si + 4]
mov dx, word ptr [si + 6]
mov di, word ptr [si + 10]
mov si, word ptr [si + 8]

pop ds

cmp numeroint, 08h
jne xnao8
int 08h
jmp xtentarint

;;*********************************************************************/

xnao8:

cmp numeroint, 09h
jne xnao9
int 09h
jmp xtentarint

;;*********************************************************************/

xnao9:

cmp numeroint, 010h
jne xnao10
int 010h
jmp xtentarint

;;*********************************************************************/

xnao10:

cmp numeroint, 013h
jne xnao13
int 013h
jmp xtentarint

;;*********************************************************************/

xnao13:

cmp numeroint, 015h
jne xnao15
clc 
int 015h
jmp xtentarint

;;*********************************************************************/

xnao15:

cmp numeroint, 016h
jne xnao16
int 016h
jmp xtentarint

;;*********************************************************************/

xnao16:

cmp numeroint, 021h
jne xnao21
int 021h
jmp xtentarint

;;*********************************************************************/

xnao21:

cmp numeroint, 090h
jne xnao90
int 90h
jmp xtentarint

;;*********************************************************************/

xnao90:


xtentarint:

push es
push ds
push si
push ax
push bp

mov bp, sp
mov ax, [bp+10]
pop bp
mov ds, ax

if @DataSize
  lds si, registradoressaida
else
  mov si, registradoressaida
endif

pop ax
mov [si + 0], ax
mov [si + 2], bx
mov [si + 4], cx
mov [si + 6], dx
mov [si + 10], di
pop ax
mov [si + 8], ax ; si
mov word ptr [si + 12], 0

jnc xlimparflag

mov word ptr [si + 12], 1

;;*********************************************************************/

xlimparflag:

if @DataSize
  lds si, sregistradores
else
  mov si, sregistradores
endif

pop ax
mov [si + 6], ax ; Salvar ds
pop ax
mov [si + 0], ax ; Salvar es

pop ds  
ret

int86x endp

_TEXTO ends

end
