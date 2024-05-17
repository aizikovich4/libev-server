## libev-server
На основе библиотеки libev реализовать простой неблокирующий  TCP-сервер под ОС на базе ядра Linux со следующим функционалом:
   Сервер:
    - Ожидание подключений клиентов на 5000 порт
    - Получаемые данные сервером, должны отправляться обратно клиенту
    - Каждое подключение должно логироваться записью (дата, время, полученные данные)
Организовать сборку через cmake
Тестового клиента можно реализовать на чем угодно хоть на Python

Server and client working on port 5000
##### checked on: 
ubuntu 18

g++ 9.4.0

python2

libev-dev

##### for build:
mkdir build

cd build

cmake ..

make

##### for run server:

./server

##### for run client:

python ./test_client.py 
