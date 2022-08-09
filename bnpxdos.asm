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

;;
;; Fun��es em baixo n�vel
;;

% .model memodel, c

public obterLongo, colocarLongo, rportb, wportb, ativar, desativar
public chamarlongo, chamarcompsp, chamareignorar, A20_ativar
public reiniciar, colocarAbs, obterAbs, desligar

;;*********************************************************************/

        .code
		
obterLongo proc, endereco: dword

        push ds
        lds bx,endereco
        mov ah,0
        mov al,[bx]
        pop ds
        ret
		
obterLongo endp

;;*********************************************************************/

colocarLongo proc, endereco2: dword, caracter: word

        push ds
		
        lds bx,endereco2           ; Receber
		
        mov al,byte ptr caracter   ; Exibir um caractere
		
        mov [bx],al
        pop ds
        ret
		
colocarLongo endp

;;*********************************************************************/

rportb proc, porta: word

        mov dx,porta
        in al,dx
        ret
		
rportb endp

;;*********************************************************************/

wportb proc, porta2: word, bytesaida: byte

        mov dx,porta2
        mov al,bytesaida
        out dx,al
        ret
		
wportb endp

;;*********************************************************************/

ativar  proc 

        sti
        ret
		
ativar  endp

;;*********************************************************************/

desativar proc 

        cli
        ret
		
desativar endp

;;*********************************************************************/

chamarlongo proc, endereco: dword

        push bx
        push ds
        push ax
        
        lds bx, endereco
        lea ax, chamarlongo_ret
        push cs
        push ax
        push ds
        push bx
        
        retf     ; Chamar rotina desejada
		
chamarlongo_ret:

        pop ax
        pop ds
        pop bx
        ret
		
chamarlongo endp

;;*********************************************************************/

chamarcompsp proc, endereco: dword, psp: dword, ss_novo: word, sp_novo: word

        push bx
        push ds
        push cx
        push es
        push si
        
        jmp short novaspaginas

;;*********************************************************************/
		
ss_salvo  dw ?
sp_salvo  dw ?
 
;;*********************************************************************/
 
novaspaginas:

        push ss_salvo
        push sp_salvo
                
        mov ss_salvo, ss
        mov sp_salvo, sp
        
        lea ax, chamarcompsp_ret
        lds bx, endereco
        les cx, psp
        
        mov cx, ss_novo
        mov si, sp_novo
        cli
        mov ss, cx
        mov sp, si
        sti
        
        push cs
        push ax

; O MS-DOS coloca um 0 no fim quando volta da interrup��o
        
        mov ax, 0
        push ax
                
        push ds
        push bx

        push es
        pop ds

        mov ax, 0ffffh
        
        retf    ; Chamar rotina desejada

;;*********************************************************************/

chamareignorar label near

        pop ax ; pular endere�o de retorno,
        pop ax ;   
        pop ax ; Obter c�digo de retorno

;;*********************************************************************/
		
chamarcompsp_ret:

        cli
        mov ss, ss_salvo
        mov sp, sp_salvo
        sti
        
        pop sp_salvo
        pop ss_salvo

        pop si        
        pop es
        pop cx
        pop ds
        pop bx
        ret
		
chamarcompsp endp

;;*********************************************************************/
       
; Ativar A20

A20_ativar proc

a20_tentar:

        in al, 064h
        test al, 2
        jnz a20_tentar
        mov al, 0dfh
        out 060h, al
        ret
		
A20_ativar endp

;;*********************************************************************/

reiniciar proc

        push es
        mov ax, 040h
        mov es, ax
        mov word ptr es:[072h], 01234h
        

        mov ax, 0ffffh
        push ax
        mov ax, 0
        push ax
        retf
        ret
		
reiniciar endp

;;*********************************************************************/

obterAbs proc, endereco3: dword

        push ds
        push bx
        push cx
        lds bx,endereco3
        mov ax, ds
        mov cl, 12
        shl ax, cl
        mov ds, ax
        mov ah,0
        mov al,[bx]
        pop cx
        pop bx
        pop ds
        ret
		
obterAbs endp

;;*********************************************************************/

colocarAbs proc, endereco4: dword, caracter: word

        push ds
        push bx
        push cx
        lds bx,endereco4
        mov ax, ds
        mov cl, 12
        shl ax, cl
        mov ds, ax        
        mov ax, caracter
        mov [bx],al
        pop cx
        pop bx
        pop ds
        ret
		
colocarAbs endp

;;*********************************************************************/

desligar proc ;; Fun��o para o desligamento do computador, acess�vel 
              ;; apenas ap�s a passagem por m�todos de seguran�a
			  
;;*********************************************************************/
;;
;;             Parte da implementa��o APM do PX-DOS�
;;
;;        Copyright � 2013-2016 Felipe Miguel Nery Lunkes
;;
;;*********************************************************************/

;;*********************************************************************/
;;
;; Esta fun��o pode retornar c�digos de erro, os quais se seguem:
;;
;;
;; Retorno em AX - c�digo de erro:
;;
;; 0 = Falha na instala��o do Driver
;; 1 = Falha na conex�o de interface de Modo Real
;; 2 = Driver APM vers�o 1.2 n�o suportado
;; 3 = Falha ao alterar o status para "off"
;;
;;*********************************************************************/

desligar_sistema:

	push bx
	push cx

	mov ax, 5300h		;; Fun��o de checagem da instala��o
	mov bx, 0			;; O ID do dispositivo (APM BIOS)
	int 15h				;; Chamar interrup��o APM
	
	jc APM_falha_instalacao

	mov ax, 5301h		;; Fun��o de interface de conex�o em modo real
	mov bx, 0			;; O ID do dispositivo (APM BIOS)
	int 15h				;; Chamar interrup��o APM
	
	jc APM_falha_conexao

	mov ax, 530Eh		;; Fun��o de sele��o de vers�o do Driver
	mov bx, 0			;; O ID do dispositivo (APM BIOS)
	mov cx, 0102h		;; Selecionar APM vers�o 1.2
						;; A funcionalidade est� presente ap�s a vers�o 1.2
	int 15h				;; Chamar interrup��o APM
	
	jc APM_falha_selecionar_versao

	mov ax, 5307h		;; Fun��o de definir estado
	mov cx, 0003h		;; Estado de desligar
	mov bx, 0001h		;; Todos os dispositivos tem ID 1
	int 15h				;; Chamar interrup��o APM
	
;; Caso o sistema n�o desligue de forma apropriada, ser�o retornados c�digos de erro ao
;; programa que chamou a fun��o de desligamento.
	
APM_falha_comando: ;; Chamado caso o comando de desligamento (c�digo 3) n�o seja executado

	mov ax, 3
	jmp APM_desligamento_ok

APM_falha_instalacao: ;; Chamado caso ocorra falha na instala��o

	mov ax, 0
	jmp APM_desligamento_ok
	
APM_falha_conexao: ;; Chamado caso ocorra falha na conex�o de interface de Modo Real

	mov ax, 1
	jmp APM_desligamento_ok
	
APM_falha_selecionar_versao: ;; Chamado quando a vers�o APM � inferior a 1.2

	mov ax, 2
	
APM_desligamento_ok: ;; Retorna a fun��o que a chamou

	pop cx
	pop bx
	
	ret
	
desligar endp
	
end
