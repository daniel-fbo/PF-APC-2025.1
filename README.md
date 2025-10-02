📌 Projeto RH_DIAC

Este projeto é um sistema em C com GTK que implementa uma interface gráfica para gerenciamento e cadastro.
Ele foi desenvolvido para fins de aprendizado e prática de programação estruturada com interface gráfica.

🚀 Tecnologias utilizadas

C (linguagem principal)

GTK+ 3 (interface gráfica)

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
(👉 você pode colocar prints da interface aqui depois)

📖 Makefile

O Makefile já possui as instruções necessárias:

make → compila o programa

make run → executa o programa

make clean → remove o executável gerado

👨‍💻 Autor

Seu Nome
GitHub
 | LinkedIn

👉 Esse README já está no padrão GitHub profissional.
Se quiser, posso melhorar ele adicionando badges automáticas (linguagem, versão do GTK, status do build) e até um tutorial passo a passo de uso do programa.

Quer que eu faça essa versão mais completa com badges e prints simulados?
