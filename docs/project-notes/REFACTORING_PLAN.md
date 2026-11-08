# ZPG - refactoring Application podľa aktuálneho projektu a Adamekka/zpg

> **Historický plán.** Používateľ 7. 11. 2026 výslovne zvolil jednoduchšiu štruktúru: tvorba scén, animácie, výber scény a svetlá sa presunuli späť do `Application.cpp`. Buildery a pomocní vlastníci sa odstránili. Tento plán znovu neimplementovať; aktuálny stav opisuje záver `REFACTORING_PROGRESS.md` a `HANDOFF_NEXT_TASK.md`.

**Aktualizované 7. 11. 2026 po dokončení cvičenia 8.** Projekt: Martin Jarabica, jar0193.

Toto je hlavný plán ďalšieho refactoringu. Vznikol na požiadavku najprv vysvetliť kamarátovu architektúru a potom navrhnúť rozumné skrátenie nášho `Application.cpp`. Pri tejto aktualizácii sa mení dokumentácia, nie aplikačný kód.

Predchádzajúca verzia je zachovaná v [archíve z 1. 11. 2026](/C:/Users/ASUS/source/repos/ZPG/docs/project-notes/archive/REFACTORING_PLAN_2026-11-01.md). Staršie handoffy a `REFACTORING_PROGRESS.md` opisujú predchádzajúce etapy; tento plán už zohľadňuje textúry, oblohu a priestorový pohyb kamery.

## 1. Ako to má kamarát

Overený repozitár: [Adamekka/zpg](https://github.com/Adamekka/zpg). Pri kontrole 7. 11. 2026 ukazovala vetva `main` na commit [`8c9f659a18d8621ddc903b19d83308dcc15c7829`](https://github.com/Adamekka/zpg/commit/8c9f659a18d8621ddc903b19d83308dcc15c7829), označený `cv7` z 31. 10. 2026. Nasledujúce odkazy sú pripnuté k tejto verzii.

| Súbor | Riadky | Čo v ňom je |
| --- | ---: | --- |
| [app.cpp](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/app.cpp) | 582 | Inicializácia OpenGL, príprava scén, výber scény, vstupy a hlavná slučka |
| [app.hpp](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/app.hpp) | 35 | Okno, `vector<Scene>` a index aktívnej scény |
| [scene_factory.cpp](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/scene_factory.cpp) | 388 | Spoločná príprava objektov, shaderov, materiálov a animácií |
| [scene.hpp](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/scene.hpp) | 48 | Vlastníctvo kamery, svetla, programov, objektov a aktualizačnej funkcie |
| [scene.cpp](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/scene.cpp) | 38 | `update()` a `draw()` jednej scény |
| [drawable_object.cpp](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/object/drawable_object.cpp) | 156 | Vykreslenie cez materiál, podmodely a podpora importovaných objektov |

Počty zahŕňajú komentáre aj prázdne riadky. Sú opisom overenej verzie, nie cieľom pre náš projekt.

### 1.1 Prečo má kratší app.cpp

Používa `SceneFactory::new_scene(camera)` a volania ako `add_tree()`, `add_bush()`, `add_sphere()`, `add_clock()` a `add_model()`. Factory pozná model, formát vrcholov, príslušné shadery a materiál. Autor scény zadáva hlavne rozmiestnenie a transformácie.

Jeho [get_shader()](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/scene_factory.cpp#L338) zdieľa program podľa dvojice vertex/fragment súborov v rámci vytváranej scény. `build()` odovzdá programy a objekty vlastníckej `Scene`.

Samotný opis scén však stále zostáva v aplikácii: `create_objects()` má približne **410 riadkov** a `run()` približne **109 riadkov**. Factory obsahuje ďalších 388 riadkov. Podstatou je spoločná príprava opakovaných objektov, nie zmiznutie všetkého kódu.

### 1.2 Čo má vyriešené mimo aplikácie

- `Scene` vlastní objekty a programy a poskytuje `update()` a `draw()`.
- Factory ukladá aktualizačné funkcie, napríklad animáciu hodín. `Scene::update()` ich spúšťa.
- [Material::bind()](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/object/material/material.cpp#L46) nastaví vlastnosti materiálu pred kreslením konkrétneho objektu.
- Vstupné klávesy scén sú v poli a scény vo vektore; výber používa index.
- Pri ukončení aplikácie sa najprv vyprázdnia scény a až potom zničí OpenGL kontext.

### 1.3 Čo preberieme ako princíp a čo zachováme vlastné

| Oblasť | Rozhodnutie pre náš projekt |
| --- | --- |
| Krátke pridávanie stromov, kríkov a gulí | Použiť malý `SceneBuilder` so spoločnou prípravou objektov |
| Vlastníctvo obsahu | Pridať `SceneContent`, ktorý bezpečne drží objekty a transformácie |
| Rozdelenie scény a hlavnej slučky | Presunúť aj samotné rozmiestnenie scén do tematických súborov |
| Materiál pri kreslení | Užitočná ďalšia etapa po rozdelení vlastníctva |
| Zoznam scén | Použiť tabuľku našich klávesov, objektov, svetiel a režimu oblohy |
| Modely | Zachovať naše zdieľané GPU modely |
| Kamera | Zachovať našu spoločnú kameru, Observer a aktuálne ovládanie |

Dôležitý rozdiel: jeho [SceneFactory::add_object()](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/scene_factory.cpp#L302) pri každom pridaní vytvára `Model::from_raw_data()`. Ten vytvorí [nový VBO](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/src/object/model/vbo.cpp). Naše stromy už používajú jeden spoločný `Model`. Builder preto dostane hotový `Model&` a bude vytvárať len ďalšie objekty s vlastnými transformáciami.

Kamarát má vlastnú kameru v každej scéne a aktualizuje aktívnu scénu. U nás kamera zostáva spoločná a planéty pokračujú v animácii aj pri zobrazení inej scény. Tieto vlastnosti zachovať.

Jeho [CMakeLists.txt](https://github.com/Adamekka/zpg/blob/8c9f659a18d8621ddc903b19d83308dcc15c7829/CMakeLists.txt) žiada C++26 a jeho kód používa aj vlastnú matematiku či importer. Náš refactoring zostáva pri Visual Studiu, C++20, GLM, OpenGL 4.3 a GLSL 410. Tieto ďalšie technológie nie sú potrebné na skrátenie aplikácie.

## 2. Aktuálny stav nášho projektu

- Skutočný repozitár: `C:\Users\ASUS\source\repos\ZPG`.
- Aktívny projekt: `project\project.vcxproj`, riešenie `ZPG.slnx`, Debug/x64.
- Pracovný priečinok aplikácie: `project`, kvôli relatívnym cestám shaderov a obrázkov.
- Cesta `C:\Users\ASUS\Documents\ChatGPT\ZPG` uvádzaná prostredím chatu pri kontrole neexistuje.
- Git HEAD pri kontrole: `6e0f24e6669575dcf971a1656ada046d15052609`. Pracovný strom obsahuje ďalšie necommitnuté zmeny a nové súbory, vrátane dokončenia cvičenia 8. Východiskom je pracovný strom, nie samotný HEAD.
- Komunikácia po slovensky; názvy, kód a komentáre po anglicky.

| Už hotová časť | Aktuálna zodpovednosť |
| --- | --- |
| `Light`, `Material` | Parametre svetla a povrchu; materiály sú zatiaľ hlavne pri nočnom lese |
| `InputController` | Pohyb objektu, kamery a stav myši |
| `ClockAnimation` | Bezstavové `updateClockHands()`, aj s explicitným časom na testovanie |
| `CameraObserver` | Prenos pohľadovej/projekčnej matice a polohy kamery do programov |
| `Texture` | Vlastníctvo 2D textúry, RGBA upload, mipmapy, automatické uvoľnenie |
| `DrawableObject::setTexture()` | Požičané textúry a jednotky; ich väzby sa nastavujú pri draw |
| `TextureScenes` | Vlastní textúry, modely, programy a objekty ukážok F1-F4 |
| `SkyRenderer` | Vlastní cubemap, panorámu, geometriu oblohy a jej dva programy |
| Kamera | W/S sa pohybuje plným smerom pohľadu vrátane Y; pravé tlačidlo myši mení smer |

Tieto časti znovu nezavádzať. `TextureScenes` a `SkyRenderer` už ukazujú, že sa ucelený obsah dá držať mimo aplikácie s platnými referenciami.

### Kde vzniká dĺžka a pomalé zostavovanie

Pri kontrole mal `Application.cpp` **813 riadkov**. Pred hlavnou slučkou je vyše 500 riadkov vrátane vytvárania programov, modelov, objektov, transformácií a scén.

Aplikácia priamo zahŕňa **11 hlavičiek s geometriou**, spolu **49 316 743 bajtov, približne 47 MiB textových definícií vrcholov**. Najväčšie sú lode, hodiny a strom. Nejde o veľkosť GPU dát ani samotný čas behu aplikácie.

Oddelením týchto includes do `RenderResources.cpp` nebude bežná zmena v `Application.cpp` vyžadovať nové spracovanie celých polí. Úplný čistý build ich bude stále musieť preložiť v príslušnom súbore. Reálny prínos treba zmerať na opakovanom zostavení po zmene aplikácie.

## 3. Výsledok, ktorý chceme

`Application` má zabezpečiť okno a kontext, zostaviť hlavné časti aplikácie a riadiť snímku. Rozmiestnenie stromov, skladanie orbitálnych transformácií ani príprava modelových polí do nej nepatria.

Orientačný cieľ: **40-80 čitateľných riadkov v `run()` a približne 150-250 v celom `Application.cpp`**. Hodnotí sa rozdelenie zodpovedností a bezpečná životnosť, nie dosiahnutie počtu za každú cenu.

Dokončenie jadra znamená:

1. Veľké modelové hlavičky a vytváranie zdieľaných GPU zdrojov sú mimo aplikácie.
2. Tvorba všetkých pôvodných scén je v tematických súboroch.
3. Orbitálne uhly a ich aktualizácia sú v `SolarAnimation`.
4. Tabuľka výberu spája scénu, ovládaný objekt, svetlo a oblohu.
5. Cvičenie 8, spoločná kamera a Observer zostávajú funkčné.
6. Referencie sú platné a GPU objekty sa uvoľnia pred kontextom.

## 4. Navrhované rozdelenie súborov

Nasledujúce nové názvy sú návrh implementácie, nie opis už existujúcich tried. Pri podobnej zodpovednosti nevytvárať viac konkurenčných obalov.

```text
project/
  Application.h/.cpp          okno, kontext, hlavná slučka
  RenderResources.h/.cpp      zdieľané pôvodné modely a programy
  SceneContent.h/.cpp         stabilné vlastníctvo obsahu pôvodných scén
  SceneBuilder.h/.cpp         krátke addTree/addBush/addSphere a spoločné nastavenia
  SceneBuilders.h            deklarácie funkcií na prípravu skupín scén
  BasicScenes.cpp            login, guľa, trojuholník a podpis
  ForestScenes.cpp           denný/nočný les a ich dekorácie
  SolarScene.cpp             planéty, lode a ich transformácie
  ClockScene.cpp             samostatná scéna hodín
  LightingTestScenes.cpp     štyri gule, normály, odvrátený odlesk
  SolarAnimation.h/.cpp       stav a aktualizácia orbitálnych rotácií
  TextureScenes.h/.cpp       existujúci vlastník ukážok F1-F4
  SkyRenderer.h/.cpp         existujúci samostatný priechod oblohy
  Texture.h/.cpp             existujúci vlastník 2D obrázka
```

Na začiatku možno nové súbory nechať v koreňovom `project`, aby sa nemenili cesty. Triedenie do podadresárov je samostatné upratovanie. Vždy aktualizovať `.vcxproj` aj `.filters`.

### 4.1 RenderResources

Najprv prevezme pôvodné zdieľané modely a **14 programov, ktoré sa dnes vytvárajú priamo v aplikácii**. Dva programy `TextureScenes` a dva programy oblohy zostávajú pri svojich existujúcich vlastníkoch. Celkovo dnes vzniká 18 programov.

- Modelové polia zahŕňať len v `.cpp`, nie v hlavičke nového vlastníka.
- Poskytovať hotové `Model&` a `ShaderProgram&` pre buildery.
- Zachovať jeden model stromu a kríka zdieľaný denným aj nočným lesom.
- Modely ani programy netreba presúvať hodnotou do rastúcich vektorov. Môžu byť pomenovanými členmi alebo vo vlastniacich `unique_ptr`.
- Registrácia Observera musí mať jasnú životnosť. Ak vlastník dostane `Camera&`, v deštruktore pred zánikom programov odregistrovať vlastné programy.

Prvý presun nezavádza deduplikáciu shaderov podľa podobnosti názvov. Programy majú vlastné uniformy a niektoré nastavenia sa dnes aplikujú iba raz pri inicializácii.

### 4.2 SceneContent

Odporúčaná jednoduchá cesta: jeden vlastník obsahu pôvodných scén, ktorého napĺňajú buildery. Vlastní scény, kreslené objekty a transformácie; podľa potreby aj príslušné scénové svetlá. Modely a programy si požičiava z `RenderResources`.

Existujúca `Scene` môže zostať ľahkým zoznamom na kreslenie. Netreba súčasne pridávať vlastníctvo do `Scene` aj ďalšieho vlastníka tých istých objektov.

- Objekty a transformácie ukladať na stabilných adresách, napríklad ako `vector<unique_ptr<...>>`.
- `SceneContent` nekopírovať a počas používania nepresúvať. Vytvoriť ho lokálne v `run()` a napĺňať cez referenciu.
- Funkcia tvorby scény pridáva objekty do tohto vlastníka; jej lokálne pomocné objekty sa nesmú stať cieľom uložených ukazovateľov.
- Spoločný podpis a transformácie hodín môžu patriť tomu istému vlastníkovi a zostať zdieľané medzi jeho scénami.
- Životnosť transformácií musí pokrývať aj celý život `SolarAnimation`, ktorá si môže požičiavať rotácie.

### 4.3 SceneBuilder a tematické funkcie

Builder dostane `RenderResources&` a `SceneContent&`. Metódy typu `addTree(scene, transform)` vrátia odkaz na vytvorený objekt, aby sa dal prípadne ovládať. Metóda vytvorí inštanciu objektu s už existujúcim modelom a správnym shaderom.

Jednotlivé funkcie vo `ForestScenes.cpp` či `SolarScene.cpp` obsahujú konkrétne rozmiestnenie a poradie transformácií. Builder rieši spoločnú prípravu; nekóduje v sebe celý les ani všetky scény.

Nezavádzať druhý všeobecný manager s celým pôvodným obsahom aplikácie. Nevyžadovať samostatnú odvodenú triedu pre každú drobnú scénu.

### 4.4 Životnosť kontextu a Observera

Bezpečný východiskový poriadok v `run()` je približne:

```cpp
Camera camera;
RenderResources resources(camera);
SceneContent content;
// Populate content and create metadata referencing its objects.
TextureScenes textures(camera);
SkyRenderer sky(camera);
```

Ide o navrhované rozhranie. Kamera žije dlhšie než registrované programy. Obsah scén zaniká skôr než jeho zdieľané GPU modely. Všetky tieto lokálne objekty zanikajú pred `Application::~Application()`, ktorý ničí okno a kontext.

`Application` teraz drží len okno. Pri budúcom presune GPU vlastníkov do jej členov treba najprv vyriešiť poradie deštrukcie: členy zanikajú až po tele deštruktora. Pre prvé rozdelenie ponechať vlastníkov lokálne v `run()`.

## 5. Hlavná slučka po rozdelení

Nasleduje cieľový pseudokód. Pomocné funkcie a `SceneEntry` ešte v tejto podobe neexistujú:

```cpp
while (!glfwWindowShouldClose(window)) {
    const float dt = calculateDeltaTime(lastTime);
    solarAnimation.update(dt);
    selectScene(entries, active);
    input.update(*active->controlledObject, dt);
    if (!updateViewportAndProjection(camera)) {
        glfwPollEvents();
        continue;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    sky.draw(active->skyMode);
    updateClockHands(resources.clockProgram);
    applySceneLighting(*active);
    active->scene->draw();
    glfwSwapBuffers(window);
    glfwPollEvents();
}
```

Pomocné funkcie majú uchovať aj pôvodnú aktualizáciu svetiel a prepínanie farieb planét. Toto je rozdelenie existujúcej práce, nie požiadavka zaviesť nový renderer alebo systém udalostí.

## 6. Poradie implementácie

| Etapa | Konkrétna zmena | Kedy je hotová |
| --- | --- | --- |
| 0 | Načítať aktuálny Git stav, overiť build a pripraviť referenciu | Referencia vznikla zo súčasného pracovného stromu vrátane cvičenia 8 |
| 1 | Vytiahnuť `SolarAnimation` | Uhly/rýchlosti a volania orbitálnych rotácií sú mimo slučky; obrazy a časovanie sa zhodujú |
| 2 | Zaviesť `RenderResources` | Veľké modelové includes a tvorba zdieľaných modelov/programov odišli z aplikácie |
| 3 | Zaviesť `SceneContent`, malý builder a presunúť základné scény | Objekty/transformácie majú stabilných vlastníkov; login, guľa a trojuholník sa zhodujú |
| 4 | Presunúť denný a nočný les | Zachované rozmiestnenie, RNG poradie, zdieľanie modelov, hodiny, podpis a svetluška |
| 5 | Presunúť hodiny a testy osvetlenia | V aplikácii nezostali ich konštrukčné bloky a transformácie |
| 6 | Presunúť slnečnú sústavu | Všetky väzby rotácií a odkazy animácie ukazujú na žijúce transformácie |
| 7 | Zaviesť tabuľku výberu a upratať svetlá | Jedna voľba určuje scénu, aktívny objekt, svetlo a oblohu; slučka je krátka |
| 8 | Posúdiť materiály, formáty vrcholov a uniformy | Každé ďalšie zjednodušenie má samostatný prínos a overenie |

Po jednej etape zostaviť a overiť aktuálny kód, vysvetliť zmenu a aktualizovať záznam postupu. Samotné oddelenie animácie je prvý krok; hlavný cieľ skracovania aplikácie dosahujú etapy 2-7.

## 7. Tabuľka scén a režimov, ktoré musia zostať

Možný malý záznam:

```cpp
struct SceneEntry {
    int key;
    Scene* scene;
    DrawableObject* controlledObject;
    const Light* light;
    SkyRenderer::Mode skyMode;
};
```

Tabuľka si požičiava odkazy od vlastníkov. Poradie kontrol musí zostať `1..9, 0, F1..F6`; pri viacerých stlačených klávesoch dnes vyhrá posledná vyhodnotená vetva. Kamarátov cyklus s prvým `break` by toto správanie zmenil.

| Kláves | Scéna/režim | Ovládaný objekt | Obloha |
| --- | --- | --- | --- |
| 1 | Login, počiatočná scéna | Login | None |
| 2 | Jedna guľa | Guľa | None |
| 3 | Transformovaný trojuholník | Trojuholník | None |
| 4 | Denný les | Prvý strom | None |
| 5 | Slnečná sústava | Slnko | None |
| 6 | Hodiny | Hodiny | None |
| 7 | Štyri Lambertove gule | Prvá guľa | None |
| 8 | Test transformácie normál | Elipsoid | None |
| 9 | Test odvráteného odlesku | Guľa | None |
| 0 | Nočný les | Svetluška | None |
| F1 | Textúrované drevo | Rovina | None |
| F2 | Dvere | Dvere | None |
| F3 | Dva obrázky na jednom modeli | Rovina | None |
| F4 | Dva modely s rôznymi textúrami | Rovina | None |
| F5 | Denný les s učiteľovým skyboxom | Ten istý prvý strom | Cube |
| F6 | Denný les s procedurálnym skydomom | Ten istý prvý strom | Dome |

F5/F6 používajú ten istý denný les; nevytvárať jeho ďalšie kópie. Prepnutie scény neresetuje kameru. Svetlo planét ďalej sleduje posun Slnka a nočné svetlo svetlušku.

## 8. Správanie, ktoré refactoring zachová

### Kamera a ovládanie

- Jedna spoločná kamera a existujúci Observer. Dnes je zapojených 18 programov, vrátane textúr a oblohy.
- FOV 90 stupňov, near/far 0.1/100, počiatočné oko `(0,0,3)` a smer `(0,0,-1)`.
- Pravé tlačidlo myši mení pohľad. W/S lieta podľa celého smeru vrátane Y. A/D zostáva bočný pohyb.
- Rýchlosť kamery 2, citlivosť myši 0.003. Nepridávať klávesy na výšku ani nepreberať inú normalizáciu diagonálneho pohybu ako súčasť presunu kódu.
- Šípky posúvajú aktívny objekt, Q/E ho otáčajú, Z/X škálujú; minimálna mierka 0.05. Zachovať rýchlosti aj stav myši naprieč scénami.
- Resize aktualizuje viewport aj projekciu; pri nulovom framebufferi sa nekreslí.

### Les, svetlá a animácie

- Denný a nočný les zostávajú samostatné. Denný má 74 stromov, 25 kríkov, Slnko, podpis a dvoje hodiny.
- Produkčný les používa náhodné semeno. Pri presune zachovať distribúcie, poradie odberov a kopírovanie rozmiestnenia do nočnej scény.
- Nočná čistinka, ručný pohyb svetlušky a jej počiatočná poloha `(0,-0.3,0)` zostávajú.
- Nočný ambient je **0.2**, RGB svetla `(2,1.8,0.8)`, útlm `(1,0.7,1.8)`. Staré odovzdávacie obrázky s ambientom 0.015 nie sú východiskom refactoringu.
- Všetky štyri gule v scéne 7 používajú Lambertov model.
- Planéty a lode sa aktualizujú aj pri neaktívnej slnečnej scéne. Hodiny používajú skutočný miestny čas a zlomkové sekundy.
- Zachovať farby planét/lodí, Ambient Slnko, Phong Zem/Venator a Blinn Mesiac/Imperial.
- Zachovať osi, rýchlosti a počiatočné fázy orbitov, opačný smer lodí, sklon 65 stupňov, mierku 0.28 a otočenie Imperialu o 180 stupňov okolo Z.
- Mesiac sleduje orbitálnu polohu Zeme, nie jej vlastnú mierku alebo rotáciu. Poradie násobenia Kompozitu zostáva rovnaké.

### Cvičenia 7 a 8

- Normálová matica, renormalizácia vo fragmente a homogénne súradnice musia naďalej správne fungovať aj pri testovacom `w = 5`.
- Predvolený správny režim normál, shininess 1 a orezanie odvráteného odlesku zachovať.
- `DrawableObject` pripojí textúry konkrétneho objektu a obnoví predchádzajúce väzby aj aktívnu jednotku.
- F3 používa jednotky 0 a 1, detail opakuje osemkrát a mieša ho silou 0.4. F4 správne prepína obrázky dvoch objektov na jednotke 0.
- F5 používa šesť pôvodných JPG z učiteľovho `skybox.zip`, F6 samostatnú panorámu. Nezamieňať poradie stien ani ich orientáciu.
- `SkyRenderer` zostáva samostatný priechod pred objektmi. Bežný `DrawableObject::draw()` povinne posiela `modelMatrix`, ktorú sky shadery nepotrebujú; nepripájať ich automaticky cez túto cestu.
- Obloha ignoruje posun kamery, reaguje na rotáciu a obnovuje depth mask, depth funkciu, culling, program, VAO aj väzby textúr.

## 9. Ďalšie úpravy po jadre

### Materiál pri kreslení

`Material` už existuje; aktuálne sa aplikuje hlavne pri príprave nočných programov. Ďalší krok môže priradiť materiál objektu a aplikovať ho pred draw, podobne ako u kamaráta.

Najprv preveriť uniformy. Dnešný `Material::apply()` vyžaduje štyri konkrétne mená a `setUniform()` pri chýbajúcom uniforme ukončuje aplikáciu. Nevolať ho naslepo na skybox, jednoduchý textúrovací shader ani všetky osvetľovacie programy. Existujúce `TextureBinding` má zostať funkčné bez druhého konkurenčného nastavovania textúr.

### Explicitný formát vrcholov

Neskôr zaviesť pomenovaný opis atribútov. Počet floatov sám neurčuje ich význam:

| Formát | Príklad |
| --- | --- |
| XYZ + normal, 6 hodnôt | Strom, krík, guľa; login používa normály aj ako farby |
| XYZ + RGB, 6 hodnôt | Farebný trojuholník |
| XYZ + RGB + normal, 9 hodnôt | Planéty a lode |
| XYZ + normal + RGB, 9 hodnôt | Hodiny, farba na location 2 |
| XYZ + normal + UV, 8 hodnôt | Rovina a dvere |

Súčasný alias normál medzi location 1 a 2 pri šiestich hodnotách má význam pre staršie shadery. Explicitné rozhranie musí zachovať správne vstupy každého programu. Geometria oblohy má dnes vlastný upload; netreba ju počas prvých etáp násilne zjednocovať.

### Uniformy, transformácie a zostavenie

- Spoločná kontrola a prípadná cache uniformov môže byť ďalšia malá etapa. Zachovať aj celočíselný setter samplerov a správnu aktiváciu programu pri Observer callbacku.
- `Transformation` kombinuje rozhranie a stav; `Rotation` má vlastný uhol. API upratovať až po bezpečnom presune obsahu, bez zmeny násobenia matíc.
- Najväčší prvý prínos pre inkrementálny build je odstránenie geometrických includes z aplikácie.
- Používateľ pôvodne nechcel osobitný `StbImage.cpp`. Ak neskôr bude vhodné oddeliť implementáciu stb od aplikácie, použiť už existujúci `Texture.cpp` a zachovať jedinú definíciu `STB_IMAGE_IMPLEMENTATION`.
- Existujúce `LNK4098` riešiť až po preverení runtime knižníc; nepotláčať ho naslepo cez `NODEFAULTLIB`.
- CMake migrácia, ECS, nové importéry, fyzika, automatické tiene ani ďalšie knižnice nie sú súčasťou tohto plánu.

## 10. Overenie každého kroku

Najnovšie existujúce kontroly sú mimo repozitára:

```text
C:\Users\ASUS\.codex\build-reviews\zpg-teacher-skybox-20261107\verify_teacher_skybox.py
C:\Users\ASUS\.codex\build-reviews\zpg-exercise8-20261107\verify_exercise8.py
```

Počas dokončovania cvičenia 8 prešlo 66 scenárov vrátane všetkých režimov, textúr, oblohy, resize a letu kamery. Po zapojení učiteľových obrázkov zostalo 52 neovplyvnených renderov pixelovo rovnakých. Overilo sa aj uvoľnenie prostriedkov pri chýbajúcom obrázku a cubemap ploche.

Pred refactoringom vytvoriť novú referenčnú etapu zo súčasných zdrojov. Existujúce `verified`, starý `baseline` ani historické snímky neprepisovať. Helper najprv prečítať; podľa potreby pridať parameter pre nový výstupný priečinok.

Praktické minimum pre etapu:

1. Debug/x64 build aktuálnych zdrojov.
2. Rovnaké snímky dotknutých scén pri pevnom čase, RNG a vstupoch; potom primeraná regresná sada.
3. Platné transformácie, riadený objekt, svetlá a Observer aj po prepnutí.
4. Zánik programov, textúr, VAO a VBO pred OpenGL kontextom aj pri chybe načítania.
5. Žiadna zmena odovzdávacích súborov v `Tasks`.

Helpery používajú pevný čas a semeno 742. Teraz textovo upravujú `Application.cpp` a zbierajú `.cpp` z koreňa `project`. Po presune lesa/animácií alebo po vytvorení podadresárov ich treba prispôsobiť novým miestam. Testovací nástroj nemá určovať finálnu produkčnú architektúru.

Niektoré kontroly predpokladajú počet programov alebo poradie pridelenia texture ID. Pri oprávnenom presune inicializácie upraviť identifikáciu zdrojov podľa ich typu, rozmerov alebo rozhrania. Indexy samplerov 0/1/2 majú funkčný význam; číselné GPU ID sa naopak nemá považovať za stabilné API.

## 11. Hranice a spôsob práce

- Refactorovať aktívny `project`. `Tasks`, staršie ZIP-y, modelové atribúcie a odovzdávacie snímky ponechať nezmenené.
- Zachovať všetky aktuálne necommitnuté a nové súbory. Nepoužívať reset/checkout/clean na návrat k starému handoffu.
- Nevytvárať nové GPU modely pre každú inštanciu stromu, gule alebo kríka.
- Nepoužívať automaticky `shared_ptr` na všetko. Vlastník a požičaná referencia zvyčajne stačia.
- Predvolené presúvanie tried vlastniacich GL ID nie je bezpečné. Prvé etapy vystačia bez novej move podpory `Model` a `ShaderProgram`.
- Nerobiť naraz presun scén, výmenu formátu vrcholov, zmenu shaderov a úpravu vzhľadu.
- Pri odovzdaní kroku uviesť, čo sa presunulo, kto údaje vlastní a ako sa overilo správanie.
- Commit ani push bez výslovnej požiadavky.

## 12. Hotový text pre ďalší chat

> Prečítaj aktuálny REFACTORING_PLAN.md a kód v C:\Users\ASUS\source\repos\ZPG. Chcem postupne implementovať tento refactoring, hlavne skrátiť Application.cpp a oddeliť veľké modelové hlavičky od jeho prekladu. Najprv over Git stav a build súčasného pracovného stromu; obsahuje aj necommitnuté dokončenie cvičenia 8. Inšpiruj sa princípmi SceneFactory a vlastníckej Scene z Adamekka/zpg, ako sú rozobraté v pláne. Zachovaj naše zdieľané GPU modely, spoločnú kameru, Observer, aktuálny let podľa pohľadu, ovládanie, vzhľad a všetky režimy 0-9 a F1-F6. Light, Material, InputController, ClockAnimation, Texture, TextureScenes a SkyRenderer už existujú. Najprv implementuj jeden malý overiteľný krok, napríklad SolarAnimation. Ďalej postupuj cez RenderResources, stabilný SceneContent a malý builder k presunu prípravy scén do tematických súborov. Na záver zjednoť výber scény, ovládaného objektu, svetla a oblohy do tabuľky. Každý krok zostav a primerane over podľa aktuálnych helperov, vysvetli zmeny a aktualizuj záznam postupu. Nemeň odovzdávacie súbory v Tasks a nerob commit ani push bez požiadavky. Po načítaní kontextu začni prvým konkrétnym krokom, nezostávaj iba pri ďalšom návrhu plánu.
