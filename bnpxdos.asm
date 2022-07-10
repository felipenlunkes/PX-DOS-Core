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
;; Funções em baixo nível
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

; O MS-DOS coloca um 0 no fim quando volta da interrupção
        
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

        pop ax ; pular endereço de retorno,
        pop ax ;   
        pop ax ; Obter código de retorno

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

desligar proc ;; Função para o desligamento do computador, acessível 
              ;; apenas após a passagem por métodos de segurança
			  
;;*********************************************************************/
;;
;;             Parte da implementação APM do PX-DOS®
;;
;;        Copyright © 2013-2016 Felipe Miguel Nery Lunkes
;;
;;*********************************************************************/

;;*********************************************************************/
;;
;; Esta função pode retornar códigos de erro, os quais se seguem:
;;
;;
;; Retorno em AX - código de erro:
;;
;; 0 = Falha na instalação do Driver
;; 1 = Falha na conexão de interface de Modo Real
;; 2 = Driver APM versão 1.2 não suportado
;; 3 = Falha ao alterar o status para "off"
;;
;;*********************************************************************/

desligar_sistema:

	push bx
	push cx

	mov ax, 5300h		;; Função de checagem da instalação
	mov bx, 0			;; O ID do dispositivo (APM BIOS)
	int 15h				;; Chamar interrupção APM
	
	jc APM_falha_instalacao

	mov ax, 5301h		;; Função de interface de conexão em modo real
	mov bx, 0			;; O ID do dispositivo (APM BIOS)
	int 15h				;; Chamar interrupção APM
	
	jc APM_falha_conexao

	mov ax, 530Eh		;; Função de seleção de versão do Driver
	mov bx, 0			;; O ID do dispositivo (APM BIOS)
	mov cx, 0102h		;; Selecionar APM versão 1.2
						;; A funcionalidade está presente após a versão 1.2
	int 15h				;; Chamar interrupção APM
	
	jc APM_falha_selecionar_versao

	mov ax, 5307h		;; Função de definir estado
	mov cx, 0003h		;; Estado de desligar
	mov bx, 0001h		;; Todos os dispositivos tem ID 1
	int 15h				;; Chamar interrupção APM
	
;; Caso o sistema não desligue de forma apropriada, serão retornados códigos de erro ao
;; programa que chamou a função de desligamento.
	
APM_falha_comando: ;; Chamado caso o comando de desligamento (código 3) não seja executado

	mov ax, 3
	jmp APM_desligamento_ok

APM_falha_instalacao: ;; Chamado caso ocorra falha na instalação

	mov ax, 0
	jmp APM_desligamento_ok
	
APM_falha_conexao: ;; Chamado caso ocorra falha na conexão de interface de Modo Real

	mov ax, 1
	jmp APM_desligamento_ok
	
APM_falha_selecionar_versao: ;; Chamado quando a versão APM é inferior a 1.2

	mov ax, 2
	
APM_desligamento_ok: ;; Retorna a função que a chamou

	pop cx
	pop bx
	
	ret
	
desligar endp
	
end
