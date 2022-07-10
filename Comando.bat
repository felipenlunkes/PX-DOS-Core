@echo off
del comando.epx

title Compilador do Interpretador de Comandos do PX-DOS(R)
cls
echo.
echo.
echo Compilador do Interpretador de Comandos do PX-DOS(R)
echo.
echo.
echo Insira a opcao de compilacao desejada:
echo.
echo.
set/p opt="Use 1 para modo Debug e 2 para modo normal: "
echo Iniciando construcao do Interpretador de Comandos do PX-DOS(R)... >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Sistema Operacional PX-DOS(R) >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Copyright (C) 2013-2016 Felipe Miguel Nery Lunkes >> pxdos.log
echo Todos os direitos reservados. >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
GOTO COMP%opt%

:COMP1
echo Usando diretiva do usuario: [Debug] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Iniciando... >> pxdos.log
cls
wasmr -zq -zcm -Dmemodel=large suporte.asm
tcc -O -c -ml comando.c PX_DOS.c tempo.c
tlink -x comando+PX_DOS+tempo+suporte,comando.epx,,C:\Dev\DOS\pxdosc.lib

if exist comando.epx ( GOTO SUCESSO1 ) else ( GOTO ERRO1 )

:SUCESSO1
echo Sucesso ao construir o Interpretador de Comandos. >> pxdos.log

echo.
echo.
echo A compilacao do Interpretador de comandos do PX-DOS(R) foi bem sucedida.
echo.
echo.
GOTO FIM

:ERRO1
echo Erro ao construir o Interpretador de Comandos. >> pxdos.log
echo.
echo Um erro ocorreu durante a compilacao do Interpretador de comandos.
GOTO FIM

:COMP2
echo Usando diretiva do usuario: [Normal] >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
echo Iniciando... >> pxdos.log
cls
wasmr -zq -zcm -Dmemodel=large suporte.asm
tcc -O -c -ml comando.c PX_DOS.c tempo.c
tlink -x comando+PX_DOS+tempo+suporte,comando.epx,,C:\Dev\DOS\pxdosc.lib

if exist comando.epx ( GOTO SUCESSO2 ) else ( GOTO ERRO2 )

:SUCESSO2
echo Sucesso ao construir o Interpretador de Comandos. >> pxdos.log
cls
cls
echo.
echo.
echo A compilacao do Interpretador de comandos do PX-DOS(R) foi bem sucedida.
echo.
echo.
GOTO FIM

:ERRO2
echo Erro ao construir o Interpretador de Comandos. >> pxdos.log
cls
echo.
echo Um erro ocorreu durante a compilacao do Interpretador de comandos.
GOTO FIM


:FIM
echo. >> pxdos.log
echo. >> pxdos.log
echo **************************************************** >> pxdos.log
echo. >> pxdos.log
echo. >> pxdos.log
pause
