# Prvé kroky refactoringu ZPG

Overené 1. 11. 2026 na aktívnom projekte `project`, Debug/x64.
Východiskom bol pracovný strom nad commitom `70ce788`, vrátane existujúcich
necommitnutých úprav cvičenia 7. Odovzdávacia kópia ani ZIP sa nemenili.

## 1. Light

- `Light.h` / `Light.cpp` oddeľujú polohu, RGB intenzitu a trojicu koeficientov
  útlmu bodového svetla od hlavnej slučky.
- `applyPosition()` obsluhuje existujúce shadery s uniformom `lightPosition`.
  `apply()` navyše vyžaduje `lightColor` a `attenuationFactors`; používa sa pri
  inicializácii troch nočných programov. Povinné uniformy sa stále kontrolujú.
- `Application.cpp` vyberá konkrétne svetlo podľa scény. Poloha slnečného svetla
  sleduje posun Slnka a nočné svetlo sleduje svetlušku, aj po prepínaní scén.
- Intenzita `(2.0, 1.8, 0.8)` a útlm `(1.0, 0.7, 1.8)` sa nastavujú z C++.
  Ich duplicitné predvolené hodnoty boli odstránené z `night.frag`.

Overenie: build prešiel; 23 renderov sa zhodovalo s východiskovou verziou bez
rozdielu pixelov, vrátane pohybu svetla v nočnom lese a planetárnej sústave.

## 2. Material

- `Material.h` / `Material.cpp` oddeľujú farbu povrchu, použitie farieb vrcholov,
  lesk a silu odlesku. Materiál neobsahuje polohu ani intenzitu svetla.
- Stromy a kríky zdieľajú vegetačný materiál s farbami vrcholov; zem používa
  samostatný hnedý materiál `(0.18, 0.12, 0.06)`. Lesk zostáva 16 a sila odlesku 0.15.
- Materiály sa aplikujú pri príprave scény na tri samostatné nočné programy.
  Ich hodnoty už nie sú duplicitne predvolené v `night.frag`.
- Rozsah je zatiaľ nočný les. Ostatné shadery ešte nemajú rovnaké materiálové
  uniformy; ich doterajšie nastavenia a prepínanie farieb planét zostávajú zachované.

Overenie: build prešiel; všetkých 23 renderov zostalo pixelovo zhodných.

Aktuálny `night.frag` mal pri začatí ambientnú zložku **0.2**, hoci handoff
uvádzal 0.015. Zachovaná bola hodnota z aktuálneho kódu, teda 0.2.

## 3. Prvé rozdelenie Application.cpp

- `InputController.h` / `InputController.cpp` preberajú posun, rotáciu a mierku
  vybraného objektu a ovládanie kamery klávesnicou a myšou.
- Kontrolér si požičiava okno a kameru a uchováva predchádzajúcu polohu myši.
  Jedna inštancia sa používa naprieč scénami. Výber scény a aktívneho objektu
  zostáva v `Application.cpp`.
- Klávesy, poradie spracovania, rýchlosti a dolná hranica mierky 0.05 sa nezmenili.
- GPU zdroje, objekty a transformácie majú pôvodné vlastníctvo aj životnosť.
  Tvorba scén a animácie zatiaľ zostávajú v `run()`. Pred presunom tvorby scén
  bude potrebné zaviesť vlastníka ich zdrojov a transformácií so stabilnými adresami.

Overenie: build prešiel; všetkých 23 renderov zostalo pixelovo zhodných, vrátane
ovládania objektov, kamery, minimálnej mierky a prepínania scén.

## 4. Oddelenie aktualizácie hodín

Overené 1. 11. 2026 nad commitom `0e23e79`. Pri začatí bol pracovný strom
čistý okrem používateľovho necommitnutého `REFACTORING_PLAN.md`, ktorý sa nemenil.

- `ClockAnimation.h` / `ClockAnimation.cpp` obsahujú bezstavovú funkciu
  `updateClockHands()`. Výpočet miestneho času, troch uhlov a odosielanie uniformov
  už nie sú zodpovednosťou hlavnej slučky. V `Application.cpp` zostalo volanie
  `updateClockHands(clockProgram)` na pôvodnom mieste po vymazaní framebufferu.
- Predvoleným vstupom je aktuálny systémový čas. Voliteľný explicitný časový okamih
  umožňuje deterministickú kontrolu bez zásahov do implementácie výpočtu.
- Zachované sú zlomkové sekundy, plynulé minúty a hodiny, smer otáčania a uniformy
  `hourMatrix`, `minuteMatrix`, `secondMatrix`. Samostatné aj lesné hodiny ďalej
  používajú pôvodný spoločný program.
- Tri dočasné rotácie patria funkcii; bezstavová aktualizácia nepotrebuje novú
  triedu. GPU zdroje a vlastníctvo scén sa v tomto kroku nemenili.
- `Application.cpp` má 766 riadkov namiesto 804. Nové súbory sú zaradené do
  `project.vcxproj` aj jeho filtrov. Existujúce Light, Material a InputController
  zostali bez zmien.

Overenie:

- Debug/x64 build prešiel pred úpravou aj po nej. Zostáva pôvodné `LNK4098`.
- Nové etapy `clock_before` a `clock_extracted` v existujúcom kontrolnom priečinku
  boli zostavené z príslušných aktuálnych zdrojov. Všetkých 23 renderov oboch etáp
  sa zhoduje s pôvodným `baseline` bez rozdielu pixelov; prešli aj kontroly všetkých
  14 shader programov, projekcie, svetiel, depth testu a uvoľnenia programov.
- Externý `verify_refactor.py` bol prispôsobený novému rozhraniu: pri oddelených
  hodinách odovzdáva pevný čas ako argument. Pôvodný `baseline` sa neprepisoval.
- Samostatný GPU test `check_clock.cpp` v
  `C:\Users\ASUS\.codex\build-reviews\zpg-animations-20261101` overil päť konkrétnych
  miestnych časov vrátane zlomkových sekúnd, polnoci a poludnia, všetkých 16 prvkov
  každej matice aj predvolené používanie aktuálneho systémového času.
- Zoznam a SHA-256 všetkých súborov v `Tasks`, vrátane ZIP-u, zostali nezmenené.
  Prešla aj kontrola whitespace cez `git diff --check`. Commit ani push sa nevykonal.

Ďalší krok podľa plánu: oddeliť stav a aktualizáciu animácie planét a lodí.
Vlastníctvo zdrojov a obsahu scén a presun ich tvorby zostávajú následnými etapami.

## Spoločné overenie prvých troch krokov

Pred úpravami aj po každom kroku prešiel build aktívneho `project.vcxproj`.
Linker naďalej vypisuje existujúce upozornenie `LNK4098`.

Pomocné kontroly a výstupy sú mimo repozitára:

`C:\Users\ASUS\.codex\build-reviews\zpg-refactor-20261101`

- `verify_refactor.py` pripravuje kontrolnú kópiu z aktuálneho `Application.cpp`,
  kompiluje aktuálne podporné zdroje a spúšťa reálne OpenGL vykresľovanie v skrytom
  okne. Na porovnanie používa pevné semeno lesa, čas, krok animácie a simulované vstupy.
- Priečinky `baseline`, `light`, `material` a `input` obsahujú snímky zdrojov,
  kontrolné projekty, binárky a výsledné PPM rendery jednotlivých krokov.
- Kontroly pokrývajú všetkých desať scén, tri režimy normál, oba režimy odlesku,
  pohyb svetlušky a Slnka, ovládanie kamery, rotáciu a mierku objektu, prepínanie scén
  a široký aj vysoký framebuffer. Nejde o ručný test používateľského rozhrania.
- Za behu sa kontroluje úspešné vytvorenie 14 shader programov, synchronizácia
  projekcie, polohy svetla, nočná intenzita a útlm, depth test a absencia OpenGL chýb.
  Shader programy sa uvoľnia ešte pred zničením kontextu.
- Po každom kroku bolo všetkých 23 renderov porovnaných s `baseline`:
  **nulový rozdiel pixelov**.
- SHA-256 všetkých existujúcich súborov v `Tasks`, vrátane `Tasks/jar0193_7.zip`,
  aj ich zoznam zostali nezmenené. Pôvodné odstránenie obrázka cvičenia 5 sa zachovalo.

Nové zdroje sú zaradené aj do `project.vcxproj` a jeho filtrov. Commit ani push
sa nevykonal.

## Refactoring podľa plánu zo 7. 11. 2026

Východiskom je aktuálny pracovný strom nad `6e0f24e`, vrátane necommitnutého
cvičenia 8 a letu kamery podľa plného smeru pohľadu. Existujúce zmeny sú zachované.
Nová referencia a záloha pôvodných zmien sú mimo repozitára v
`C:\Users\ASUS\.codex\build-reviews\zpg-refactoring-20261107`.
Historické referencie sa neprepisujú.

### Etapa 0: aktuálna referencia

- Debug/x64 build prešiel; pôvodné upozornenie `LNK4098` zostáva.
- `00-baseline` obsahuje novú referenciu 66 scenárov z aktuálnych zdrojov.
  Kontroly používajú skryté OpenGL okno, pevný čas, semeno 742 a simulované vstupy.
- Pred úpravami sa zaznamenali SHA-256 a zoznam všetkých 226 súborov v `Tasks`.

### Etapa 1: SolarAnimation

- `SolarAnimation` vlastní uhly a rýchlosti animácie, požičiava si sedem rotácií.
  Transformácie zatiaľ žijú v `run()`; animácia zaniká pred nimi.
- Aktualizácia zostala pred výberom scény, vrátane opačného smeru lodí
  a počiatočných fáz -45/+45 stupňov.
- Debug/x64 build prešiel. `01-solar`: štyri scenáre planét, posunu Slnka,
  prepínania a hodín sú pixelovo zhodné s novou referenciou; prešli GPU kontroly.

### Etapa 2: RenderResources

- Jeden nepohyblivý vlastník drží 12 pôvodných modelov a 14 programov.
  Všetkých 11 veľkých modelových hlavičiek je iba v `RenderResources.cpp`.
- Kamera žije dlhšie než zdroje. Deštruktor odregistruje programy pred ich zánikom.
  Programy textúr a oblohy zostali pri existujúcich vlastníkoch.
- Debug/x64 build prešiel. `02-resources`: všetkých 66 scenárov je pixelovo
  zhodných s `00-baseline`; platí aj kontrola Observera, textúr a GPU cleanupu.

### Etapa 3: stabilný obsah a základné scény

- `SceneContent` vlastní scény, objekty a polymorfné transformácie cez `unique_ptr`.
  Rast kolekcií nemení adresy. Vlastníka nemožno kopírovať ani presúvať.
- Malý `SceneBuilder` požičiava hotové modely/programy. `BasicScenes.cpp` tvorí
  login, guľu, zloženú transformáciu trojuholníka a spoločný podpis.
- `Scene` zostala nevlastniacim zoznamom na kreslenie. Návratové `SceneView`
  obsahujú iba referencie na obsah, ktorý prežije funkciu tvorby scény.
- Debug/x64 build prešiel. `03-basic`: osem scenárov základných scén, ovládania,
  minimálnej mierky a podpisu v lese/planétach je pixelovo zhodných.

### Etapa 4: denný a nočný les

- `ForestScenes.cpp` tvorí samostatný denný/nočný obsah, materiály, Slnko a svetlušku.
  Zachováva 74 stromov, 25 kríkov, distribúcie, poradie RNG a nočnú čistinku.
- Lokálne zoznamy stromov/kríkov obsahujú iba požičané ukazovatele; objekty drží
  `SceneContent`. Oba lesy používajú jeden model stromu a jeden model kríka.
- Debug/x64 build prešiel. `04-forests`: osem scenárov vrátane pohybu svetla,
  kamery, skyboxu, skydomu a návratu na pôvodné pozadie je pixelovo zhodných.

### Etapa 5: hodiny a testy osvetlenia

- `ClockScene.cpp` tvorí samostatné hodiny aj dve lesné inštancie; zdieľajú mesh,
  program aj odraz Z, ktorý vlastní `SceneContent`.
- `LightingTestScenes.cpp` tvorí štyri Lambertove gule, elipsoid a test odlesku.
  Nastavuje pôvodné predvolené uniformy normál a odvráteného odlesku.
- Debug/x64 build prešiel. `05-clock-lighting`: desať scenárov hodín, lesa,
  normál, odlesku a resize je pixelovo zhodných.

### Etapa 6: slnečná sústava

- `SolarScene.cpp` skladá všetky orbitálne transformácie, planéty a lode.
  Poradie kompozitov zostalo rovnaké vrátane oddelenej orbitálnej polohy Zeme.
- Všetky transformácie teraz vlastní `SceneContent`; `SolarAnimation` si požičiava
  jeho stabilné rotácie a zaniká pred vlastníkom. Podpis zostáva spoločný.
- Debug/x64 build prešiel. `06-solar-scene`: štyri scenáre planét, posunu Slnka,
  prepínania a návratu ku guli sú pixelovo zhodné.

### Etapa 7: tabuľka scén a svetlá

- `SceneSelection` obsahuje 16 záznamov pre `1..9, 0, F1..F6`. Každý spája
  scénu, ovládaný objekt, svetlo a oblohu. Pri súčasnom stlačení naďalej vyhráva
  posledný záznam; bez klávesu zostáva aktívny predchádzajúci výber.
- `SceneLighting` nadväzuje na existujúci `Light`. Poloha svetla sleduje Slnko
  alebo svetlušku po spracovaní vstupu. Farby vrcholov planét sa zapínajú iba
  pre slnečnú scénu. Nočná intenzita, útlm a ambient 0.2 sú zachované.
- `Application.cpp` má **154 riadkov namiesto 813**, `run()` má **50 riadkov**.
  Obsahuje inicializáciu, zostavenie vlastníkov a riadenie snímky. Všetka tvorba
  pôvodných scén je v tematických súboroch; nevznikol ďalší všeobecný manager.
- Poradie lokálnych vlastníkov je kamera, zdieľané zdroje, obsah a potom ich
  používatelia. Animácia zaniká pred transformáciami, obsah pred GPU zdrojmi,
  programy pred kamerou a všetko pred oknom/OpenGL kontextom.
- `.vcxproj` aj `.filters` obsahujú všetky nové súbory. Existujúce `InputController`,
  `ClockAnimation`, `Texture`, `TextureScenes`, `SkyRenderer` a shadery sa nemenili.

Finálne overenie:

- Debug/x64 build prešiel po každej etape. Zostáva pôvodné `LNK4098`.
- `07-selection`: všetkých **66** pôvodných scenárov je pixelovo zhodných
  s novou referenciou `00-baseline`. Kontroluje sa aj 18 programov, Observer,
  svetlá, textúrovacie jednotky, obnovovanie GL stavu a zánik GPU zdrojov.
- `08-edges-baseline` a `08-edges-final`: ďalších **18** scenárov je pixelovo
  zhodných. Zahŕňajú súčasné stlačenie čísel/F-klávesov, držanie poslednej voľby,
  spoločný denný les vo F5/F6/4, pohyb objektov, kameru naprieč scénami,
  dočasný nulový framebuffer a planéty aktualizované pri neaktívnej scéne.
- Pri chýbajúcej detailovej textúre aj cubemap ploche sa zdroje uvoľnili ešte
  pred zničením kontextu. Produkčné obrázky sa pri teste nemenili.
- Pri dvoch opakovaných prekladoch samotného `Application.cpp` s rovnakými
  Debug/x64 nastaveniami trval pôvodný súbor **8.81/8.86 s**, nový **2.42/2.41 s**.
  Ide o izolovaný preklad jednej jednotky cez MSBuild, bez linkovania a bez
  zmeny produkčných zdrojov. Čistý build stále spracuje geometriu v `RenderResources.cpp`.
- Zoznam a SHA-256 všetkých **226** súborov v `Tasks` zostali nezmenené.
  Existujúce necommitnuté súbory cvičenia 8 sú zachované. Commit ani push sa nerobil.
- Kontroly sú automatizované cez skryté OpenGL okno; finálny prehľad renderov
  sa skontroloval aj vizuálne. Ručný test fyzickou klávesnicou sa nevykonával.

Pomocné skripty, snímky zdrojov, buildy a výsledky zostali mimo repozitára:
`C:\Users\ASUS\.codex\build-reviews\zpg-refactoring-20261107`.

Jadro plánu (etapy 0-7) je dokončené. Voliteľná etapa 8 — zjednotenie materiálov,
formátov vrcholov a uniformov — zostáva samostatnou ďalšou prácou.

## Zjednodušenie podľa požiadavky používateľa — 7. 11. 2026

Používateľovi predchádzajúce rozdelenie s mnohými pomocnými súbormi nevyhovovalo.
Výslovne schválil väčší `Application.cpp`, v ktorom vidí prípravu scén pokope,
a ponechanie samostatných tried pre vstupy, zdroje, textúry a oblohu.
Východiskom je aktuálny pracovný strom nad `ef20612`, vrátane jeho vlastných úprav;
staršia verzia aplikácie z GitHubu sa nekopírovala späť.

Čo sa zmenilo:

- `Application.cpp` má 654 riadkov. Označené bloky priamo vytvárajú základné scény,
  lesy, testy osvetlenia, planéty, hodiny a textúrovacie ukážky.
- Objekty a transformácie sú lokálne premenné v `run()`. Opakované objekty používajú
  `vector<DrawableObject>` s vopred rezervovanou kapacitou, aby odkazy v scénach
  zostali počas hlavnej slučky platné. GPU modely sa stále zdieľajú.
- Výpočet ručičiek zostal malou funkciou `updateClockHands()` priamo v aplikácii.
  Stav a aktualizácia orbitov sú priamo pri hlavnej slučke.
- Samostatné podmienky vyberajú scénu, ovládaný objekt a oblohu; zachováva sa
  pôvodná priorita `1..9, 0, F1..F6` aj ponechanie poslednej voľby bez stlačenia.
- Svetlá sú lokálne hodnoty `Light`; poloha Slnka a svetlušky sa číta po vstupe.
- Zostali `InputController`, `RenderResources`, `Texture`, `SkyRenderer`, základné
  triedy kamery, Observera, modelov, shaderov, scén, materiálov a transformácií.
  Veľké pôvodné geometrické hlavičky sa naďalej prekladajú v `RenderResources.cpp`.
- Odstránilo sa 20 súborov: dvojice `ClockAnimation`, `SolarAnimation`,
  `SceneBuilder`, `SceneContent`, `SceneSelection`, `SceneLighting`, `TextureScenes`,
  ďalej `SceneBuilders.h` a päť pôvodných tematických `.cpp` súborov scén.
  Ich položky sa odstránili aj z `.vcxproj` a `.filters`.
- Používateľov rozpracovaný `ClassromScene.h`, export `Models/eb408.h`, jeho úpravy
  modelov, shaderových atribútov, kamery a kreslenia zostali zachované.
- Aktualizovali sa poznámky cvičenia 8 a handoff pre pokračovanie učebňou.
  Predchádzajúci plán a refactoringový handoff sú označené ako historické.

Overenie:

- Nový produkčný Debug/x64 build prešiel; zostáva pôvodné `LNK4098`.
- Čerstvá referencia pred zmenou aj zjednodušená aplikácia prešli všetkých
  66 hlavných a 18 okrajových scenárov. Všetky výsledné snímky sú pixelovo zhodné.
- Zachované sú ovládanie, kamera a Observer, 18 programov, normály, odlesk,
  osvetlenie, textúry, skybox/skydome, resize aj animácia neaktívnych planét.
- Overil sa zánik GPU zdrojov pred kontextom aj pri chýbajúcom detailovom PNG
  a cubemap JPG; produkčné obrázky sa pri teste nemenili.
- Kontroly zoznamu súborov aj SHA-256 chránia pôvodných 226 súborov v `Tasks`.
  Commit ani push sa nerobil. Ručný test fyzickou klávesnicou sa nevykonával.

Záloha pracovného stromu pred úpravou, upravené externé helpery, buildy a výsledky:
`C:\Users\ASUS\.codex\build-reviews\zpg-simplify-20261107`.

Toto je aktuálne požadované rozdelenie. Ďalším obsahovým krokom je učebňa EB408,
nie obnovenie odstránených pomocných tried podľa starého plánu.

## Čitateľnejšie formátovanie aplikácie — 7. 11. 2026

- Na požiadavku používateľa sa vizuálne oddelili vytváranie objektov, nastavenia
  transformácií a priraďovanie do scén. Dlhé konštruktory a inicializátory sú
  rozdelené na kratšie riadky; scény a časti hlavnej slučky majú krátke popisy.
- Kód zostal v rovnakých funkciách a súbore. Po pridaní medzier a komentárov
  má `Application.cpp` 872 riadkov. Poradie a obsah všetkých 4460 C++ tokenov
  vrátane reťazcov sú zhodné s verziou pred formátovaním.
- Nový Debug/x64 build prešiel, zostáva pôvodné `LNK4098`. Celá render sada sa
  neopakovala, pretože sa zmenili iba komentáre a formátovanie.
- Záloha pôvodnej aplikácie a kontroly sú mimo repozitára v
  `C:\Users\ASUS\.codex\build-reviews\zpg-readability-20261107`.

### Následná úprava na kompaktnejší štýl

Používateľ následne uprednostnil striedmejšie medzery a stručné označenia scén
podobne ako v Adamekka/zpg. `Application.cpp` má teraz 734 riadkov oproti 837
pri začatí tejto úpravy. Krátke konštruktory sú na jednom riadku, súvisiace
príkazy spolu a medzery oddeľujú väčšie celky. Žiadne triedy ani funkcie sa
nepresúvali. Všetkých 4460 C++ tokenov zostalo zhodných a nový Debug/x64 build
prešiel. Záloha a kontrola sú v
`C:\Users\ASUS\.codex\build-reviews\zpg-compact-style-20261107`.
