# Praca inżynierska II UWr

## Status
Działający generator diagramów Woronoja. Poprawione rysowanie, używając glew.

Działający parser podstawowego języka i pliku z początkowymi stanami.

## Wymagane biblioteki
1. glfw
2. glew
3. freeglut
4. kompilator clang w wersji 9.0 lub nowszej
5. pegtl

## Plik z danymi
Dane muszą być umieszczone w pliku w formacie ```.csv``` w katalogu ```/input```.
Pierwszy rząd to nie będące liczbami nazwy kolumn. Pierwsze dwie kolumny to odpowiednio współrzędne ```x, y``` punktów. Kolejne kolumny odpowiadają początkowym wartością stanu pól.

Liczby rzeczywiste umieszczone w pliku wejściowym, muszą być w formacie angielskim (część ułamkowa oddzielona kropką).

## Kompilacja
1. make clean
2. make
3. make run
   Uruchamia program, biorąc /input/input.csv jako plik z danymi.
4. make run FILE=exampleFileName.csv
   Uruchamia program, biorąc /input/exampleFileName.csv jako plik z danymi.
