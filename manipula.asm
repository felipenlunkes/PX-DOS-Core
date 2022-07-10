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

% .model memodel, c

extrn int0: proc
extrn int20:proc
extrn int21:proc
extrn int90:proc
extrn intctrlc:proc

public instalarInt
public manipulador20h
public manipulador21h
public EnviarDadosSerial

        .code
		
;; Manipulador de Dados para a porta serial

EnviarDadosSerial proc uses ax bx dx si

 lodsb         ;; Carrega o próximo caractere à ser enviado

 or al, al     ;; Compara o caractere com o fim da mensagem
 jz pronto    ;; Se igual ao fim, pula para .pronto

 mov ah, 0x1   ;; Função de envio de caractere do BIOS por Porta Serial
 int 0x14      ;; Chama o BIOS e executa a ação 

 jc erro

 jmp EnviarDadosSerial ;; Se não tiver acabado, volta à função e carrega o próximo caractere


 pronto: ;; Se tiver acabado...

pop ax
mov [si + 6], ax
pop ax
mov [si + 0], ax 

pop ds  
ret


 ret      ;; Retorna a função que o chamou

 erro:
 
pop ax
mov [si + 6], ax
pop ax
mov [si + 0], ax 

pop ds  
ret

 
 ret

EnviarDadosSerial endp

instalarInt proc uses bx es  ;; Instala as interrupções do PX-DOS

        mov bx, 0
        push bx
        pop es
		
        cli     ;; Desativa as Interrupções para a instalação das mesmas
		
	;;***************************************************************
	;;
	;; Interrupções do DOS (int 20h e int 21h) - declaração
	;;
	;;***************************************************************
		
		;; Interrupção 0h - Divisão por Zero
		
		mov bx, offset manipulador0h
        mov es:[00h], bx                ;; 0h = (int [0h * 4] = 0h)
        mov bx, seg manipulador0h
        mov es:[02h], bx
		
		;; Interrupção 20h - Terminar aplicativo
		
        mov bx, offset manipulador20h
        mov es:[80h], bx                ;; 80h = (int [20h * 4] = 80h)
        mov bx, seg manipulador20h
        mov es:[82h], bx
		
		;; Interrupção 21h - Serviços PX-DOS básicos
		
        mov bx, offset manipulador21h
        mov es:[84h], bx                ;; 84h = (int [21h * 4] = 84h)
        mov bx, seg manipulador21h
        mov es:[86h], bx
	
    ;;***************************************************************	
	;;
	;; Fim das interrupções do DOS (int 20h e int 21h legadas) - declaração
	;;
    ;;***************************************************************

	
	;;***************************************************************
    ;;
    ;; Início da declaração da interrupção exclusiva do PX-DOS ( int 90h )
	;;                   Serviços PX-DOS completos
    ;;
    ;;***************************************************************	
	
		mov ax, 0       
        mov es, ax
       
        mov al, 90h                   ;; Calcular o endereco do vetor para int 90h
       
        mov bl, 4h                    ;; Multiplicar 90h por 4 
        mul bl          
        mov bx, ax
       
        mov si, offset [interrupcao90h]  ;; Copiar o deslocamento para o vetor
        mov es:[bx], si
        add bx, 2   
         
        mov ax, cs     
        mov es:[bx], ax               ;; Copia o segmento para o vetor
		
	;;***************************************************************
    ;;
    ;; Fim da declaração da interrupção exclusiva do PX-DOS ( int 90h )
    ;;
    ;;***************************************************************
    
	;;***************************************************************
    ;;
    ;; Início da declaração da interrupção Ctrl-C ( int 1Bh )
    ;;
    ;;***************************************************************	
	
		mov ax, 0       
        mov es, ax
       
        mov al, 1Bh                   ;; Calcular o endereco do vetor para int 90h
       
        mov bl, 4h                    ;; Multiplicar 90h por 4 
        mul bl          
        mov bx, ax
       
        mov si, offset [interrupcao1Bh]  ;; Copiar o deslocamento para o vetor
        mov es:[bx], si
        add bx, 2   
         
        mov ax, cs     
        mov es:[bx], ax               ;; Copia o segmento para o vetor
		
	;;***************************************************************
    ;;
    ;; Fim da declaração da interrupção exclusiva do PX-DOS ( int 90h )
    ;;
    ;;***************************************************************
	
        sti     ;; Restaura as interrupções
        ret     ;; Retorna
		
instalarInt endp


;;*********************************************************************/

interrupcao90h proc  ;; Instala a interrupção 90h

        push ax
        push ax   
        push bx
        push cx
        push dx
        push si
        push di
        push ds
        push es
		
		mov dx, DGROUP
		
        mov ds, dx
        mov ax, sp
		
        push ss
        push ax
		
        call int90
		
        add sp, 4
        
        pop es
        pop ds
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax 
		
		cmp ax, 0
        je limpar90
		
        jmp naolimpar90
		
		
		limpar90:
        
	    pop ax
        push bp
        mov bp, sp
        and word ptr [bp+6],0fffeh
        pop bp
		
        iret
		
       naolimpar90:        
       
	    pop ax
        push bp
        mov bp, sp
        or word ptr [bp+6],0001h
        pop bp
		
        iret
		
interrupcao90h endp		

;;*********************************************************************/

interrupcao1Bh proc  ;; Instala a interrupção 90h

        push ax
        push ax   
        push bx
        push cx
        push dx
        push si
        push di
        push ds
        push es
		
		mov dx, DGROUP
		
        mov ds, dx
        mov ax, sp
		
        push ss
        push ax
		
        call intctrlc
		
        add sp, 4
        
        pop es
        pop ds
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax 
		
		cmp ax, 0
        je limpar1Bh
		
        jmp naolimpar1Bh
		
		
		limpar1Bh:
        
	    pop ax
        push bp
        mov bp, sp
        and word ptr [bp+6],0fffeh
        pop bp
		
        iret
		
       naolimpar1Bh:        
       
	    pop ax
        push bp
        mov bp, sp
        or word ptr [bp+6],0001h
        pop bp
		
        iret
		
interrupcao1Bh endp		

;;*********************************************************************/
		
manipulador0h proc  ;; Instala a interrupção 0h

        push ax
        push ax   
        push bx
        push cx
        push dx
        push si
        push di
        push ds
        push es

        mov dx, DGROUP
		
        mov ds, dx
        mov ax, sp
		
        push ss
        push ax
		
        call int0
		
        add sp, 4
        
        pop es
        pop ds
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax   

        cmp ax, 0
        je limpar0
		
        jmp naolimpar0
		
limpar0:

        pop ax
        push bp
        mov bp, sp
        and word ptr [bp+6],0fffeh
        pop bp
		
        iret
		
naolimpar0:

        pop ax
        push bp
        mov bp, sp
        or word ptr [bp+6],0001h
        pop bp
		
        iret
		
manipulador0h endp

;;*********************************************************************/
		
manipulador20h proc  ;; Instala a interrupção 20h

        push ax
        push ax   
        push bx
        push cx
        push dx
        push si
        push di
        push ds
        push es

        mov dx, DGROUP
		
        mov ds, dx
        mov ax, sp
		
        push ss
        push ax
		
        call int20
		
        add sp, 4
        
        pop es
        pop ds
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax   

        cmp ax, 0
        je limpar20
		
        jmp naolimpar20
		
limpar20:

        pop ax
        push bp
        mov bp, sp
        and word ptr [bp+6],0fffeh
        pop bp
		
        iret
		
naolimpar20:

        pop ax
        push bp
        mov bp, sp
        or word ptr [bp+6],0001h
        pop bp
		
        iret
		
manipulador20h endp

;;*********************************************************************/

manipulador21h proc  ;; Instala a interrupção 21h
       
	    push ax
        push ax   
        push bx
        push cx
        push dx
        push si
        push di
        push ds
        push es

        mov dx, DGROUP
		
        mov ds, dx
        mov ax, sp
		
        push ss
        push ax
		
        call int21
		
        add sp, 4
        
        pop es
        pop ds
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax  

        cmp ax, 0
        je limpar21
		
        jmp naolimpar21
		
limpar21:
        
		pop ax
        push bp
        mov bp, sp
        and word ptr [bp+6],0fffeh
        pop bp
		
        iret
		
naolimpar21:        
       
	    pop ax
        push bp
        mov bp, sp
        or word ptr [bp+6],0001h
        pop bp
		
        iret
		
manipulador21h endp

end
