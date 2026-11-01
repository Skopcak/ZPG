# ZPG – odovzdanie kontextu pre nový chat a refaktoring

Stav skontrolovaný **29. 10. 2026**. Autor projektu: **Martin Jarabica, jar0193**.
Tento súhrn vznikol s pomocou AI podľa rozhovoru, aktuálnych súborov a Git stavu.

Dokument zachytáva kontext; sám osebe nie je požiadavkou začať refaktoring.
Starší `HANDOFF_TASK_6.md` opisuje stav z 24. 10. 2026. Informácie, že ešte chýbajú lode, screenshoty alebo odovzdávací súhrn, už nie sú aktuálne.

## 1. Kde pokračovať

- Skutočný repozitár: `C:\Users\ASUS\source\repos\ZPG`.
- Aktívne zdroje aplikácie: `C:\Users\ASUS\source\repos\ZPG\project`.
- Projekt Visual Studia: `project\project.vcxproj`; riešenie: `ZPG.slnx`.
- Odovzdávacia kópia úlohy 6: `Tasks\6.task`.
- Staršie študijné poznámky: `docs\project-notes\TASK_1.md` až `TASK_5.md` a `BACKLOG.md`.
- Podrobné študijné poznámky `TASK_6.md` zatiaľ neexistujú. Nezamieňať ich s krátkym odovzdávacím súhrnom.

Cesta `C:\Users\ASUS\Documents\ChatGPT\ZPG` z kontextu chatu nebola platným pracovným priečinkom repozitára. Pri príkazoch používať skutočný repozitár uvedený vyššie.

Prostredie: Windows, PowerShell, Visual Studio 2026 Insiders, **Debug / x64**, C++20. Aplikácia vytvára OpenGL 4.3 kontext a používa GLSL `#version 410 core`.
Relatívne cesty shaderov sa vyhodnocujú voči pracovnému priečinku procesu; pri spúšťaní používať priečinok `project`.

## 2. Ako chce používateľ spolupracovať

- Komunikovať po slovensky, zmeny vysvetľovať jednoducho a postupne.
- Používateľ najprv chcel zmeny písať sám, neskôr výslovne povolil priame úpravy a požiadal vysvetliť, čo sa zmenilo a prečo.
- Pri refaktoringu zachovať malé, overiteľné kroky. Nepridávať zbytočné triedy, knižnice, ovládanie ani rozsiahle architektonické zmeny.
- Zdrojový kód a jeho komentáre písať po anglicky. Učiteľove modely v hlavičkách neupravovať.
- Zachovať pôvodné farby Slnka, Zeme, Mesiaca aj materiálové farby lodí.
- Používateľ odmietol prerobenie scény 7 na štyri rôzne osvetlenia naraz podľa spolužiaka. Všetky štyri gule majú používať rovnaký program vybraný v kóde. Nepridávať automaticky F1–F4.
- Zachovať čelný pohľad na gule v rovine XY a existujúce ovládanie kamery.
- Commit a push robiť na následnú požiadavku. V tomto pokračovaní zatiaľ nebol vytvorený nový commit.

## 3. Čo sme dokončili v úlohe 6

Už pred týmto pokračovaním boli implementované štyri osvetľovacie modely, testovacia scéna štyroch gúľ, normálová matica a osvetlenie farebných planét.

V tomto chate pribudlo:

1. **Ambientná zložka Lamberta** v `project\lambert.frag`: `vec3 ambient = vec3(0.1)` a výsledok `ambient + diffuse`, podľa prednášky. Odvrátené časti modelu tak majú slabý základný jas.
2. **Exportný skript** `tools\export_model_to_header.py`, ktorý sa spúšťa priamo v Blenderi.
3. **Venator a Imperial Star Destroyer**, ktoré používateľ stiahol, otvoril v Blenderi a vyexportoval do `.h`.
4. Zapojenie lodí do `Application::run()` a ich registrácia v `project.vcxproj` aj `project.vcxproj.filters`.
5. Obe lode obiehajú Slnko v opačných smeroch okolo osi Y, teda v rovine XZ. Z pohľadu počiatočnej kamery sa pohybujú doľava/doprava a zároveň približujú/vzďaľujú.
6. Krátky odovzdávací súhrn `Tasks\6.task\jar0193.md`: štyri body, najviac dve jednoduché vety na bod.

Používateľ následne potvrdil, že otestoval všetky shadery a FOV, natočil video planét a vytvoril screenshoty scény 7 pri jednotlivých osvetleniach. Tieto testy netreba prezentovať ako stále nevykonané; po budúcom refaktoringu však treba overiť zachovanie správania.

Zverejnenie videa na Discorde ani finálne odovzdanie do LMS/Kelvinu neboli výslovne potvrdené.

## 4. Aktuálne nastavenia, ktoré zachovať

Tieto hodnoty boli prečítané z kódu 29. 10. 2026; používateľ ich po našich úpravách menil sám.

| Nastavenie | Aktuálna hodnota / správanie |
|---|---|
| FOV v `Camera::updateProjection()` | **90°**, nie pôvodných 60° |
| Počiatočná kamera | `(0, 0, 3)`, pohľad smerom k zápornému Z |
| Shader štyroch testovacích gúľ | **`lambertProgram`**, nie predtým používaný Blinn |
| Rozmiestnenie gúľ | `(0, 0.8, 0)`, `(-0.8, 0, 0)`, `(0.8, 0, 0)`, `(0, -0.8, 0)` |
| Mierka testovacích gúľ | `0.35f`; svetlo v počiatku |
| Slnko | `ambientProgram`, pôvodné RGB, konštantné osvetlenie |
| Zem | `phongProgram`; obieha Slnko a otáča sa okolo vlastnej osi |
| Mesiac | `blinnProgram`; obieha Zem a otáča sa okolo vlastnej osi |
| Venator | `phongProgram`, polomer obehu `1.05f` |
| Imperial | `blinnProgram`, polomer obehu `1.40f` |
| Náklon lodí `shipTilt` | **65° okolo X**, nie pôvodných 25° |
| Mierka oboch lodí `shipScale` | `0.28f` |
| Rýchlosť obehu `shipOrbitSpeed` | `0.35f` rad/s, jeden obeh približne 18 s |
| Počiatočné fázy | Venator −45°, Imperial +45° |
| Opačný smer | Venator `+= speed * deltaTime`, Imperial `-= speed * deltaTime` |
| Dodatočné otočenie Imperialu | `imperialFacing`: 180° okolo Z, aby letel nosom dopredu |

Používateľ sa pýtal, prečo sú lode otočené rozdielne. Bolo vysvetlené dodatočné otočenie Imperialu aj vplyv polohy na dráhe; vtedy výslovne nechcel nič meniť. Tento rozdiel neopravovať automaticky ako chybu.

Skladanie transformácií lodí má poradie: základný stav Slnka → rotácia obehu → posun od Slnka → prípadné otočenie Imperialu → náklon → mierka. Rotácia obehu je pred posunom, preto loď obieha Slnko.
Vlastná rotácia a mierka samotného modelu Slnka sa do lodí ani obežných dráh planét neprenášajú.

## 5. Modely a export z Blenderu

Autor oboch stiahnutých lodí: **Digital Sock**, licencia uvedená na zdrojoch **CC Attribution**. Zdroj a autor sú zaznamenané aj v komentároch vyexportovaných hlavičiek.

- Venator: https://sketchfab.com/3d-models/venator-class-star-destroyer-af3648043a5b4fd394adfc32baee369e
- Imperial: https://sketchfab.com/3d-models/low-poly-imperial-star-destroyer-224ad417d1e246019fe35966a72e5125

| Súbor v `project\Models` | Symboly | Vrcholy | Trojuholníky |
|---|---|---:|---:|
| `venator.h` | `venatorVertices`, `venatorVertexCount` | 117 444 | 39 148 |
| `imperial.h` | `imperialVertices`, `imperialVertexCount` | 142 998 | 47 666 |

Pôvodné stiahnuté súbory sú mimo repozitára:

- `C:\Users\ASUS\Downloads\venator_class_star_destroyer.glb`
- `C:\Users\ASUS\Downloads\low_poly_imperial_star_destroyer.glb`

Skript `tools\export_model_to_header.py`:

- V Object Mode spracuje vybrané objekty typu MESH, teda aj viac častí jednej lode.
- Použije ich transformácie, trianguláciu, farby materiálov a transformované jednotkové normály.
- Pri exporte spoločne vycentruje geometriu a rovnomerne upraví najväčší rozmer na 2 jednotky; pôvodný model v Blenderi tým nemení.
- Podporuje nepriehľadné materiály s jednoduchými farbami. Obrázkové textúry automaticky neprevádza; pri prepojenej farbe vyhlási zrozumiteľnú chybu.
- Vynechá trojuholníky s nulovou plochou, kontroluje platnosť dát a zapisuje hlavičku cez dočasný súbor.
- V hlavičke vytvorí pole aj počet vrcholov a označí AI pomoc pri exporte.
- V súbore skriptu je momentálne `MODEL_NAME = "venator"` a venatorový zdrojový odkaz. Pri opätovnom exporte Imperialu zmeniť názov aj odkaz, aby nevzniklo nesprávne autorstvo/metadata alebo prepísanie Venatora.

Použitý Blender: `C:\Program Files\Blender Foundation\Blender 5.2\blender.exe`, overená verzia 5.2.2 LTS.

## 6. Architektúra a zásadné veci pri refaktoringu

`Application.cpp` má aktuálne približne **761 riadkov**. `Application.h` vlastní iba ukazovateľ na GLFW okno; väčšina zdrojov a stavov je lokálna v `Application::run()`.

| Trieda | Aktuálna zodpovednosť |
|---|---|
| `Application` | Okno/kontext, vytvorenie scén a zdrojov, vstupy, animácie, odosielanie uniformov a hlavná slučka |
| `Camera` | Poloha a smer, pohľadová a projekčná matica, pohyb a rozhlížanie |
| `Shader`, `ShaderProgram` | Kompilácia, linkovanie, aktivácia a uniformy |
| `Model` | VBO/VAO, atribúty vrcholov a jedno `glDrawArrays` |
| `DrawableObject` | Požičaný model a program, vlastný základný stav a voliteľná požičaná transformácia |
| `Scene` | Zoznam požičaných ukazovateľov na objekty a ich vykreslenie |
| `CompositeTransformation` | Požičané ukazovatele na transformácie, násobenie ich matíc v poradí pridania |

Najdôležitejšie obmedzenia:

1. **Životnosť:** `Scene`, `DrawableObject` ani `CompositeTransformation` nevlastnia všetko, na čo odkazujú. Model, program, objekt a jeho transformácie musia existovať počas celého používania scény. Vrátenie scény z pomocnej funkcie s odkazmi na jej lokálne premenné by vytvorilo neplatné odkazy.
2. **OpenGL kontext:** dnes lokálne GPU zdroje zaniknú pri návrate z `run()`, ešte pred zničením okna. Pri presune zdrojov do členov triedy vyriešiť aj poradie inicializácie a uvoľnenia. Členy sa ničia až po tele deštruktora; telo, ktoré už zničilo GLFW okno, by bolo pre ich GPU deštruktory problém.
3. **Stabilné adresy:** pri vektoroch objektov sa používa `reserve`. Presun alebo realokácia objektov môže zneplatniť ukazovatele uložené v scénach alebo transformáciách.
4. **Nekopírovateľné zdroje:** `Model` a `ShaderProgram` majú zakázané kopírovanie a uvoľňujú GPU zdroje v deštruktoroch. Nepredpokladať automaticky existujúce bezpečné presúvanie.
5. **Uniformy:** `ShaderProgram::setUniform()` pri neexistujúcom uniforme ukončí program. Pri zjednocovaní odosielania neposielať všetkým shaderom uniformy, ktoré nepotrebujú alebo ich linker odstránil.
6. **Farby a svetlo:** `useVertexColor` je float uniform ambientného, Phongovho a Blinnovho programu. V slnečnej sústave je 1, inde 0. Svetlo sústavy vychádza z polohy Slnka; v scéne 7 je v počiatku.
7. **Observer:** v existujúcom riešení sa nepoužíva. `Camera::m_shaderProgram` je starší nevyužitý ukazovateľ. Nezamieňať ho s implementovaným Observerom ani nezavádzať tento vzor len kvôli rozdeleniu súboru.

Formáty vrcholov sa líšia:

- `sphere`: XYZ + normála, 6 floatov.
- `sun`, `earth`, `moon`, `venator`, `imperial`: XYZ + RGB + normála, 9 floatov.
- `clockModel`: XYZ + normála + RGB, tiež 9 floatov, ale iné poradie.
- Trojuholník: XYZ + RGB, 6 floatov; druhá trojica nie je normála.

`Model.cpp` dnes dáva prvú trojicu na location 0, druhú na 1 a tretiu na 2. Pri šiestich hodnotách sa na location 2 opakuje druhá trojica, aby existujúca guľa dodala normálu osvetľovaciemu shaderu. Pri refaktoringu layoutov tieto rozdiely zachovať.

## 7. Čo chce používateľ robiť ďalej

Používateľovi prekáža veľkosť `Application.cpp` a chce pri ďalšom pokračovaní **refaktoring a prípadnú optimalizáciu**. Refaktoring ešte nezačal.

Odporúčaný postup na zváženie po jeho požiadavke:

1. Overiť aktuálny Git stav a fungujúce zostavenie ako východisko; používateľ môže medzitým upraviť hodnoty v kóde.
2. Navrhnúť malé rozdelenie prípravy scén, vstupov, animácií a vykresľovania s jasným vlastníctvom a životnosťou zdrojov.
3. Presúvať po jednej logickej časti, priebežne zostavovať a overovať rovnaké správanie scén.
4. Zvážiť výpočet pohľadovej matice len raz za snímku a zjednotenie opakovaného odosielania matíc bez straty potrebných uniformov.
5. Výkonové úpravy posudzovať podľa merania. Samotný menší počet riadkov aplikáciu nezrýchli. Existujúce scény už zdieľajú modely medzi inštanciami; nevytvárať znovu GPU model pre každý strom alebo každú snímku.

Odovzdávací archív úlohy 6 je samostatný snímok. Budúci refaktoring aktívneho priečinka `project` nemá automaticky prepisovať jeho kópie.

## 8. Stav odovzdania a čo môže byť ešte treba

V `Tasks\6.task` je pri kontrole 36 súborov:

- vlastné `.cpp` v `c++` a hlavičky tried v `h`,
- `ambient.frag`, `lambert.frag`, `phong.frag`, `blinn.frag`, `phong.vert`,
- screenshoty `jar0193_1a.png` až `jar0193_1d.png`,
- video `2026-10-25 15-34-52.mp4`,
- krátky súhrn `jar0193.md`.

Kontrola obsahu zdrojových kópií voči aktívnym súborom nenašla rozdiely k 29. 10. 2026.

**Ak sa ešte bude dokončovať odovzdanie:** lokálny priečinok zatiaľ neobsahuje modelové hlavičky vrátane `venator.h` a `imperial.h`, exportný skript, priečinok `shaders` ani podporné `tree.vert`, `bush.vert`, `clock.vert`, na ktoré aplikácia odkazuje. Overiť obsah skutočne odovzdávaného balíka podľa zadania; nepredpokladať, že lokálny priečinok je úplný alebo že už bol odovzdaný.

Zachovať zdroje a podmienky použitia modelov, kontrolovať autorstvo/AI označenie podľa pravidiel predmetu. Knižnice a build výstupy podľa zadania nepatria do odovzdania.

Podrobné `TASK_6.md` a doplnenie `BACKLOG.md` sú možné neskoršie študijné poznámky; používateľ zatiaľ požiadal iba o krátky odovzdávací MD a tento handoff.

## 9. Git stav pri kontrole

HEAD je stále `c4f272b` – `Add lighting test scene and preserve planet colors`.
Predchádzajúce relevantné commity sú `6db5087` a `8562fed`.

Zmeny od posledného commitu:

```text
 M project/Application.cpp
 M project/Camera.cpp
 M project/lambert.frag
 M project/project.vcxproj
 M project/project.vcxproj.filters
?? project/Models/venator.h
?? project/Models/imperial.h
?? tools/
?? Tasks/6.task/
?? docs/project-notes/HANDOFF_TASK_6.md
```

Tento nový `HANDOFF_REFACTORING.md` pri vytvorení tiež pribúda ako necommitnutý súbor.

Samostatná staršia zmena obrázka z úlohy 5 zostáva:

```text
 D Tasks/5.task/jar0193_5.png
?? Tasks/5.task/jar0193_3.png
```

Pôvodný handoff zaznamenal, že ide o premenovanie rovnakého obrázka podľa Git hashov. Automaticky ho nevracať ani nemiešať do prípadného commitu refaktoringu. Nový commit ani push neboli súčasťou vytvorenia tohto súboru.

## 10. Overenie a užitočné príkazy

V priebehu tohto chatu prešli:

- zostavenie Debug/x64 po vložení lodí a po doplnení ich opačného obehu,
- kompilácia a linkovanie všetkých deviatich shader programov v skutočnom OpenGL kontexte,
- GPU kontrola používaných uniformov, atribútov a zachovania pôvodných farieb planét,
- kontrola vyexportovaných lodí: počty dát, platné hodnoty a jednotkové normály,
- Blender exportér na skutočnom Venatore a na transformovaných skúšobných objektoch vrátane neuniformnej a zápornej mierky.

Používateľ potom potvrdil svoje praktické skúšanie shaderov a FOV a prípravu videa/screenshotov. Pri písaní tohto handoffu sa vykonali len čítania a kontroly stavu, nie nový build alebo vizuálny test.

Projekt nemá samostatný nakonfigurovaný formatter, linter ani automatickú testovaciu sadu. Pri plnom linkovaní sa občas objaví existujúce upozornenie `LNK4098` na CRT knižnice; nie je nové kvôli lodiam.

Príklad zostavenia s výstupmi mimo repozitára:

```powershell
Set-Location 'C:\Users\ASUS\source\repos\ZPG'
& 'C:\Program Files\Microsoft Visual Studio\18\Insiders\MSBuild\Current\Bin\MSBuild.exe' 'project\project.vcxproj' /t:Build /p:Configuration=Debug /p:Platform=x64 '/p:OutDir=C:\Users\ASUS\.codex\build-reviews\zpg-rocket-20261025\bin\' '/p:IntDir=C:\Users\ASUS\.codex\build-reviews\zpg-rocket-20261025\obj\' /m /nologo /v:minimal
```

Existujúca GPU kontrola mimo repozitára:

```powershell
& 'C:\Users\ASUS\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' 'C:\Users\ASUS\.codex\build-reviews\zpg-planet-colors-20261023\check_shaders.py'
```

Kontrola exportéra mimo repozitára:

```powershell
& 'C:\Program Files\Blender Foundation\Blender 5.2\blender.exe' --background --factory-startup --python-exit-code 1 --python 'C:\Users\ASUS\.codex\build-reviews\zpg-rocket-20261025\check_exporter.py'
```

Tieto cesty existovali pri kontrole. Pomocné testy a ich skúšobné výstupy nie sú súčasťou projektu ani odovzdania. C++ build sám nekompiluje GLSL; shaderové chyby sa overujú cez OpenGL.

## 11. Ovládanie a scény

| Kláves | Scéna |
|---|---|
| 1 | Login JAR0193 |
| 2 | Jedna testovacia guľa |
| 3 | Trojuholník a transformácie |
| 4 | Les: 74 stromov, 25 kríkov, zemina a dvoje hodiny |
| 5 | Slnečná sústava s oboma loďami |
| 6 | Samostatné hodiny |
| 7 | Štyri gule s jedným programom vybraným v kóde |

WASD pohybuje kamerou, pravé tlačidlo myši umožňuje rozhlížanie. Šípky posúvajú vybraný objekt, Q/E ho otáčajú, Z/X menia mierku a Escape ukončí aplikáciu.
Kamera je medzi scénami spoločná a automaticky sa neresetuje. Zmena záberu po pohybe a prepnutí scény nemusí znamenať chybu vykresľovania.

## 12. Správa na začiatok nového chatu

> Pokračujeme v mojom C++ OpenGL projekte ZPG. Prečítaj si `C:\Users\ASUS\source\repos\ZPG\docs\project-notes\HANDOFF_REFACTORING.md` a over aktuálny Git stav. Úlohu 6 sme implementovali, otestoval som shadery a FOV a pripravil screenshoty aj video. Chcem pokračovať postupným refaktoringom príliš veľkého `Application.cpp`, zachovať všetky scény a správanie a rozumieť zmenám. Najprv sa pozri na rozdelenie zodpovedností a životnosť modelov, shaderov, objektov a transformácií.
