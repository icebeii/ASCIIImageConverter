# ASCII Image Converter

Projekt bude konzolová aplikace, která převádí rastrový obrázek (.jpg, .png) do ASCII grafiky - textové reprezentace obrázku sestávající ze znaků.
Program bude navržen tak, aby se spouštěl z příkazového řádku a umožňuje uživateli zadat různé parametry převodu, 
jako je šířka výstupu, jas, kontrast, použití filtrů, název znakové sady a v případě potřeby i barevný výstup.

### Příklady vstupů
`ascii_converter photo.jpg --width 120 --brightness 15 --filter sepia`

`ascii_converter cat.png --width 100 --contrast 1.4 --color`

`ascii_converter test.png --filter sharpen --set alphanumeric`

### Dostupné parametry
- `--width <hodnota>`: Nastaví šířku výsledného ASCII obrázku
- `--brightness <hodnota>`: Změna jasu (kladná = světlejší, záporná = tmavší)
- `--contrast <hodnota>`: Změna kontrastu
- `--filter <název>`: Aplikuje vizuální filtr (invert, sepia, blur, sharpen)
- `--color`: Aktivuje barevný výstup pomoci ANSI barev (pokud je výstup v konzoli)
- `--set <název>`: Zadaná sada znaků bude použita k převodu obrázku (numerical, alphanumerical, math symbols, ...)
- `--interactive`: Spustí interaktivní režim, kde lze měnit parametry během běhu programu
- `--output <název>`: Uloží výstup do zadaného textového souboru

### Algoritmus
1. Program načte vstupní soubor pomocí `stb_image.h` a uloží data pixelů do paměti.
2. Pokud je zadán parametr `--wisth`, proběhne downsampling nebo upsampling pomocí bilineární interpolace.
Uživatel určuje cílovou šířku, výška se dopočítá automaticky podle poměru stran.
3. Pokud jsou zadány parametry `--brightness`, `--contrast` nebo `--filter` algoritmus provede odpovídající změny v obrázku.
4. Převod obrázku do grayscale: `grayscale = 0.3*R + 0.59*G + 0.11*B` (Luminosity method)
5. Jas každého pixelu je namapován na symbol ze sady určené uživatelem pomocí parametru `--set`.
Pokud je zadán parametr `--color`, pak se barva každého znaku určí také na základě průměrné hodnoty pixelů v dané oblasti.
6. Výsledný ASCII obrázek se zobrazí v terminálu nebo v souboru zadaném v parametru `--output`.

Po spuštění s parametrem --interactive program vstoupí do režimu, kde lze zadávat příkazy během běhu, například:

`> brightness +10`

`> filter sepia`

`> color on`

Výsledek se okamžitě zaktualizuje a znovu vykreslí v konzoli.

### Technické detaily
- `stb_image.h` pro načítání obrázků
- ANSI escape sekvence pro barevné kódování výstupu
- CMake pro sestavení projektu a zajištění multiplatformní kompatibility