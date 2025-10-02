📌 Projeto RH_DIAC

Este projeto é uma abstração de uma seção de recursos humanos em um site de uma empresa. É um sistema em C com GTK, acoplado com um Banco de Dados SQL para o gerenciamento e cadastro de candidatos a vagas na empresa.
Ele foi desenvolvido com fins de aprendizado e de prática de programação estruturada com interface gráfica aliada a um sistema de gerenciamento de banco de dados.

🚀 Tecnologias utilizadas

C (linguagem principal)

GTK+ 3 (interface gráfica)

CSS (auxílio a interface gráfica)

SQL (persistência de dados)

Makefile (automatização da compilação)


📂 Estrutura do projeto
gtk_workspace/

├── main.c               # Ponto de entrada do programa

├── interface.c / .h     # Implementa a interface gráfica (GTK)

├── database.c / .h      # Manipulação de dados simulando banco

├── validacao.c / .h     # Funções de validação

├── estilo.css           # Estilos visuais da interface GTK

├── Makefile             # Script de build

└── README.md            # Este arquivo

⚙️ Compilação e execução

🔹 Linux (Debian/Ubuntu)

Instale as dependências:

sudo apt-get update
sudo apt-get install build-essential pkg-config libgtk-3-dev


Compile e execute:

make
make run


Limpar arquivos gerados:

make clean

🔹 Windows (MSYS2)

Instale as dependências:

pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3


Compile e execute:

make
./rh_diac.exe

🖼️ Interface

O sistema utiliza GTK+ 3 para construir a interface gráfica.
Os estilos estão definidos no arquivo estilo.css.

📖 Makefile

O Makefile já possui as instruções necessárias:

make → compila o programa

make run → executa o programa

make clean → remove o executável gerado

👨‍💻 Autor

Daniel Filipe Borges de Oliveira

