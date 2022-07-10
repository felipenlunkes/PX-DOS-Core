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

;; Código de início para Loader



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

;; Alocação inicial

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
