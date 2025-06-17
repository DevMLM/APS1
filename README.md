## Sistema de Gerenciamento de Eventos - Estrutura de Dados em C
Este projeto tem como objetivo simular um sistema completo de cadastro, organização e controle de eventos, utilizando conceitos clássicos de estruturas de dados na linguagem C.

## Funcionalidades
O sistema permite o cadastro e listagem de eventos com categorias e número de vagas, além de permitir que participantes sejam cadastrados (ou reaproveitados com base no e-mail). Ele gerencia inscrições nos eventos, controla o limite de vagas, adiciona participantes em uma fila de espera quando necessário e ainda mantém um histórico de eventos acessados. Conta com sistema de busca eficiente com índice ordenado e busca binária, persistência de dados em arquivos CSV e um modo administrador com senha.

## Estruturas de Dados Utilizadas
O sistema utiliza listas encadeadas para gerenciar eventos e inscrições, listas circulares para os participantes dentro de cada evento, uma tabela hash para busca rápida de participantes por e-mail, uma fila (FIFO) para fila de espera dos eventos, uma pilha (LIFO) para o histórico de interações, além de vetores de ponteiros ordenados para o índice de eventos com busca binária.

**Organização do Projeto**
O código está modularizado em pastas específicas:

src/Evento/ → funções relacionadas aos eventos

src/Participantes/ → cadastro e hash de participantes

src/Inscricoes/ → gerenciamento de inscrições e remoções

src/FilaPilha/ → fila de espera e pilha de histórico

src/Persistencia/ → salvar e carregar os dados em arquivos CSV

src/Utils/ → funções auxiliares (como entrada segura)

src/comum.h → structs, constantes e variáveis globais

## Persistência de Dados
O sistema salva automaticamente os dados ao encerrar e carrega tudo novamente ao iniciar. Isso é feito com arquivos .csv salvos na pasta data/. Os arquivos utilizados são: eventos.csv (ID, Nome, Categoria, Vagas), participantes.csv (ID, Nome, Email), inscricoes.csv (Evento ID, Participante ID, Nome, Email) e fila_espera.csv (Evento ID, Participante ID).

## Requisitos
Para rodar o sistema é necessário um compilador C (GCC, MinGW, etc). O console deve ser compatível com cores ANSI (ou as macros podem ser ajustadas em comum.h). Também é necessário criar a pasta data/ na raiz para salvar os arquivos .csv.

## Compilação (Exemplo com GCC)
gcc src/main.c src/Utils/utils.c src/Evento/eventos.c src/Participantes/participantes.c src/Inscricoes/inscricoes.c src/FilaPilha/filaPilha.c src/Persistencia/persist.c -I"src" -I"src/Utils" -I"src/Evento" -I"src/Participantes" -I"src/Inscricoes" -I"src/FilaPilha" -I"src/Persistencia" -o sistema_eventos

**Sugestão para Teste**

Cadastre um novo evento

Cadastre ou insira um participante

Faça a inscrição no evento

Preencha todas as vagas para ativar a fila de espera

Remova um participante e veja o próximo da fila ser promovido

Reinicie o programa e veja os dados sendo recuperados

## Acesso Administrativo
Algumas funções (como remoção de inscrição por evento) estão disponíveis apenas para administradores. A senha padrão é: admin123

Observações
Este projeto foi desenvolvido com foco didático e utiliza várias estruturas de dados importantes da disciplina. O código é modularizado, com uso intensivo de ponteiros, alocação dinâmica e leitura de arquivos. É um ótimo exemplo de aplicação prática dos conceitos estudados em Estrutura de Dados.
