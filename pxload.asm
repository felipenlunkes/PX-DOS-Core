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

;; C�digo de in�cio para Loader



% .model memodel, c

extrn Carregador:proc

public ___iniciacao
public __saira


_DADOS   segment word public 'DATA'
_DADOS   ends
_BSS    segment word public 'BSS'
_BSS    ends

_TEXT segment word public 'CODE'

org 0100h

top:



;;*********************************************************************/

___iniciacao proc

nop
nop
mov ax, 0005h


jmp inicializacao

;;*********************************************************************/

padrao:

push cs
push cs
pop ds
pop es

;; Aloca��o inicial

mov bx, 01111h
mov ah, 4ah
int 21h

jmp novaspaginas

;;*********************************************************************/

inicializacao:

push cs
pop ax
sub ax, 010h

lea bx, novoinicio

push ax
push bx
retf 

;;*********************************************************************/

novoinicio:

push ax
push ax
pop es
pop ds
mov ss, ax
mov sp, 0fffeh

;;*********************************************************************/

novaspaginas:

call near ptr Carregador

sub sp,2
mov ax, 0
push ax

call near ptr __saira

___iniciacao endp

;;*********************************************************************/

ifdef NECESSARIO_EXIBIRC

;;*********************************************************************/

public displayc

;;*********************************************************************/

displayc proc

push ax
push bx
mov ah, 0eh
mov al, 043h
mov bl, 0
mov bh, 0
int 10h
pop bx
pop ax
ret

displayc endp
;;*********************************************************************/

endif

;;*********************************************************************/

__saira proc

pop ax
pop ax
mov ah, 02h
int 90h

__saira endp

;;*********************************************************************/

_TEXT ends
          
end top
