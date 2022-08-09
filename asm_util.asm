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

;; Rotinas requeridas para compiladores DOS

% .model memodel

;;*********************************************************************/
;;
;;
;; Fun��es matem�ticas para os m�dulos em C
;;
;;
;;*********************************************************************/


extrn ___divide:proc
extrn ___modulo:proc

assume cs:_TEXTO,ds:_TEXTO

_TEXTO segment word public 'CODE'

public n_ludiv@

;;*********************************************************************/

n_ludiv@ proc

push bp
mov bp,sp
push bx

cmp word ptr [bp + 10], 0

jne ludiv_full

mov ax, [bp + 6]
mov dx, 0
div word ptr [bp + 8]
mov bx, ax
mov ax, [bp + 4]
div word ptr [bp + 8]

mov dx, bx

jmp short ludiv_fin

;;*********************************************************************/

ludiv_full:

push word ptr [bp + 10]
push word ptr [bp + 8]
push word ptr [bp + 6]
push word ptr [bp + 4]

call near ptr ___divide

add sp, 8

;;*********************************************************************/

ludiv_fin:

pop bx
pop bp
ret 8

n_ludiv@ endp

;;*********************************************************************/

public n_lumod@

;;*********************************************************************/

n_lumod@ proc

push bp
mov bp,sp

cmp word ptr [bp + 10], 0

jne lumod_full

mov ax, [bp + 6]
mov dx, 0

div word ptr [bp + 8]

mov ax, [bp + 4]

div word ptr [bp + 8]

mov ax,dx
mov dx, 0

jmp short lumod_fin

;;*********************************************************************/

lumod_full:

push word ptr [bp + 10]
push word ptr [bp + 8]
push word ptr [bp + 6]
push word ptr [bp + 4]

call near ptr ___modulo

add sp, 8

;;*********************************************************************/

lumod_fin:

pop bp
ret 8

n_lumod@ endp

;;*********************************************************************/

; shift dx:ax por cl

public n_lxlsh@

;;*********************************************************************/

n_lxlsh@ proc

push bx

cmp cl, 24
jl lxlsh_16

mov dh, al
mov dl, 0
mov ax, 0
sub cl, 24

jmp short lxlsh_last

;;*********************************************************************/

lxlsh_16:

cmp cl, 16
jl lxlsh_8

mov dx, ax
mov ax, 0
sub cl, 16

jmp short lxlsh_last

;;*********************************************************************/

lxlsh_8:

cmp cl, 8
jl lxlsh_last

mov dh, dl
mov dl, ah
mov ah, al
mov al, 0
sub cl, 8

;jmp short lxlsh_last

;;*********************************************************************/

lxlsh_last:

mov ch, 8
sub ch, cl
xchg ch, cl
mov bx, ax
shr bx, cl
xchg ch, cl
shl dx, cl
or dl, bh
shl ax, cl

pop bx
ret

;;*********************************************************************/

n_lxlsh@ endp

;;*********************************************************************/

; Multiplica cx:bx por dx:ax, resultado em dx:ax

public __I4M

;;*********************************************************************/

__I4M:

;;*********************************************************************/

public __U4M

;;*********************************************************************/

__U4M:

;;*********************************************************************/

public n_lxmul@

;;*********************************************************************/

n_lxmul@ proc

push bp
mov bp,sp
push cx

push ax
mul cx
mov cx, ax
pop ax
mul bx
add dx, cx

pop cx
pop bp
ret

;;*********************************************************************/

n_lxmul@ endp

;;*********************************************************************/

; Shift dx:ax por cl

public n_lxursh@

;;*********************************************************************/

n_lxursh@ proc

push bx

cmp cl, 24
jl lxursh_16

mov al, dh
mov ah, 0
mov dx, 0
sub cl, 24

jmp short lxursh_last

;;*********************************************************************/

lxursh_16:

cmp cl, 16
jl lxursh_8

mov ax, dx
mov dx, 0
sub cl, 16

jmp short lxursh_last

;;*********************************************************************/

lxursh_8:

cmp cl, 8
jl lxursh_last

mov al, ah
mov ah, dl
mov dl, dh
mov dh, 0
sub cl, 8

;jmp short lxursh_last

;;*********************************************************************/

lxursh_last:

mov ch, 8
sub ch, cl
xchg ch, cl
mov bx, dx
shl bx, cl
xchg ch, cl
shr ax, cl
or ah, bl
shr dx, cl

pop bx
ret

;;*********************************************************************/

n_lxursh@ endp

_TEXTO ends

end
