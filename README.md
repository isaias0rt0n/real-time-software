## Trabalho da disciplina de Software em Tempo Real

## ⚙Config
- clone repositório
```shell
git clone https://github.com/isaias0rt0n/real-time-software.git
```
- Entrar na pasta
```shell
cd real-time-software
```
- Compilar os programas
```shell
gcc -o main main.c socket/socket.c monitores/sensores.c monitores/tela.c -lpthread
```
- Executar o 'Aquecedor2021.jar' e iniciar simulação
```shell
java -jar Aquecedor2021.jar 4545
```
- Executar o binário (atenção para o localhost)
```shell
./main localhost 4545
```
