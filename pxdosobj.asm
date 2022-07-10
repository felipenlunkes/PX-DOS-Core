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
;; Código inicial do PX-DOS


.model large

extrn _EntradaKernel:proc

_DADOS   segment word public 'DATA'
_DADOS   ends

_BSS    segment word public 'BSS'
_BSS    ends

_PILHA  segment word stack 'STACK'
        db 1000h dup(?)
_PILHA  ends

DGROUP  group   _DADOS,_BSS
        assume cs:_TEXTO,ds:DGROUP

_TEXTO segment word public 'CODE'

top:

public ___inicializacao

;;*********************************************************************/

___inicializacao proc

push cs
push cs
pop ds
pop es
mov ax, cs
mov ss, ax
mov sp, 0fffeh

mov ax, DGROUP
mov ds, ax


call far ptr _EntradaKernel ;; Pula para o Kernel


sub sp,2
mov ax, 0
push ax

call far ptr ___saira

___inicializacao endp

;;*********************************************************************/

public ___saira

___saira proc

pop ax
pop ax

mov ah, 02h
int 90h

___saira endp

_TEXTO ends
          
end top
