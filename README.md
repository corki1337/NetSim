# NetSim – Symulacja Linii Produkcyjnej (C++)

Projekt NetSim to system służący do modelowania i symulacji działania sieci przepływowych, oparty na przykładzie procesów zachodzących w fabryce. System pozwala na odwzorowanie drogi półproduktu od momentu dostawy do zakładu, przez kolejne etapy obróbki, aż po składowanie w magazynie końcowym.

## Model Sieci

Linia produkcyjna składa się z połączonych ze sobą węzłów (nodes), z których każdy pełni określoną funkcję:

1. **Rampy rozładunkowe (Loading Ramps)** – stanowią źródła sieci. To tutaj półprodukty trafiają do fabryki z określoną częstotliwością.
2. **Robotnicy (Workers)** – przetwarzają półprodukty. Każdy robotnik posiada określoną wydajność (czas pracy w turach) oraz własną kolejkę oczekujących produktów.
3. **Magazyny (Storehouses)** – stanowią ujścia sieci. Są to miejsca docelowe, w których gromadzone są gotowe produkty po zakończeniu procesu technologicznego.

### Dopuszczalne połączenia:
* Rampa rozładunkowa → Robotnik
* Robotnik → Robotnik (w tym przekazywanie produktu samemu sobie)
* Robotnik → Magazyn

## Logika Działania i Symulacji

### Półprodukty
* Każdy półprodukt posiada unikalny, dodatni numer seryjny, który nie zmienia się podczas procesu.
* Produkty są dostarczane do ramp w określonych odstępach czasu (np. co drugą turę).

### Przetwarzanie przez Robotników
* Robotnik przetwarza w danej turze tylko jedną jednostkę produktu.
* Obsługiwane są różne strategie kolejkowania:
    * **FIFO (First In, First Out)** – produkty są przetwarzane w kolejności napływania.
    * **LIFO (Last In, First Out)** – produkty są pobierane ze stosu (ostatni dostarczony jest przetwarzany jako pierwszy).
* Robotnik nie przerywa pracy nad aktualnym zadaniem, gdy napływają nowe dostawy.

### Przekazywanie produktów
* Każdy nadawca (rampa/robotnik) może mieć wielu zdefiniowanych odbiorców.
* Wybór odbiorcy następuje zgodnie z określoną strategią (domyślnie wybór losowy z jednakowym prawdopodobieństwem).

### Przebieg Tury Symulacji
Symulacja odbywa się w krokach czasowych zwanych turami. Każda tura składa się z etapów:
1. **Dostawa**: Półprodukty pojawiają się na rampach.
2. **Przekazanie**: Produkty gotowe do wysyłki trafiają natychmiast do kolejek odbiorców.
3. **Przetworzenie**: Robotnicy wykonują pracę nad produktami.
4. **Raportowanie**: Zbierane są informacje o aktualnym stanie sieci.

## Kluczowe Funkcjonalności

* **Weryfikacja spójności**: Przed rozpoczęciem symulacji system sprawdza, czy sieć jest poprawnie zbudowana – tzn. czy każdy półprodukt z dowolnej rampy ma szansę trafić do któregoś z magazynów.
* **Elastyczność**: Architektura pozwala na dodawanie nowych typów robotników, strategii kolejkowania oraz sposobów wyboru odbiorców.
* **Raportowanie**: System umożliwia generowanie raportów o strukturze sieci oraz szczegółowych raportów o stanie symulacji w konkretnych momentach czasu.
