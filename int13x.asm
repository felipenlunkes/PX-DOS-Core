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

;; Interrupção e interfaces para acesso ao disco
 


% .model memodel, c

assume cs:_TEXTO

_TEXTO segment word public 'CODE'

public int13x

;;*********************************************************************/

int13x proc uses ax bx cx dx si di ds es, \
            registradoresentrada:ptr, registradoressaida:ptr, sregistradores:ptr

push ds

  mov si, sregistradores


mov es, [si + 6]
push es 

mov es, [si + 0]

  mov si, registradoresentrada


mov ax, word ptr [si + 0]
mov bx, word ptr [si + 2]
mov cx, word ptr [si + 4]
mov dx, word ptr [si + 6]
mov di, word ptr [si + 10]
mov si, word ptr [si + 8]

pop ds

int 013h

push es
push ds
push si
push ax
push bp

mov bp, sp
mov ax, [bp+10]
pop bp
mov ds, ax


  mov si, registradoressaida


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


  mov si, sregistradores


pop ax
mov [si + 6], ax
pop ax
mov [si + 0], ax 

pop ds  
ret

int13x endp

_TEXTO ends

end
