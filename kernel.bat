@echo off
set ver=0.9.0
del PXDOS.SIS
cls
title Compilador do Kernel do PX-DOS(R) versao %ver% 
cls
echo.
echo.
echo Ferramenta de construcao do Nucleo (Kernel) do Sistema
echo Operacional PX-DOS(R) versao %ver%
echo.
echo.
echo Esta ferramenta permite compilar o Kernel do PX-DOS(R).
echo.
echo Sistema Operacional PX-DOS(R). 
echo Copyright (C) 2013-2016 Felipe Miguel Nery Lunkes
echo Todos os direitos reservados
echo.
echo.
echo Escolha o modo de compilacao desejado:
echo.
echo.
set/p opt="Use 1 para modo Debug e 2 para compilacao sem Debug: "
echo. >> pxdos.log
echo Sistema Operacional PX-DOS(R)>> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Copyright (C) 2013-2016 Felipe Miguel Nery Lunkes >> pxdos.log
echo Todos os direitos reservados >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Iniciando a compilacao do Kernel do PX-DOS(R)... >> pxdos.log
echo Iniciando log... >> pxdos.log
echo. >> pxdos.log
echo Horario do inicio do log: [%TIME%] >> pxdos.log
echo. >> pxdos.log
echo Data desta compilacao: [%DATE%] >> pxdos.log
echo. >> pxdos.log
echo Arquitetura do Sistema Operacional utilizado para a compilacao: [%PROCESSOR_ARCHITECTURE%] >> pxdos.log
echo. >> pxdos.log
echo Sistema Operacional: [%OS%] >> pxdos.log
echo. >> pxdos.log
echo Versao atual do Kernel compilado: [%ver%] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Dados do Kernel PX-DOS(R) por Felipe Miguel Nery Lunkes: >> pxdos.log
echo. >> pxdos.log
echo Nome de código do Kernel: >> pxdos.log
echo. >> pxdos.log
type apelidokrnl.px >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Tipo de Kernel: [16 Bits] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Usando diretivas: [Normal] >> pxdos.log
echo. >> pxdos.log


GOTO COMP%OPT%


:COMP1
cls
title Compilador do Kernel do PX-DOS(R) versao %ver% em Modo Debug

echo. >> pxdos.log
echo Usando opcao: [Debug] >> pxdos.log
echo Iniciando... >> pxdos.log

cls
wasmr -zq -zcm -Dmemodel=large pxdosobj.asm
wasmr -zq -zcm -Dmemodel=large suporte.asm
wasmr -zq -zcm -Dmemodel=large bnpxdos.asm
wasmr -zq -zcm -Dmemodel=large manipula.asm
wasmr -zq -zcm -Dmemodel=large pxdosasm.asm
tcc -O -c -ml memoria.c formato.c patmat.c
tcc -O -c -ml bios.c fat.c string.c caracter.c pxdospc.c
tcc -O -j1 -c -ml pxdosn.c 
tlink -x pxdosobj+pxdosn+bios+suporte+fat+pxdosasm+string+pxdospc+bnpxdos+manipula+memoria+formato+caracter+patmat,PXDOS.SIS



if exist PXDOS.SIS ( GOTO SUCESSO1 ) else ( GOTO ERRO1 )

:SUCESSO1

echo Status do Kernel: [Compilado] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Kernel compilado com sucesso. >> pxdos.log

title Sucesso ao compilar o Kernel do PX-DOS(R) versao %VER%
echo.
echo.
echo A compilacao do Kernel do PX-DOS(R) foi bem sucedida.
echo.
echo.
GOTO FIM

:ERRO1
echo.
echo. >> pxdos.log
echo Status do Kernel: [Falha] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Falha ao compilar o Kernel. >> pxdos.log
echo Um erro ocorreu durante a compilacao do Kernel.
GOTO FIM

:COMP2

cls
title Compilador do Kernel do PX-DOS(R) versao %ver% sem Modo Debug
echo. >> pxdos.log
echo Usando opcao: [Normal] >> pxdos.log
echo Iniciando... >> pxdos.log
cls
wasmr -zq -zcm -Dmemodel=large pxdosobj.asm
wasmr -zq -zcm -Dmemodel=large suporte.asm
wasmr -zq -zcm -Dmemodel=large bnpxdos.asm
wasmr -zq -zcm -Dmemodel=large manipula.asm
wasmr -zq -zcm -Dmemodel=large pxdosasm.asm
tcc -O -c -ml memoria.c formato.c patmat.c
tcc -O -c -ml bios.c fat.c string.c caracter.c pxdospc.c
tcc -O -j1 -c -ml pxdosn.c 
tlink -x pxdosobj+pxdosn+bios+suporte+fat+pxdosasm+string+pxdospc+bnpxdos+manipula+memoria+formato+caracter+patmat,PXDOS.SIS

if exist PXDOS.SIS ( GOTO SUCESSO2 ) else ( GOTO ERRO2 )

:SUCESSO2

title Sucesso ao compilar o Kernel do PX-DOS(R) versao %VER%
echo Status do Kernel: [Compilado] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Kernel compilado com sucesso. >> pxdos.log
cls
echo.
echo.
echo A compilacao do Kernel do PX-DOS(R) foi bem sucedida.
echo.
echo.
GOTO FIM

:ERRO2
echo. >> pxdos.log
echo Status do Kernel: [Falha] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Falha ao compilar o Kernel. >> pxdos.log
cls
echo.
echo.
echo Um erro ocorreu durante a compilacao do Kernel.
GOTO FIM


:FIM
echo. >> pxdos.log
echo. >> pxdos.log
echo **************************************************** >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
pause