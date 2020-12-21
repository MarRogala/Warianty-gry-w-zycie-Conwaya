# Opis

## Zmienne i zmienne tablicowe
- Nazwy zmiennych tworzone jak w języku C.
- Zmienne tablicowe to tylko: state[], newState[], color[].
- Indeksem zmiennej tablicowej może być tylko liczba naturalna.

## Wyrażenia
Dostępne operatory: ```+, -, * , /, %, <, >, <=, >=, ==, &&, ||```;

Wyrażeniem "prymitywnym" jest:
1. liczba (całkowita, lub zmiennoprzecinkowa z kropką)
2. zmienna
3. zmienna tablicowa
4. wywołanie funkcji

Wyrażenie binarne jest postaci:


wyrażenie prymitywne lub wyrażenie binarne w nawiasie --> operator --> wyrażenie prymitywne lub wyrażenie binarne w nawiasie

Wyrażenie to wyrażenie binarne lub wyrażenie prymitywne.

Przykłady wyrażeń:
    1. varName
    2. 100
    3. 100.52
    4. 1 + varName
    5. 1.0 + (state[1] + varName)
    6. (1 + 2) + (4 % 3)
    7. 1 + 2 * 4 --> ŹLE!!!

## Funkcje
- Wartość zwracana przez funkcję może być ignorowana.
- Funkcje przyjmują jako swoje argumenty tylko liczby.

Dostępne funkcje:

1. ```count(index, value)``` - zwraca liczbę sąsiadów, których stan spełnia ```state[index] == value```.
2. ```random(l, r)``` - zwraca losową liczbę rzeczywistą z przedziału [l, r].
3. ```stepsLimit(limit)``` - ustawia limit kroków do wykonania. -1 oznacza nieskończoność.
4. ```initialColor(r, g, b)``` - ustawia początkowy kolor pól
5. ```printEvery(value)``` - ustawia, co ile kroków wygląd planszy jest aktualizowany
6. ```skipState(index, value)``` - przerywa wykonywanie programu TRANSITION jeśli zachodzi ```state[index] == value```. Dla tego pola pominięte zostanie też wykonanie programu COLOR.

Wywołanie funkcji nie będące częścią wyrażenia kończy się średnikiem.

## Przypisania
Przypisanie możemy wykonywać do zmiennej lub jednej z trzech zmiennych tablicowych. Wartością przypisania może być dowolne wyrażenie.

Przykłady:
    - var = 1.0;
    - state[1] = 1 + (1 * 2);
    - color[0] = (var + state[1]) + 10.5;
    - var2 = random(0.0, 1.0);
    - var3 = count(0, 1.0) + 1;

- Zmienne nie muszą być deklarowane przed wykonaniem przypisania.
- Zmienne, do których nie została przypisana inna wartość, mają wartość 0.
- Wszystkie przypisania, widoczne są tylko w obrębie jednego programu (przypisanie wykonane w TRANSITION, nie jest widoczne w COLOR, ani w następnym TRANSITION).
- Przypisanie kończy się średnikiem.

### if-y
- Warunkiem może być dowolne wyrażenie.
- W bloku można wykonywać tylko przypisania.
- Blok musi być otoczony nawiasami ```{}```

Przykład:

    if(var == 1.0)
    {
        newState[0] = 100.5 + 10;
    }

## Definiujemy trzy programy
1. ```Init``` - wywołuje funkcje ustawiające podstawowe informacje o grze
2. ```Color``` - Korzystając ze stanu komórki generuje kolor w postaci RGB
3. ```Trans``` - Korzystając ze stanu komórki generuje nowy stan

### Init
Domyślne wartości:
- ```printEvery = 1```
- ```stepsLimit = -1```
- ```initialColor = (1.0, 1.0, 1.0)``` -> biały

W programie INIT wywołujemy funkcje ```stepsLimit, printEvery, initialColor```. Jeśli któraś z funkcji nie zostanie wywołana, gra rozpocznie się z domyślnymi wartościami.

### Color
Dostęp do aktualnego stanu przez zmienną ```state```. Wygenerowany kolor zapisujemy do zmiennej ```color```.

Na początku wykonywania programu COLOR, do zmiennej ```color```, przypisywany jest aktualny kolor pola.

### Trans
Dostęp do aktualnego stanu komórki - zmienna ```state```.
Budowanie nowego stanu w zmiennej ```newState```.

Na początku wykonywania programu TRANSITION, do zmiennej ```newState``` przypisywana jest wartość zmiennej ```state```.

# Opis przykładu
Komentarze dodane do obu programów nie są obsługiwane.

## Stan pola
    - state[0] - mówi czy pole jest żywe
    - state[1] - liczy, od ilu kroków pole powinno być żywe

## Program TRANSITION

    skipState(0, 1.0); // jeśli pole jest żywe, nie trzeba z nim już nic robić
    cond1 = count(0, 1.0) > 0; // ilu jest żywych sąsiadów
    if(cond1 && random(0.0, 1.0) > 0.5)
    {
        // wykonuje się z prawdopodobieństwem 1/2 jeśli pole ma żywego sąsiada
        newState[0] = 1;
    }
    if(cond1 && (cond == 0.0))
    {
        // jeśli pole ma żywego sąsiada, zwiększ licznik o 1.
        newState[1] = state[1] + 1;
    }

## Program COLOR

    if(state[0])
    {
        // jeśli pole jest żywe zmień kolor (na błękitny)
        color[0] = 0.1;
        color[1] = 0.8;
    }
    if((state[1] > 0) && (state[0] == 0))
    {
        //jeśli pole powinno być żywe, ale nie jest zmień kolor (na fioletowy)
        color[0] = 0.9;
        color[1] = 0.0;
    }
