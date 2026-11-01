# ZPG – kontext pre ďalší chat a začiatok refactoringu

Stav overený **1. 11. 2026**. Autor projektu: **Martin Jarabica, jar0193**.

Tento dokument vznikol na používateľovu žiadosť pripraviť kontext pre ďalší chat. Aktuálnou úlohou bolo vytvoriť tento Markdown; samotný refactoring sa pri jeho príprave nezačal. Používateľ chce **v ďalšom chate začať rozumným, postupným refactoringom**, ktorý reálne zjednoduší kód. Nechce iba ďalší všeobecný plán ani zbytočne veľa tried.

Staré súbory `HANDOFF_REFACTORING.md` a `HANDOFF_TASK_6.md` opisujú starší stav. Najmä ich tvrdenia o chýbajúcom Observeri, nedokončenej úlohe 7 alebo nesamostatnom nočnom lese už neplatia. Vychádzaj z aktuálnych súborov, tohto dokumentu a `TASK_7.md`.

## 1. Projekt a pracovné prostredie

- Skutočný repozitár: `C:\Users\ASUS\source\repos\ZPG`.
- Aktívne zdroje: `C:\Users\ASUS\source\repos\ZPG\project`.
- Riešenie: `ZPG.slnx`; projekt: `project\project.vcxproj`.
- Cesta `C:\Users\ASUS\Documents\ChatGPT\ZPG` z prostredia chatu neexistuje. Pri príkazoch explicitne používaj skutočný repozitár.
- Windows, PowerShell, Visual Studio 2026 Insiders, **Debug/x64**, C++20.
- OpenGL 4.3 kontext; shadery GLSL `#version 410 core`.
- Aplikáciu spúšťaj s pracovným priečinkom `project`, pretože odtiaľ načítava relatívne cesty shaderov.
- Remote: `https://github.com/Skopcak/ZPG.git`.

Používaný shell:

```text
C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe
```

## 2. Čo používateľ chce

- Komunikovať po slovensky, zdrojový kód a komentáre písať po anglicky.
- Refactor robiť po malých, funkčných krokoch a vysvetliť, **čo sa zmenilo a prečo**.
- Po požiadavke začať refactor overiť stav a implementovať prvý primeraný krok; nezostať iba pri návrhu alebo opakovanom pýtaní na súhlas s už zadanou prácou.
- Zachovať existujúce scény, ich ovládanie, farby, transformácie a výsledné osvetlenie.
- Nepridávať novú funkcionalitu, klávesy alebo rozsiahly engine iba kvôli refactoringu.
- Nevytvárať automaticky ECS, kontajner závislostí, generické továrne, veľa managerov ani hierarchiu pre každú scénu. Trieda má mať konkrétnu zodpovednosť a musí sa skutočne používať.
- Výkonové optimalizácie robiť podľa konkrétneho problému alebo merania. Menej riadkov samo osebe neznamená rýchlejšie vykresľovanie.
- Commit a push robiť až na požiadavku; existujúce necommitnuté zmeny zachovať.

Používateľ pôvodne plánoval refactoring až po odovzdaní. Najnovšie chce tento dokument a následne začať refactoring v novom chate. Reálne nahratie do LMS/Kelvinu ani na Discord zatiaľ nepotvrdil; netreba z toho robiť ďalšiu schvaľovaciu podmienku začatia refactoringu.

## 3. Hotové cvičenie 7 a odovzdávacia verzia

Funkčne sú pripravené:

1. Tri režimy normál: bez transformácie, chybná modelová matica a správna inverzná transpozícia pri nerovnomernej mierke.
2. Test odlesku na odvrátenej strane s mocninou 1 a oprava podmienkou `dot(N, L) > 0`.
3. Chybné ukážky viewportu/projekcie a správny resize širokého aj vysokého okna.
4. Samostatný nočný les s jedným bodovým svetlom, materiálmi a útlmom vzdialenosti.
5. Depth test a LMS test homogénnej matice `diag(1, 1, 1, 5)`.
6. Observer medzi kamerou a shader programami.

Podklady:

- `Tasks\7.task\project` – odovzdávacia kópia zdrojov, shaderov a používaných modelov.
- `Tasks\7.task\jar0193.md` – stručný súhrn; väčší refactoring je v ňom ešte označený ako odložený.
- `Tasks\7.task\jar0193_*.png` – **12 obrázkov** správnych aj chybných ukážok.
- `Tasks\jar0193_7.zip` – overený balík, **65 súborov, približne 3.72 MiB**; bez knižníc a výstupov zostavenia.
- `docs\project-notes\TASK_7.md` – podrobná teória, ovládanie a postupy overenia.

**Refactoruj aktívny priečinok `project`. Odovzdávaciu kópiu a ZIP automaticky neprepisuj.** Predstavujú pripravenú verziu pred refactoringom. Podklady pre ďalšie odovzdanie možno vytvoriť samostatne po dohode.

Po zabalení sa v aktívnych shaderoch robili ďalšie úpravy zápisu/formátovania. Nepredpokladaj preto bajtovú zhodu aktívnych súborov s archívom; na začiatku nového chatu over aktuálne zdroje a ich kompiláciu. Pri príprave tohto handoffu sa všetkých 14 aktuálnych programov úspešne skompilovalo a prepojilo aj na GPU.

Ukážka planét už existuje z cvičenia 6 v `Tasks\6.task`; zverejnenie na Discorde nebolo potvrdené. V tomto refactoringu nejde o ďalšie odosielanie súborov do externých služieb.

## 4. Scény a nastavenia, ktoré zachovať

| Kláves | Obsah |
| --- | --- |
| 1 | Login; počiatočná scéna |
| 2 | Jedna guľa s Blinnovým osvetlením a podpis |
| 3 | Transformácie trojuholníka a podpis |
| 4 | Pôvodný denný les, zem, slnko, podpis a dvoje hodiny |
| 5 | Slnko, Zem, Mesiac, Venator, Imperial a podpis |
| 6 | Hodiny so skutočným miestnym časom |
| 7 | Štyri symetrické gule, všetky aktuálne používajú Lambert |
| 8 | Test normál na nerovnomerne škálovanej guli |
| 9 | Test odlesku na odvrátenej strane |
| 0 | Samostatný nočný les so svetluškou |

- WASD pohybuje kamerou, pravé tlačidlo myši otáča pohľad.
- Šípky posúvajú vybraný objekt; v nočnom lese je ním svetluška. Q/E otáča objekt a Z/X mení jeho mierku.
- Kamera je spoločná pre scény. Po prepnutí sa automaticky neresetuje.
- FOV je **90°**, počiatočné oko `(0, 0, 3)`, smer `(0, 0, -1)`, near/far `0.1/100`.
- `updateViewportOnResize = true`, `updateProjectionOnResize = true`.
- Pri nulovom framebuffere aplikácia preskočí kreslenie; kamera si ponechá poslednú platnú projekciu.
- `normalMode = 2`; alternatívy 0 a 1 zostávajú ako demonštračné režimy v `normal_test.vert`.
- `shininess = 1` a `clipBackfaceSpecular = 1` v testovacom programe.
- Svetlo testu štyroch gúľ je v počiatku. Svetlo testu presvitu je `(0, 0, -3)`.
- Správna guľa pri presvite je spredu tmavá, s ambientnou zložkou; nejde o chybu osvetlenia.

Používateľ výslovne odmietol automatickú zmenu scény 7 na štyri rôzne modely osvetlenia naraz. Všetky štyri gule majú naďalej používať ten istý program vybraný v kóde. Nepridávať F1–F4.

### Nočný les

- Je samostatný pod klávesom **0**, denný les zostáva pod **4**. Toto je posledná používateľom zvolená verzia.
- Zdieľané GPU modely, samostatné objekty a programy `nightTreeProgram`, `nightBushProgram`, `nightGroundProgram`.
- 74 stromov, 25 kríkov, zem a jedna svetluška. Bez denných hodín, slnka a podpisu.
- Vegetácia v malej čistinke pred počiatočnou kamerou sa posúva nabok, aby nezakrývala svetlušku. Denný les sa tým nemení.
- Svetluška začína v `(0, -0.3, 0)`, mierka `0.05`. Šípkami sa mení jej poloha aj poloha jediného bodového svetla.
- RGB intenzita svetla `(2.0, 1.8, 0.8)`; útlm `1 / (1 + 0.7*d + 1.8*d*d)`.
- Ambient `0.015 * surfaceColor` sa neutlmuje; difúzna aj zrkadlová zložka áno.
- Materiál zeme `(0.18, 0.12, 0.06)`, farby stromov/kríkov cez `useVertexColor = 1`.
- Aktuálne predvolené hodnoty nočného shaderu: lesk 16 a sila odlesku 0.15.

### Planéty a lode

- Zachovať pôvodné RGB modelov. Slnko používa ambientný program, Zem a Venator Phongov, Mesiac a Imperial Blinnov.
- Zem obieha Slnko a Mesiac Zem. Vlastná rotácia a mierka modelu Slnka sa neprenášajú na ich obežné dráhy.
- Lode obiehajú okolo osi Y v rovine XZ opačnými smermi, rýchlosť `0.35` rad/s.
- Polomery Venator/Imperial `1.05/1.40`, počiatočné fázy −45°/+45°.
- Mierka lodí `0.28`, náklon 65° okolo X. Imperial má navyše zámerné otočenie 180° okolo Z.
- Zachovať poradie transformácií: základ Slnka, rotácia obehu, orbitálny posun, prípadné otočenie Imperialu, náklon, mierka.
- Modely v hlavičkách neupravovať ani znovu neexportovať. Autor lodí Digital Sock a zdroj/licencia sú uvedené v komentároch.

## 5. Súčasná architektúra a hlavné riziká

`Application::run()` stále vytvára GPU zdroje a všetky scény, drží animácie, spracúva klávesnicu/myš, odosiela svetelné uniformy a vykresľuje. To je hlavný dôvod refactoringu. `Application.h` vlastní zatiaľ iba ukazovateľ na GLFW okno.

| Typ | Aktuálna zodpovednosť a vlastníctvo |
| --- | --- |
| `Model` | Vlastní VBO/VAO, nastavuje atribúty a kreslí geometriu; nekopírovateľný |
| `ShaderProgram` | Vlastní GPU program, kompilácia/linkovanie/uniformy a Observer kamery; nekopírovateľný |
| `DrawableObject` | Požičaný `Model&`, požičaný `ShaderProgram*`, vlastná základná `Transformation`, voliteľná požičaná dodatočná transformácia |
| `Scene` | Zoznam požičaných ukazovateľov na `DrawableObject`; objekty nevlastní |
| `CompositeTransformation` | Požičané ukazovatele na transformácie; násobí ich matice v poradí pridania |
| `Camera` | Poloha/smer, uložené view/projection, zoznam požičaných observerov |

### Životnosť a OpenGL kontext

1. Scénu nemožno jednoducho vrátiť z funkcie, ak bude odkazovať na modely, objekty, programy alebo transformácie lokálne v tejto funkcii. Po návrate by odkazy neplatili.
2. Vektory objektov používajú `reserve`, aby sa nezmenili adresy po vložení ukazovateľov do scény. Pri novom vlastníctve treba zabezpečiť stabilné adresy.
3. `Model` a `ShaderProgram` nemajú explicitný bezpečný move konštruktor. Nepredpokladať, že sa dajú ľubovoľne presúvať do rastúcich vektorov; primerané vlastníctvo cez `unique_ptr` môže byť jednoduchšie.
4. Teraz lokálne GPU objekty zaniknú pri návrate z `run()`, ešte pred `glfwDestroyWindow`. Ak sa presunú do členov `Application`, ich deštruktory sa spustia až po tele deštruktora aplikácie. Kontext treba zachovať až do uvoľnenia všetkých GPU zdrojov.
5. Aj GPU konštruktory musia bežať až po vytvorení aktuálneho kontextu a načítaní GLAD.
6. Zdieľané modely nevytvárať znova pre každý strom alebo každú scénu.

### Observer a uniformy

- Observer už je implementovaný a funguje. `ShaderProgram` dedí z `CameraObserver`, `Camera` nepozná konkrétny `ShaderProgram`.
- Registrácia deduplikuje pozorovateľa a ihneď odošle aktuálny stav; podporované je aj odpojenie.
- Kamera neoznamuje nulový pohyb ani nezmenené rozmery. V súčasnosti registruje 14 programov.
- `onCameraChanged` posiela view/projection všetkým a polohu kamery iba tam, kde príslušný uniform existuje.
- Pri zmene vlastníctva treba vyriešiť aj životnosť a odpojenie observerov; samotný vzor netreba prerábať nanovo.
- `ShaderProgram::setUniform()` pri neexistujúcom uniforme ukončí aplikáciu. `Light`/`Material` nesmú bez rozlíšenia posielať všetky uniformy všetkým programom.
- Napríklad nočný shader má `lightColor`, `attenuationFactors`, `shininess`, `specularStrength`, ale dnešný `phong.frag` a `blinn.frag` niektoré hodnoty stále uvádzajú ako konštanty. Pri spoločnom rozhraní zachovať pôvodné hodnoty, kým sa jednotlivé shadery vedome upravia.

### Rôzne formáty vrcholov

- Guľa, strom, krík, zem a login: XYZ + normála, 6 floatov. Základný farebný shader pri logine používa absolútne hodnoty normály ako RGB.
- Trojuholník: XYZ + RGB, 6 floatov; druhá trojica nie je normála.
- Planéty a lode: XYZ + RGB + normála, 9 floatov.
- Hodiny: XYZ + normála + RGB, tiež 9 floatov, ale iné poradie.
- `Model.cpp` dáva prvé tri hodnoty na location 0, druhé na 1, pri 9 hodnotách tretie na 2. Pri 6 hodnotách sa druhá trojica opakuje na location 2.
- `clock.vert` preto číta farbu na location 2; Phong číta normálu na location 2 a strom/krík na location 1.

Pri refactoringu sa nesmie zameniť farba s normálou. Samostatný opis vertex layoutu možno riešiť neskôr, keď preň bude konkrétna potreba.

### Homogénne súradnice a osvetlenie

V `phong.vert`, `normal_test.vert`, `tree.vert` a `bush.vert` je oprava:

```glsl
vec4 worldHomogeneous = modelMatrix * vec4(position, 1.0);
worldPosition = worldHomogeneous / worldHomogeneous.w;
gl_Position = projectionMatrix * viewMatrix * worldHomogeneous;
```

Zachovať delenie cez `w` pre osvetlenie aj pôvodné homogénne súradnice pre projekciu. Matica `diag(1,1,1,5)` bola overená porovnaním s mierkou 0.2; výsledné obrázky sa zhodovali bez pixelového rozdielu. Normály naďalej používajú inverznú transpozíciu a po interpolácii sa normalizujú vo fragment shaderi.

## 6. Odporúčaný postup refactoringu

Toto je návrh poradia, nie povinnosť bezmyšlienkovito prebrať každé pomenovanie. Po overení aktuálneho stavu zvoľ najmenší krok s reálnym prínosom.

### Prvý krok: `Light`

- Oddeliť údaje bodového svetla: poloha, RGB intenzita, konštantný/lineárny/kvadratický útlm.
- Triedu skutočne zapojiť do existujúcich scén a odosielania uniformov. Nevytvárať ju len ako nepoužívaný obal okolo `glm::vec3`.
- Zachovať väzbu svetla na Slnko v sústave a na svetlušku v noci, vrátane klávesnicového posúvania.
- Zjednotiť nastavenie dát a odstrániť súvisiace roztrúsené hodnoty. Rozdiely medzi shader programami zatiaľ riešiť explicitne; neposielať neexistujúce uniformy.
- Overiť scény 5, 7, 9 a 0 a zachovanie denného lesa. Ak je bezpečnejšie prvý krok obmedziť na nočný les, jasne uviesť rozsah a pokračovať následne.

### Druhý krok: `Material`

- Oddeliť farbu/použitie farieb vrcholov, lesk a silu odlesku; presný rozsah voliť podľa skutočných shaderov.
- Primeraným začiatkom sú tri nočné materiály, ktoré už používajú spoločný fragment shader. Neskôr rozšíriť na Phong/Blinn pri zachovaní ich doterajších hodnôt.
- Rozlíšiť materiál povrchu od svetla. Neprenášať do každého materiálu ďalšiu kópiu pohyblivej polohy svetla.
- Zvážiť vzťah `DrawableObject` → `Material` → `ShaderProgram` a jasné vlastníctvo; nepokaziť zdieľanie programov ani režimy `useVertexColor` v planetárnej scéne.

### Tretí krok: odľahčiť `Application.cpp`

- Najprv navrhnúť a zaviesť vlastníka zdrojov/stavu scén s dostatočnou životnosťou. Až potom presúvať ich tvorbu do funkcií alebo súborov.
- Vytiahnuť samostatné zodpovednosti: prípravu scén, spracovanie vstupu, animácie a vykreslenie. Robiť to po jednej časti.
- `Application` má postupne zostať pri inicializácii okna/kontextu a riadení hlavnej slučky.
- Samotný presun celého `run()` do rovnako veľkého `SceneManager` problém nevyrieši. Uprednostni konkrétne a zrozumiteľné rozdelenie.
- Do shaderov nesťahovať C++ zodpovednosti ani nepresúvať veľké polia modelov do ďalších miest. Zachovať matematické poradie transformácií a zdieľanie geometrií.

### Ďalšie upratovanie podľa potreby

- Zjednotiť štýl a názvy v upravovaných súboroch, odstrániť skutočne mŕtvy kód a drobné preklepy. Neriešiť všetky formátovacie zmeny naraz s logikou.
- Až podľa potreby zvážiť cache uniform lokácií alebo obmedzenie opakovaných nastavení. Zachovať diagnostiku skutočne chýbajúcich povinných uniformov.
- Pre každý krok uviesť zmenené súbory, prínos, výsledok overenia a prípadné zostávajúce obmedzenia.

## 7. Overovanie

Posledné kontroly pred odovzdávacím balíkom prešli:

- Debug/x64 build aktívneho projektu.
- Kompilácia/linkovanie všetkých 14 aktuálnych shader programov a synchronizácia projekcie.
- Tri režimy normál: rovnaká geometria, rozdielne osvetlenie.
- Presvit pri mocnine 1 a jeho orezanie.
- Chybná projekcia, chybný viewport, správne široké aj vysoké okno.
- Depth test, platná kamera po nulových rozmeroch a obnove veľkosti.
- Denný/nočný les, planéty, viditeľná svetluška, útlm a pohyb svetla.
- Homogénna matica versus mierka 0.2.
- Obsah ZIP-u a zhoda všetkých 52 odovzdávaných zdrojových/shaderových/modelových súborov s overeným projektom.

Zostavenie s výstupmi mimo repozitára, z koreňa skutočného repozitára:

```powershell
& 'C:\Program Files\Microsoft Visual Studio\18\Insiders\MSBuild\Current\Bin\MSBuild.exe' 'project\project.vcxproj' /t:Build /p:Configuration=Debug /p:Platform=x64 '/p:OutDir=C:\Users\ASUS\.codex\build-reviews\zpg-observer-20261029\bin\' '/p:IntDir=C:\Users\ASUS\.codex\build-reviews\zpg-observer-20261029\obj\' /m /nologo /v:minimal
```

V niektorých zostaveniach sa objavilo existujúce upozornenie linkera `LNK4098`; build prešiel. Nezamieňať ho s chybou shaderu ani automaticky nerozširovať refactor na úpravy externých knižníc.

Existujúce pomocné kontroly sú mimo repozitára:

- Python: `C:\Users\ASUS\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe`.
- `C:\Users\ASUS\.codex\build-reviews\zpg-planet-colors-20261023\check_shaders.py` – pôvodná kontrola deviatich programov a RGB planét; pri poslednom overení sa zoznam v pamäti rozšíril podľa aktuálnych shaderov. Samotný pôvodný skript nepokrýva všetkých 14.
- `C:\Users\ASUS\.codex\build-reviews\zpg-camera-20261030\check_camera.vcxproj` – kontrola Observera, pohybu, resize, odpojenia a nulových rozmerov. Pred použitím po refactoringu ho znova zostav a spusti s cwd `project`.
- `C:\Users\ASUS\.codex\build-reviews\zpg-submit-20261101\capture_exercise7.cpp` a `capture_exercise7.vcxproj` – zachytenie porovnávacích renderov a overenie projekcie/depth testu.
- `C:\Users\ASUS\.codex\build-reviews\zpg-submit-20261101\Application_capture.cpp` – **testovacia kópia**, nie aktívny zdroj aplikácie. Má pevné semeno lesa, voliteľné chybné prepínače a doplnkový homogénny test.

Pozor: starý capture projekt kompiluje uvedenú kópiu aplikácie. Po refactoringu ju treba zodpovedajúco obnoviť alebo upraviť kontrolu na nové rozhrania; spustenie starej binárky neoveruje nové zdroje. Historické PNG sú referencie, nie dôkaz nového testu. Počet programov 14 opisuje dnešný stav, nie zákaz neskoršieho rozumného zdieľania programov.

V repozitári nie je nakonfigurovaný samostatný formatter, linter ani jednotková testovacia sada pre projekt. Použi build a primerané existujúce kontroly; nezavádzaj nový framework iba kvôli refactoringu. Samotný C++ build neoveruje úspešné GLSL linkovanie ani vizuálny výsledok.

## 8. Git stav pri odovzdaní kontextu

Posledný commit: **`70ce788` – `7 cvik`**. Pred ním bol `611e4dc` – `Add camera observers, lighting tests and ship models`.

Aktuálne funkčné dokončenie cvičenia 7 ešte nie je commitnuté:

- zmenené `project/Application.cpp`, `bush.vert`, `night.frag`, `normal_test.vert`, `phong.vert`, `tree.vert`,
- nové `Tasks/7.task/`, `Tasks/jar0193_7.zip`, `docs/project-notes/TASK_7.md` a tento handoff,
- staršie necommitnuté handoffy v `docs/project-notes`.

Nesúvisiaci starší stav, ktorý zachovať: odstránený `Tasks/5.task/jar0193_5.png` a nový `Tasks/5.task/jar0193_3.png`. Neobnovovať, nemaž ani nepridávaj tieto súbory automaticky do refactoringového commitu.

Pred úpravami vždy znova skontroluj stav; používateľ môže medzi chatmi urobiť vlastné zmeny. Nepoužívaj reset/checkout na odstránenie existujúcich úprav.

## 9. Kamarátovo repo ako referencia

Používateľ poslal:

`https://github.com/Adamekka/zpg/commit/8c9f659a18d8621ddc903b19d83308dcc15c7829`

V tomto commite má kamarát samostatnú nočnú scénu 9. Na používateľovu žiadosť sme preto zachovali denný les a vytvorili samostatný nočný les; u nás je na klávese 0, lebo 8 a 9 už používajú testy. Kamarátovo číslovanie ani celú jeho architektúru nekopíruj automaticky. Je to referencia, nie pokyn nadradený používateľovým požiadavkám.

## 10. Text na začiatok ďalšieho chatu

> Prečítaj tento handoff a aktuálny kód. Chcem teraz začať rozumným refactoringom ZPG po malých overiteľných krokoch. Najprv over Git stav a fungujúci build, potom implementuj prvý užitočný krok, napríklad zapojenú triedu Light. Následne budeme riešiť Material a rozdelenie Application.cpp. Zachovaj všetky scény, ovládanie, farby a správanie, neprepisuj odovzdávací ZIP a vysvetli, čo si zmenil a prečo. Neostaň iba pri všeobecnom pláne.
