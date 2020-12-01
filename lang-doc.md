# Opis

## Definiujemy trzy funkcje
1. ```Init``` - definiuje startowy stan komórek.
2. ```Color``` - Korzystając ze stanu komórki generuje kolor w postaci RGB
3. ```Trans``` - Korzystając ze stanu komórki generuje nowy stan

### Init
Wczytywanie danych z pliku lub proste generowanie losowych stanów. Wszystko opakowane w dwie lub trzy funkcje np. ```start = (ReadFile file.txt)```.

### Color
Dostęp do aktualnego stanu przez zmienną ```state```. Wygenerowany kolor zapisujemy do zmiennej ```color```.

### Trans
Dostęp do aktualnego stanu komórki - zmienna ```state```.
Budowanie nowego stanu w zmiennej ```newState```.

## Operatory i wyrażenia

Wyrażenia w postaci (op arg1, ...). Wyrażenia można przypisywać do pomocniczych zmiennych.

### Operatory
1. Operacje arytmetyczne
2. Operacje logiczne
3. Count
   Przyjmuje numer pola oraz indeks. Zwraca liczbę sąsiadów danego pola, którzy mają ```state[index] > 0```.
4. Neighbours
    Zwraca liczbę sąsiadów pola.

### if-y
Sprawdzają, czy dane wyrażenie ma wartość większą od zera.
Jako blok instrukcji mogą wykonać tylko jedno przypisanie do zmiennej lub pod indeks stanu.
