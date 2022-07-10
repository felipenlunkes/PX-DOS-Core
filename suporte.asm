;;********************************************************************/
;;                                                                   */
;;                                                                   */
;;                                                                   */
;;                                                                   */
;;   #$$%%@!#$%                                                      */
;;   !!@#!$!$!$         Sistema Operacional PX-DOS ®                 */
;;   !@@#   #$%                                                      */
;;   #$$%   &*$                                                      */
;;   $#%@   @#&                                                      */
;;   #%$&*(@*@&                                                      */
;;   @#$@$#@$%$     © 2013-2016 Felipe Miguel Nery Lunkes            */
;;   $%&*                Todos os direitos reservados                */
;;   @#&*                                                            */
;;   @&*%                                                            */
;;   #&*@                                                            */
;;                                                                   */
;;                                                                   */
;; O PX-DOS ® é marca registrada de Felipe Miguel Nery Lunkes no     */
;; Brasil. © 2013-2016 Felipe Miguel Nery Lunkes. Todos os direitos  */
;; reservados. A reprodução total ou parcial, de quaisquer trechos   */
;; do código aqui presente é expressamente probida, sendo passível   */
;; de punição legal severa.                                          */
;;                                                                   */
;; Copyright © 2013-2016 Felipe Miguel Nery Lunkes                   */
;; Todos os direitos reservados.                                     */
;;                                                                   */
;;********************************************************************/ 
;;

;; Suporte em baixo nível para chamadas DOS


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
