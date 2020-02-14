### Readers-writers

Zadanie polega na realizacji problemu czytelników i pisarzy (opis wersji klasycznej w Wikipedii), przy czym:

    1. jest ustalona liczba procesów — N;
    2. każdy proces działa naprzemiennie w dwóch fazach: fazie relaksu i fazie korzystania z czytelni;
    3. w dowolnym momencie fazy relaksu proces może (choć nie musi) zmienić swoją rolę: z pisarza na czytelnika lub z czytelnika na pisarza;
    4. przechodząc do fazy korzystania z czytelni proces musi uzyskać dostęp we właściwym dla swojej aktualnej roli trybie;
    5. pisarz umieszcza efekt swojej pracy — swoje dzieło — w formie komunikatu w kolejce komunikatów, gdzie komunikat ten pozostaje do momentu, aż wszystkie procesy, które w momencie wydania dzieła były w roli czytelnika, nie odczytają go (po odczytaniu przez wszystkie wymagane procesy komunikat jest usuwany);
    6. pojemność kolejki komunikatów — reprezentującej półkę z książkami — jest ograniczona, tzn. nie może ona przechowywać więcej niż K dzieł;
    7. podczas pobytu w czytelni proces (również pisarz) czyta co najwyżej jedno dzieło, po czym czytelnik opuszcza czytelnię, a pisarz czeka na miejsce w kolejce, żeby opublikować kolejne dzieło.

### Harbour

Do portu zawijają statki, cumują w nim przez jakiś czas i go opuszczają. Każdy statek zarówno w celu wejścia do portu, jak i wyjścia z niego musi dostać pewną liczbę holowników, zależną od jego masy. Musi być też dla niego miejsce w doku. Zarówno liczba miejsc w doku, jak i liczba holowników są ustalone i ograniczone.

Cel zadania: synchronizacja statków (doki i holowniki to zasoby)