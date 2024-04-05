## Предварительные требования
- CMake 3.10+
## Установка
Склонируйте репозиторий
```
git clone https://github.com/DHawk34/lab2-tcp.git
```
Перейдите в папку ``server`` и забилдите сервер
```
bash run_build.sh
```
Перейдите в папку ``client`` и забилдите клиента
```
bash run_build.sh
```

## Запуск клиента
```
./client/build/Client
```

## Запуск сервера
```
./server/build/Server <port> <max_threads> <max_file_size> <save_path>
```

## Использование

### Сервер
Для запуска сервера необходимо указать порт, максимальное количество потоков, максимальный размер файла и путь для сохранения файла.

При запуске сервера он автоматически создаёт демона


### Клиент
После запуска запросит ip и порт сервера, после чего попытается к ним подключится.
- Если пользователь подключается, то он вводит путь к файлу, который нужно передать
- Если пользователь не смог подключиться, то он должен заново ввести ip и порт сервера