@echo off
del PX.SIS
set VER=0.9.0
title Compilando o Carregador de inicializacao do PX-DOS(R)
cls
echo.
echo.
echo Ferramenta de construcao do Carregador de Inicializacao
echo do PX-DOS(R).
echo.
echo.
echo Escolha o modo de compilacao desejada:
echo.
echo.
set/p opt="Use 1 para Debug e 2 para compilacao normal: "
echo Iniciando Log de construcao do Carregador de Inicializacao do PX-DOS(R)... >> pxdos.log
echo. >> pxdos.log
echo Sistema Operacional PX-DOS(R)>> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Copyright (C) 2013-2016 Felipe Miguel Nery Lunkes >> pxdos.log
echo Todos os direitos reservados >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Iniciando a construcao do Carregador de Inicializacao do PX-DOS(R) [%VER%] >> pxdos.log
echo. >> pxdos.log

GOTO COMP%OPT%

:COMP1
echo Usando diretiva: [Debug] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
cls
wasmr -zq -zcm -Dmemodel=tiny pxload.asm
wasmr -zq -zcm -Dmemodel=tiny int13x.asm
wasmr -zq -zcm -Dmemodel=tiny asm_util.asm
wasmr -zq -zcm -Dmemodel=tiny suporte.asm 
wasmr -zq -zcm -Dmemodel=tiny bnpxdos.asm
tcc -O -c -mt -DNECESSARIO_EXIBIR pxloader.c bios.c carregar.c string.c pxdospc.c
tcc -O -c -mt -DPXLOADER fat.c
tlink -t -x pxload+pxloader+int13x+asm_util+suporte+bios+pxdospc+fat+string+carregar+bnpxdos,PX.SIS,,,

if exist PX.SIS ( GOTO SUCESSO1 ) else ( GOTO FALHA1 )

:SUCESSO1
echo Sucesso ao construir o Carregador de Inicializacao do PX-DOS(R). >> pxdos.log
title Sucesso ao compilar o carregador de inicializacao do PX-DOS(R)
echo.
echo.
echo Sucesso ao compilar o carregador de inicializacao do PX-DOS(R)
echo.
echo.
GOTO FIM

:FALHA1
echo Falha ao construir o Carregador de Inicializacao do PX-DOS(R). >> pxdos.log
title Falha ao compilar...
echo.
echo.
echo Ocorreu uma falha ao compilar o C.I. do PX-DOS(R).
echo.
echo.
GOTO FIM

:COMP2
echo Usando diretiva: [Normal] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
cls
wasmr -zq -zcm -Dmemodel=tiny pxload.asm
wasmr -zq -zcm -Dmemodel=tiny int13x.asm
wasmr -zq -zcm -Dmemodel=tiny asm_util.asm
wasmr -zq -zcm -Dmemodel=tiny suporte.asm 
wasmr -zq -zcm -Dmemodel=tiny bnpxdos.asm
tcc -O -c -mt -DNECESSARIO_EXIBIR pxloader.c bios.c carregar.c string.c pxdospc.c
tcc -O -c -mt -DPXLOADER fat.c
tlink -t -x pxload+pxloader+int13x+asm_util+suporte+bios+pxdospc+fat+string+carregar+bnpxdos,PX.SIS,,,

if exist PX.SIS ( GOTO SUCESSO2 ) else ( GOTO FALHA2 )


:SUCESSO2
echo Sucesso ao construir o Carregador de Inicializacao do PX-DOS(R). >> pxdos.log
cls
title Sucesso ao compilar o carregador de inicializacao do PX-DOS(R)
cls
echo.
echo.
echo Sucesso ao compilar o carregador de inicializacao do PX-DOS(R)
echo.
echo.
GOTO FIM

:FALHA2
echo Falha ao construir o Carregador de Inicializacao do PX-DOS(R). >> pxdos.log
cls
title Falha ao compilar...
cls
echo.
echo.
echo Ocorreu uma falha ao compilar o Carregador de Inicializacao do PX-DOS(R)
echo.
echo.
GOTO FIM


REM


:FIM
echo.
echo. >> pxdos.log
echo. >> pxdos.log
echo **************************************************** >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
pause
