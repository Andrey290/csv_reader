# Инструкция по компиляции и запуску
Эта программа полностью соответствует техническому заданию. 
В текущей версии реализована 
- обработка .csv файла
- вывод в терминал
-  обработка деления на ноль
-  обработка отрицательных числел
-  обработка ввода некорректных адресов
-  рекурсивное вычисление

## Компиляция
Для компиляции используется Cmake.
Склонируйте репозиторий:
> $ git clone https://github.com/Andrey290/csv_reader.git

В корне проекта создайте каталог build/ и перейдите в него командой: 

> $ mkdir -p build/ 
> $ cd build/

Внутри него выполните:

> cmake ..
> make

В build/ появятся исполняемые файлы csvreader и csvreader_test.
## Запуск
Запускайте csvreader с любыми .csv файлами прямо из build/. Например так:
> $ ./csvreader ../test.csv

csvreader_test это исполняемый файл, запускающий заранее подготовленные Google Tests.
Установите gtests:
> $ sudo pacman -S gtest

Запустите файл:
> $ ./csvreader_tests

## Всё должно заработать! 
В данной версии могут возникать ошибки или быть реализованы неправильные подходы. Я только учусь! Я надеюсь в следущие несколько дней успеть добавить алгоритмы быстрой сортировки и бинарного поиска для поиска ячеек в действительно больших таблицах. =^0w0^=
