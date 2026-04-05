# ASCII Image Converter

Konzolová aplikace, která převádí rastrový obrázek do ASCII grafiky - textové reprezentace obrázku sestávající ze znaků.

## Uživatelská dokumentace

`project/src/ASCIIImageConverter/` obsahuje skripty potřebné k sestavení a spuštění projektu pro Unix a Windows.

#### Build:
```
./build_windows.bat
```
nebo
```
./build_unix.sh
``` 

#### Run:

```
./run_windows.bat image.png
```

nebo

```
./run_unix.sh image.png
```

#### Parametry

První argument je vždy cesta ke vstupnímu obrázku.
Složka `project/src/data/` obsahuje několik testovacích obrázků. Pro jejich použití lze zadat předdefinovaný prefix `DATADIR`.
**Příklad:**
```
./run_windows.bat DATADIR/street.jpg
```

##### Podporované argumenty:
* `--width <hodnota>`: změní velikost obrázku. Šířka bude nastavena na zadaný počet pixelů a výška bude vypočítána automaticky.
**Příklad:**
```
./run_windows.bat image.png --width 700
```

* `--brightness <hodnota>`: změní jas obrazu. Kladná hodnota jas zvýší, zatímco záporná hodnota jej sníží.
**Příklad:**
```
./run_windows.bat image.png --brightness -0.7
```

* `--contrast <hodnota>`: změní kontrast obrazu. Hodnota mezi 0 a 1 kontrast sníží, zatímco hodnota větší než 1 jej zvýší.
**Příklad:**
```
./run_windows.bat image.png --contrast 1.2
```

* `--filter <název>`: použije na obrázek zadaný filtr.
**Podporované filtry**:
    + `invert`: invertuje barvy obrázku
    + `grayscale`: převádí barvy obrazu do odstínů šedi
    + `sepia`: aplikuje sépiový efekt
    + `blur`: aplikuje efekt rozmazání
    + `sharpen`: aplikuje efekt doostření
    + `edges`: vrátí černobílý obrázek s nalezenými hranami
Lze kombinovat více filtrů.
**Příklady:**
```
./run_windows.bat image.png --filter edges
./run_windows.bat image.png --filter invert --filter sepia
```

* `--set <název>`: definuje znakovou sadu pro výstupní reprezentaci obrázku
**Možnosti:**
    + `default`
    + `detailed`
    + `alpha`
    + `num`
    + `minimalist`
    + `alnum`
**Příklad:**
```
./run_windows.bat image.png --set alnum
```

* `--color`: pokud je zadán, zapne barevný výstup. Funguje pouze pro standardní výstup

* `--output <název>`: výstup bude zapsán do zadaného souboru
**Příklad:**
```
./run_windows.bat image.png --output out.txt
```

* `--interactive`: spustí interaktivní režim
Pokud byly spolu s tímto argumentem zadány další argumenty, změny se aplikují a poté se spustí interaktivní režim. 
V tomto případě bude počáteční obrázek zároveň ten, který je výsledkem ostatních zadaných argumentů.

#### Interaktivní režim
Tento režim umožňuje zadávat příkazy postupně. Změny zadané jednotlivými příkazy se použijí na aktuální verzi obrazu. 
Po každém zadaném příkazu se zobrazí aktuální výsledek.
**Příkazy:**
* `width <hodnota>`
* `brightness <hodnota>`
* `contrast <hodnota>`
* `filter <název>`
* `set <název>`
* `color <on/off>`
* `output <název>`: lze použít `STDCOUT` pro výstup zpět do konzole
* `undo`: vrátí stav obrazu o jednu operaci zpět
* `redo`: vrátí jednu změnu zrušenou příkazem undo
* `exit`: ukončit interaktivní režim

## Programátorská dokumentace

Program začíná ve funkci `main`, která provádí následující kroky:
1. Parsování vstupních argumentů 
2. Načtení obrázku
3. Úprava obrázku
4. Převod na ASCII
5. Volitelně spuštění interaktivního režimu

#### ArgsParser
Je zodpovědný za parsování argumentů příkazového řádku a příkazů interaktivního režimu a také validuje vstup.
Vrací objekt struktury `Arguments` nebo `InteractiveCommand` pro interaktivní režim.
Také používá seznamy podporovaných názvů filtrů a znakových sad pro ověření vstupu.

#### Image a RGB
`RGB` je struktura reprezentující jeden pixel. Obsahuje červenou, modrou a zelenou složku.
`Image` je třída reprezentující obrázek. Tato třída ukládá šířku a výšku obrázku v pixelech, počet kanálů a pole pixelů (`RGB`), které tvoří obrázek.

#### ImageLoader
Používá `stb_image` pro načítání obrázků. Funkce `loadFromFile` vrací objekt třídy `Image` nebo vyvolá výjimku, pokud zadaný obrázek nelze otevřít.

#### ImageProcessor 
Třída zodpovědná za zpracování obrazu. 

Funkce `processArgs` aplikuje všechny změny definované vstupní strukturou `Arguments`.
Funkce `processInteractive` aplikuje pouze jednu změnu určenou strukturou `InteractiveCommand`.
Funkce `resize` používá bilineární interpolaci ke změně velikosti obrázku. Nový pixel je interpolací 4 sousedních pixelů
Funkce `applyKernel` implementuje konvoluci jádra 3x3, t.j. každý pixel je vážený součet sousedních pixelů.

##### Edge detection
Funkce `detectEdges` implementuje algoritmus Canny edge detection.
Nejprve je na vstupní obraz aplikováno rozmazání pomocí funkce `blur`. 
Poté následuje výpočet gradientu pomocí funkce `computeGradient`. 
V tomto kroku je obraz nejprve implicitně převeden na odstíny šedi a následně jsou pomocí Sobelových operátorů vypočteny horizontální a vertikální změny intenzity. 
Výsledkem jsou dvě pole: velikost gradientu (`magnitude`), která určuje sílu hrany, a směr gradientu (`direction`), který popisuje orientaci hrany.
Na základě těchto hodnot je dále provedena non-maximum suppression pomocí funkce `suppress`.
Výsledná data jsou následně zpracována funkcí `hysteresis`, která provádí finální rozhodnutí o tom, které pixely tvoří hrany. 
Nejprve je uvnitř této funkce automaticky vypočten práh pomocí funkce `calculateThreshold`, která využívá histogram hodnot gradientu (`buildHistogram`) a Otsuovu metodu (`otsu`). 
Na základě tohoto prahu jsou určeny horní a dolní úrovně.
Pixely s hodnotou vyšší než horní práh jsou označeny jako silné hrany, zatímco pixely mezi dolním a horním prahem jsou označeny jako slabé hrany. 
Ostatní pixely jsou považovány za pozadí.
Následně je použito prohledávání do hloubky, které začíná od silných hran a postupně připojuje sousední slabé hrany. 
Slabé hrany jsou tedy zachovány pouze v případě, že jsou spojeny se silnou hranou.
Výsledkem funkce je binární obraz, kde silné a validované slabé hrany jsou reprezentovány bílou barvou, zatímco ostatní pixely jsou černé.

#### ASCIIConverter
Třída slouží k převodu obrazu do textové reprezentace a k výstupu výsledku na standardní výstup nebo do souboru.
Při převodu pomocí funkce `convert` obraz je rozdělen na menší bloky o pevné velikosti 4×8 pixelů. Každý takový blok odpovídá jednomu znaku ve výsledném textu. 
Pro každý blok se vypočítá průměrná hodnota jasu. Jas je určen pomocí váženého součtu RGB složek. 
Na základě průměrného jasu je následně zvolen odpovídající znak z vybrané znakové sady. Znakové sady jsou předdefinované a obsahují znaky seřazené od „nejtmavších“ po „nejsvětlejší“.
Pokud je aktivován barevný režim, jsou ke každému znaku přidány ANSI escape sekvence, které nastavují barvu textu podle průměrné barvy daného bloku. 
Tento režim funguje pouze při výstupu do terminálu a není používán při zápisu do souboru.
Třída také umožňuje vybrat výstupní stream pomocí funkce `getOutputStream`, která umožňuje zapisovat data buď na standardní výstup, nebo do souboru. 
Funkce `print` poté zobrazí skutečný výsledek.

#### InteractiveSession
Zajišťuje interaktivní režim programu. 
Atribut `original` obsahuje původní obraz, který slouží jako výchozí stav pro případné přepočítání. 
`current` reprezentuje aktuální stav obrazu po aplikaci všech dosavadních operací. 
Pro podporu funkcí `undo` a `redo` je použit vektor `history`, který ukládá posloupnost struktur `Arguments`, tedy jednotlivé konfigurace úprav. 
Proměnná `idx` určuje aktuální pozici v této historii a tím i aktuální stav obrazu.
Funkce `run` opakovaně načítá vstup od uživatele, dokud není zadán příkaz `exit`. 
Každý vstupní řádek je zpracován pomocí `ArgsParser`, který vytvoří objekt `InteractiveCommand` a aktualizuje kopii aktuálních argumentů. 
Následně je příkaz aplikován metodou `apply`. Pokud se nejedná o operaci `undo` nebo `redo`, uloží se nový stav do historie a aktualizuje se index `idx`. 
Po každé změně je aktuální obraz převeden na ASCII pomocí `ASCIIConverter` a vypsán na výstup.

Funkce `undo` a `redo` umožňují pohyb v historii změn. `undo` sníží index `idx`, pokud je to možné, a zavolá `rebuild`. `redo` naopak index zvýší, pokud existuje novější stav.
Funkce `rebuild` zajišťuje rekonstrukci aktuálního obrazu na základě historie. 
Vždy začíná od původního obrazu (`original`) a pomocí `ImageProcessor` znovu aplikuje všechny operace uložené v aktuálním stavu `history[idx]`.