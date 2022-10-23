<div align="center">

<h1>PX-DOS Operating System</h1>
<h1>Sistema Operacional PX-DOS</h3>

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

# Repositories

<div align="center">
  
[![PX-DOS Kernel](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS&theme=dark)](https://github.com/felipenlunkes/PX-DOS)
[![PX-DOS Drivers](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-Drivers&theme=dark)](https://github.com/felipenlunkes/PX-DOS-Drivers)
[![PX-DOS Init](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-init&theme=dark)](https://github.com/felipenlunkes/PX-DOS-init)
[![PX-DOS libc](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-libc&theme=dark)](https://github.com/felipenlunkes/PX-DOS-libc)
[![PX-DOS Apps](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-Apps&theme=dark)](https://github.com/felipenlunkes/PX-DOS-Apps)
[![PX-DOS libasm](https://github-readme-stats.vercel.app/api/pin/?username=felipenlunkes&repo=PX-DOS-libasm&theme=dark)](https://github.com/felipenlunkes/PX-DOS-libasm)

</div>

# English

<div align="justify">

[PX-DOS](https://github.com/felipenlunkes/PX-DOS) is derived from an older version of a DOS system, the [Public Domain Operating System](http://www.pdos.org/) (PDOS). PX-DOS adds new layers, abstractions and functions on top of PDOS and extends its functionality. Also added a number of new utilities (userland) to the base system.

System components have been divided into repositories. [Here](https://github.com/felipenlunkes/PX-DOS) we have the repository that contains the kernel, boot loader and command interpreter, [here](https://github.com/felipenlunkes/PX-DOS-init) what contains the system init (user mode) and [here](https://github.com/felipenlunkes/PX-DOS-Apps) the system utilities. We also have the [repository](https://github.com/felipenlunkes/PX-DOS-libasm) with the Assembly development libraries and [libc](https://github.com/felipenlunkes/PX-DOS-libc).

</div>

### Dependencies needed to build PX-DOS

<div align="justify">

To build PX-DOS, you will need the following dependencies:

</div>

<div align="center">

![Windows](https://img.shields.io/badge/Windows_32_bit-0078D6?style=for-the-badge&logo=windows&logoColor=white)
![TurboC](https://img.shields.io/badge/Borland_Turbo_C-F57842?style=for-the-badge&logo=c&logoColor=white)
![OpenWatcom](https://img.shields.io/badge/Open_Watcom-4EAA25?style=for-the-badge&logo=c&logoColor=white)
![TASM](https://img.shields.io/badge/TASM-0C322C?style=for-the-badge&logo=assembly&logoColor=white)

</div>

<div align="justify">

You cannot start the process without these dependencies installed. The dependencies listed above can only run on 32-bit versions of Windows NT (or on virtual machines with some 32-bit version of Windows installed). Make sure the path to the dependencies' executables is declared in %PATH%.

After all dependencies are installed, you can proceed with this tutorial.

</div>

### Bootloader

<div align="justify">

The boot loader can be built by running the file [pxloader.bat](pxloader.bat), present in this directory.

</div>

### Kernel

<div align="justify">

The kernel must be built by running the [kernel.bat](kernel.bat) file.

</div>

### Command interpreter

<div align="justify">

Before building the shell, you must build the PX-DOS libc. To do this, clone [repository](https://github.com/felipenlunkes/PX-DOS-libc) to your computer and follow the instructions there. Then copy the generated library to the following path: `C:\Dev\DOS\pxdosc.lib`. The shell builder script will fetch the libc at this location.

</div>

### Execution and Tests

<div align="justify">

Mount the PX-DOS floppy image and replace the generated files.

> The bootloader installer, adapted from FreeDOS, will be available soon, so you can do a floppy or hard disk installation from scratch.

Afterwards, run the system with a virtual machine manager, such as VirtualBox or qemu.

</div>

# Português

### O PX-DOS

<div align="justify">

[PX-DOS](https://github.com/felipenlunkes/PX-DOS) é derivado de uma versão mais antiga de um sistema DOS, o [Public Domain Operating System](http://www.pdos.org/) (PDOS). O PX-DOS adiciona novas camadas, abstrações e funções sobre o PDOS e estende sua funcionalidade. Também foram adicionados vários novos utilitários (userland) ao sistema básico.

Os componentes do sistema foram divididos em repositórios. Aqui temos o repositório que contém o kernel, carregador de inicialização e interpretador de comandos, [aqui](https://github.com/felipenlunkes/PX-DOS-init) o ​​que contém o init do sistema (modo de usuário) e [aqui](https://github.com/felipenlunkes/PX-DOS-Apps) os utilitários do sistema. Também temos o [repositório](https://github.com/felipenlunkes/PX-DOS-libasm) com as bibliotecas de desenvolvimento Assembly e [libc](https://github.com/felipenlunkes/PX-DOS-libc).

</div>
  
### Dependências necessárias à construção do PX-DOS

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

### Carregador de inicialização

<div align="justify">

O carregador de inicialização pode ser construído executando o arquivo [pxloader.bat](pxloader.bat), presente neste diretório.

</div>

### Kernel

<div align="justify">

O kernel deve ser construído executando o arquivo [kernel.bat](kernel.bat).

</div>

### Interpretador de comandos

<div align="justify">

Antes de construir o interpretador de comandos, você deve construir a libc do PX-DOS. Para isso, clone o [repositório](https://github.com/felipenlunkes/PX-DOS-libc) para seu computador e siga as instruções nele presentes. Depois, copie a biblitoeca gerada para o seguinte caminho: `C:\Dev\DOS\pxdosc.lib`. O script de construção do insterpretador de comandos irá buscar a libc nesta localização.

</div>

### Execução e testes

<div align="justify">

Monte a imagem de disquete do PX-DOS e substitua os arquivos gerados.

> O instalador do carregador de inicialização, apaptado do FreeDOS, será disponibilizado em breve, para que seja possível fazer uma instalação em disquete ou disco rígido do zero.

Após, execute o sistema junto a uma gerenciador de máquinas virtuais, como VirtualBox ou qemu.

</div>

<!-- Versão do arquivo: 1.0

Copyright © 2012-2022 Felipe Miguel Nery Lunkes

-->