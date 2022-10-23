<div align="center">

![](https://img.shields.io/github/license/felipenlunkes/PX-DOS.svg)
![](https://img.shields.io/github/stars/felipenlunkes/PX-DOS.svg)
![](https://img.shields.io/github/issues/felipenlunkes/PX-DOS.svg)
![](https://img.shields.io/github/issues-closed/felipenlunkes/PX-DOS.svg)
![](https://img.shields.io/github/issues-pr/felipenlunkes/PX-DOS.svg)
![](https://img.shields.io/github/issues-pr-closed/felipenlunkes/PX-DOS.svg)
![](https://img.shields.io/github/downloads/felipenlunkes/PX-DOS/total.svg)
![](https://img.shields.io/github/release/felipenlunkes/PX-DOS.svg)

</div>

<hr>

# O PX-DOS

<div align="justify">

[PX-DOS](https://github.com/felipenlunkes/PX-DOS) é derivado de uma versão mais antiga de um sistema DOS, o [Public Domain Operating System](http://www.pdos.org/) (PDOS). O PX-DOS adiciona novas camadas, abstrações e funções sobre o PDOS e estende sua funcionalidade. Também foram adicionados vários novos utilitários (userland) ao sistema básico.

Os componentes do sistema foram divididos em repositórios. Aqui temos o repositório que contém o kernel, carregador de inicialização e interpretador de comandos, [aqui](https://github.com/felipenlunkes/PX-DOS-init) o ​​que contém o init do sistema (modo de usuário) e [aqui](https://github.com/felipenlunkes/PX-DOS-Apps) os utilitários do sistema. Também temos o [repositório](https://github.com/felipenlunkes/PX-DOS-libasm) com as bibliotecas de desenvolvimento Assembly e [libc](https://github.com/felipenlunkes/PX-DOS-libc).

</div>

<div align="center">
  
[![PX-DOS Kernel](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS&theme=dark)](https://github.com/felipenlunkes/PX-DOS)
[![PX-DOS Drivers](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-Drivers&theme=dark)](https://github.com/felipenlunkes/PX-DOS-Drivers)
[![PX-DOS Init](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-init&theme=dark)](https://github.com/felipenlunkes/PX-DOS-init)
[![PX-DOS libc](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-libc&theme=dark)](https://github.com/felipenlunkes/PX-DOS-libc)
[![PX-DOS Apps](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-Apps&theme=dark)](https://github.com/felipenlunkes/PX-DOS-Apps)
[![PX-DOS libasm](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-libasm&theme=dark)](https://github.com/felipenlunkes/PX-DOS-libasm)

</div>
  
# Dependências necessárias à construção do PX-DOS

<div align="justify">

Para construir o PX-DOS, você vai precisar das seguintes dependências:

</div>

<div align="center">

![Windows](https://img.shields.io/badge/Windows_32_bit-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![TurboC](https://img.shields.io/badge/Borland_Turbo_C-F57842?style=for-the-badge&logo=c&logoColor=white)
![OpenWatcom](https://img.shields.io/badge/Open_Watcom-4EAA25?style=for-the-badge&logo=c&logoColor=white)
![TASM](https://img.shields.io/badge/TASM-0C322C?style=for-the-badge&logo=assembly&logoColor=white)

</div>

<div align="justify">

Você não pode iniciar o processo sem essas dependências instaladas. As dependências listadas acima só podem ser executadas sobre versões do Windows NT de 32-bit (ou em máquinas virtuais com alguma versão do Windows de 32-bit instalada). Tenha certexa que o caminho para os executáveis das dependências estejam declarados em %PATH%.

Após a instalação de todas as dependências, você poderá seguir com este tutorial.

</div>

## Carregador de inicialização

<div align="justify">

O carregador de inicialização pode ser construído executando o arquivo [pxloader.bat](pxloader.bat), presente neste diretório.

</div>

## Kernel

<div align="justify">

O kernel deve ser construído executando o arquivo [kernel.bat](kernel.bat).

</div>

## Interpretador de comandos

<div align="justify">

Antes de construir o interpretador de comandos, você deve construir a libc do PX-DOS. Para isso, clone o [repositório](https://github.com/felipenlunkes/PX-DOS-libc) para seu computador e siga as instruções nele presentes. Depois, copie a biblitoeca gerada para o seguinte caminho: `C:\Dev\DOS\pxdosc.lib`. O script de construção do insterpretador de comandos irá buscar a libc nesta localização.

</div>

## Execução e testes

<div align="justify">

Monte a imagem de disquete do PX-DOS e substitua os arquivos gerados. O instalador do carregador de inicialização, apaptado do FreeDOS, será disponibilizado em breve, para que seja possível fazer uma instalação em disquete ou disco rígido do zero.

</div>

<!-- Versão do arquivo: 1.0

Copyright © 2012-2022 Felipe Miguel Nery Lunkes

-->