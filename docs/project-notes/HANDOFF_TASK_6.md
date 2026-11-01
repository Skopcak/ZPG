# Pokračovanie projektu ZPG v novom chate – Úloha 6

Stav overený: **24. 10. 2026**. Autor projektu: **Martin Jarabica, jar0193**.
Tento súhrn bol pripravený s pomocou AI podľa aktuálneho kódu, histórie Git a zadania používateľa.

## 1. Čo teraz riešime a ako spolupracovať

Dokončujeme **Cvičenie / Úlohu 6: osvetľovacie modely a osvetlená slnečná sústava**, podľa LMS aj Kelvinu. Úlohy 1 až 5 majú vlastné staršie poznámky; netreba začínať projekt odznova.

- Komunikuj po slovensky. Používateľ je začiatočník a chce sa učiť postupne.
- Vysvetli účel úpravy, presný súbor a miesto vloženia. Dávaj malé kroky.
- Bežne používateľa navádzaj, aby zmeny písal sám. Keď výslovne povie „sprav to“ alebo „oprav mi súbor“, môžeš ich priamo vykonať.
- Nepridávaj zbytočné triedy, súbory, ovládanie alebo rozsiahly refaktoring.
- Používateľ chce **pôvodné farby modelov Slnka, Zeme a Mesiaca**. Osvetlenie sa má počítať nad nimi.
- Štyri osvetľovacie modely už existujú. Na Slnko sa používa existujúci `ambientProgram`; nový `sun.frag` netreba.
- Osvetlenie testovacích gúľ sa vyberá **natvrdo v kóde**, potom sa aplikácia znova spustí. Používateľ si vybral tento postup; nepridávaj automaticky prepínanie cez F1 až F4.
- Testovacie gule sú v rovine XY pred kamerou. Používateľ chce toto rozloženie, nepridávaj svojvoľne inú polohu kamery.
- Učiteľove modely v hlavičkách neupravuj. Zdrojový kód a jeho komentáre píš po anglicky.
- Commit vytváraj na požiadanie. Posledný commit už bol vytvorený; tento handoff súbor zatiaľ commitnutý nie je.

## 2. Kde je skutočný projekt

**Repozitár:** `C:\Users\ASUS\source\repos\ZPG`

**Aktívna aplikácia:** `C:\Users\ASUS\source\repos\ZPG\project`

**Visual Studio projekt:** `C:\Users\ASUS\source\repos\ZPG\project\project.vcxproj`

V kontexte chatu sa môže objaviť pracovný priečinok `C:\mnt\c\Users\ASUS\Documents\ChatGPT\ZPG`. Pri doterajšej práci to nebola platná cesta k repozitáru. Použi skutočnú cestu vyššie a najprv over aktuálny stav.

Pracujeme vo Windows, v PowerShelli. Aktívna konfigurácia je **Debug / x64**, C++20. Aplikácia žiada OpenGL 4.3, naše GLSL shadery používajú `#version 410 core`.

Relatívne cesty k shaderom sa vyhodnocujú voči pracovnému priečinku procesu. Pri spustení má byť dostupný obsah priečinka `project`.

## 3. Stav Git a staršej dokumentácie

Posledné relevantné commity:

| Commit | Obsah |
|---|---|
| `c4f272b` | `Add lighting test scene and preserve planet colors` – štyri testovacie gule, priradenie osvetlenia planétam, pôvodné RGB farby a posielanie polohy svetla. |
| `6db5087` | `Add ambient, Lambert, Phong and Blinn lighting models` – základ štyroch osvetľovacích modelov. |
| `8562fed` | `Complete camera and clock scenes with task 5 notes` – dokončenie predchádzajúcej úlohy a jej poznámok. |

Pri kontrole pred vytvorením tohto súboru boli zdrojové súbory commitnuté. Mimo commitu zostalo samostatné premenovanie obrázka z úlohy 5:

```text
 D Tasks/5.task/jar0193_5.png
?? Tasks/5.task/jar0193_3.png
```

Ide o rovnaký obrázok podľa Git hashov. Nemiešaj túto zmenu automaticky do commitu úlohy 6 a nevracaj ju používateľovi späť.

Staršie poznámky sú v `docs/project-notes`: `TASK_1.md` až `TASK_5.md` a spoločný `BACKLOG.md`. Podrobné študijné poznámky `TASK_6.md` zatiaľ vytvorené nie sú. Tento súbor slúži na odovzdanie kontextu, nie ako odovzdávací súhrn splnenia zadania.

## 4. Čo požaduje aktuálne zadanie

### Kelvin

1. Štyri shader programy: iba ambient, Lambertov diffuse, Phong a Blinn-Phong.
2. Slnečná sústava:
   - Slnko s konštantným shaderom.
   - Ostatné planéty s Phongovým alebo Blinnovým osvetlením.
   - Ďalší zdarma dostupný model, napríklad raketa, pridaný do sústavy.
3. Štyri symetricky rozmiestnené gule s bodovým svetlom uprostred; overiť všetky osvetľovacie modely.
   - Overiť viewport a projekciu pri zmene veľkosti okna, najmä pri úzkom a vysokom okne. Gule sa nemajú naťahovať na elipsoidy.
4. Podľa potreby vhodne upraviť kód.

Odovzdanie: vlastné zdroje, shadery a modely bez knižníc, screenshoty `jar0193_1.png` až `jar0193_X.png` s prípadnými písmenami, stručný súhrn a ukážka osvetlených planét na spoločnom Discorde. Nahratie videa používateľom ešte nie je potvrdené.

### Doplnenie z LMS

- Zapnúť z-buffer – už máme `glEnable(GL_DEPTH_TEST)`.
- Použiť modelovú, pohľadovú a projekčnú maticu, svetovú pozíciu a normálu.
- Používame aj normálovú maticu `transpose(inverse(modelMatrix))`.
- Slnko, Zem a Mesiac sa majú pohybovať; ich transformácie už existujú.
- Zatiaľ sa nepožadujú vrhané tiene medzi planétami.
- Vyskúšať FOV **30°, 45°, 60°, 90°, 120°, 150° a 170°** a vysvetliť jeho vplyv.
- LMS obsahuje vetu „Světlo v [0,0,0], čtyři kulové plochy souměrně rozmístěny a kamera nad objekty.“ Aktuálna implementácia používa na želanie používateľa čelný pohľad na rovinu XY. Túto odlišnosť od formulácie LMS nezamlčať, ale ani bez dohody prerábať celú kameru. Kelvin vyžaduje symetrické gule a svetlo uprostred, rovinu výslovne neurčuje.

## 5. Čo už je spravené

- [x] Štyri osvetľovacie modely sú implementované a ich programy sa kompilujú a linkujú.
- [x] Kamera poskytuje pohľadovú a perspektívnu projekčnú maticu aj polohu pre zrkadlovú zložku.
- [x] Pri zmene framebufferu sa nastavuje viewport; pomer strán projekcie sa priebežne aktualizuje.
- [x] Scéna so štyrmi guľami a bodovým svetlom v počiatku je na klávese **7**.
- [x] Slnko používa ambientný program a zachováva pôvodné farby.
- [x] Zem používa Phongov program, Mesiac Blinnov program.
- [x] Pôvodné RGB farby Zeme a Mesiaca sa prenášajú do fragment shaderov.
- [x] Poloha bodového svetla v slnečnej sústave vychádza z aktuálneho posunu Slnka.
- [x] Zem obieha Slnko, Mesiac Zem, všetky tri telesá sa otáčajú aj okolo vlastnej osi.
- [x] Testovacie gule a farebné planéty fungujú s rozdielnym počtom hodnôt na vrchol.
- [x] `DrawableObject` a `Scene` majú metódu `setShaderProgram`.
- [x] Aktuálne zmeny aplikácie sú uložené v commite `c4f272b`.

Používateľ pred poslednou úpravou farieb potvrdil fungujúce osvetlenie a skúšanie modelov osvetlenia. Posledný prenos pôvodných RGB bol overený aj na GPU. Celkový vizuálny výsledok slnečnej sústavy po tejto poslednej úprave ešte používateľ výslovne nepotvrdil.

## 6. Čo ešte zostáva

### Najbližší vývojový krok: ďalší model do slnečnej sústavy

- [ ] Vybrať zdarma dostupný model, napríklad raketu. Zaznamenať zdroj a podmienky použitia.
- [ ] Importovať ho do Blenderu a pripraviť export do používaného formátu, ak bude treba.
- [ ] Pre farebný model je vhodné pole deviatich hodnôt: `x, y, z, r, g, b, nx, ny, nz`.
- [ ] Pridať model do `Models`, vytvoriť jeho `Model` a `DrawableObject` a vložiť ho do `solarScene`.
- [ ] Nastaviť primeranú polohu a mierku cez existujúce transformácie. Pri osvetlení použiť existujúci Phong alebo Blinn.

Projekt zatiaľ načítava modely z polí v hlavičkách. Nemá všeobecný loader OBJ/glTF. Namiesto nevyžiadaného pridávania celého loadera využi doterajší Blender export, pokiaľ sa s používateľom nedohodne iný postup.

### Overenie a odovzdanie

- [ ] Skontrolovať celkový vzhľad planét po obnovení pôvodných farieb.
- [ ] Dokončiť a zaznamenať porovnanie všetkých štyroch osvetľovacích modelov.
- [ ] Prakticky overiť úzke a vysoké okno. Kód projekcie existuje, ale výsledok tohto konkrétneho testu používateľ zatiaľ nepotvrdil.
- [ ] Vyskúšať požadované FOV a stručne vysvetliť výsledok. Aktuálna hodnota je 60°.
- [ ] Pripraviť screenshoty: jednotlivé osvetľovacie modely, sústava s ďalším modelom a dôkaz správneho pomeru strán pri resize. Nie je nutné vyrábať rovnaké obrázky opakovane, ak jeden zrozumiteľne dokumentuje viac bodov.
- [ ] Nahrať video pohybu a osvetlenia planét; používateľ ho má zverejniť na Discorde.
- [ ] Napísať krátky odovzdávací súhrn podľa bodov zadania, spravidla 1–2 vety na bod.
- [ ] Pripraviť odovzdávací priečinok úlohy 6 so zdrojmi, shadermi a modelmi bez knižníc a build výstupov.
- [ ] Pri príprave odovzdania skontrolovať požiadavky na angličtinu v zdrojoch, autorstvo/login a označenie AI pomoci podľa pokynov predmetu. Doteraz sa nerobilo plošné dopĺňanie hlavičiek.
- [ ] Ak používateľ požiada, doplniť študijné poznámky `TASK_6.md` v štýle starších úloh a rozšíriť `BACKLOG.md`.
- [ ] Ďalší commit a push vykonať podľa následnej požiadavky používateľa.

Úlohu zatiaľ neoznačovať za úplne dokončenú: ďalší model do sústavy chýba a odovzdanie nie je pripravené.

## 7. Ovládanie a scény

| Kláves | Scéna |
|---|---|
| 1 | Vlastný login JAR0193 |
| 2 | Jedna testovacia guľa |
| 3 | Trojuholník a transformácie |
| 4 | Les: 74 stromov, 25 kríkov, zemina a dvoje hodiny |
| 5 | Slnečná sústava |
| 6 | Samostatné hodiny |
| 7 | Štyri gule na testovanie osvetlenia |

WASD pohybuje kamerou. Držanie pravého tlačidla myši umožňuje rozhlížanie; kurzor sa nezamyká. Šípky posúvajú vybraný objekt, Q/E ho otáčajú a Z/X menia jeho mierku. Escape ukončuje aplikáciu.

Kamera je spoločná pre scény a pri prepnutí sa automaticky neresetuje. Ak objekty nie sú v zábere po pohybe kamerou, nemusí ísť o chybu shaderu.

Štyri gule majú stredy `(0, 0.8, 0)`, `(-0.8, 0, 0)`, `(0.8, 0, 0)` a `(0, -0.8, 0)`, mierku `0.35` a svetlo v `(0, 0, 0)`.

Osvetlenie celej testovacej štvorice sa vyberá týmto riadkom v `Application::run()`:

```cpp
lightingSpheres.emplace_back(sphereModel, blinnProgram);
```

Program môže byť `ambientProgram`, `lambertProgram`, `phongProgram` alebo `blinnProgram`. Samostatný `sphereObject` patrí do scény 2; jeho zmena neprepína shader štvorice v scéne 7.

## 8. Architektúra, ktorú netreba vytvárať znova

| Trieda | Zodpovednosť |
|---|---|
| `Application` | Inicializácia GLFW/GLAD, vytvorenie zdrojov a scén, ovládanie, aktualizácia a renderovacia slučka. |
| `Camera` | Poloha a smer kamery, `glm::lookAt`, perspektíva a aktualizácia pomeru strán. |
| `Shader` | Načítanie a kompilácia jedného shaderu, kontrola a výpis chýb. |
| `ShaderProgram` | Linkovanie programu, aktivácia a preťažené posielanie uniformov. |
| `Model` | VBO, VAO, počet vrcholov a `glDrawArrays(GL_TRIANGLES, ...)`. |
| `DrawableObject` | Odkaz na model, požičaný ukazovateľ na shader program a transformácie konkrétneho objektu. |
| `Scene` | Zoznam požičaných ukazovateľov na objekty a ich vykreslenie. |
| `Transformation` | Spoločný základ transformácií a pôvodný základný stav objektu. |
| `Translation`, `Rotation`, `Scale` | Jednotlivé transformácie, ktoré dedia z `Transformation`. |
| `CompositeTransformation` | Kompozit transformácií, násobenie matíc uložených detí v poradí pridania. |

`DrawableObject::draw()` násobí základnú transformáciu objektu s prípadnou priradenou transformáciou, zapne jeho program, nastaví `modelMatrix` a zavolá `model.draw()`.

Modely, programy a priradené transformácie musia žiť dlhšie ako objekty, ktoré si ich požičiavajú. `Scene` objekty nevlastní. Pri ukladaní objektov do vektorov sa používa `reserve`, aby neskoršie pridávanie nezneplatnilo uložené adresy.

Väčšina zdrojov vzniká lokálne v `Application::run()`. Zaniknú pred zničením okna a OpenGL kontextu. `Model` a `ShaderProgram` sa nekopírujú; vlastnené GPU objekty uvoľňujú v deštruktoroch.

`ShaderProgram::setUniform` má preťaženia pre `float`, tri `float` hodnoty a `glm::mat4`. Kontroluje `glGetUniformLocation == -1` a v takom prípade ukončí program. Neposielaj uniformy do programov, ktoré ich nemajú alebo ich linker odstránil.

Observer sa v predchádzajúcej úlohe nakoniec nepoužil. Matice kamery sa posielajú priamo z `Application`; nezavádzaj ho spätne bez dôvodu. V `Camera` zostal starší ukazovateľ na shader, ale nie je to implementácia Observera.

## 9. Formáty modelov a atribúty – dôležité

| Model | Hodnoty na vrchol v pôvodnom poli |
|---|---|
| Testovacia `sphere` | 6: poloha XYZ + normála XYZ |
| `sun`, `earth`, `moon` | 9: poloha XYZ + RGB + normála XYZ |
| `clockModel` | 9: poloha XYZ + normála XYZ + RGB |
| Trojuholník | 6: poloha XYZ + RGB |

**Nie každá deväťhodnotová hlavička má rovnaké poradie. Hodiny majú farby až v tretej trojici.** Ich existujúci shader sa preto nemá slepo nahradiť planetárnym osvetlením.

Aktuálny `Model.cpp` nastavuje:

- `location 0`: prvá trojica.
- `location 1`: druhá trojica.
- `location 2`: pri deviatich hodnotách tretia trojica, pri šiestich hodnotách znovu druhá trojica.

Pri guli sa tak jej normála sprístupní na vstupe 2 bez zmeny pôvodného poľa. Pri farebných planétach je na vstupe 1 RGB a na vstupe 2 normála. Pri modeloch, ktoré normály vôbec nemajú, samotné nastavenie atribútu normály nevytvorí.

## 10. Aktuálne osvetľovacie programy

| Program | Vertex shader | Fragment shader |
|---|---|---|
| `ambientProgram` | `shaders/basic.vert` | `ambient.frag` |
| `lambertProgram` | `phong.vert` | `lambert.frag` |
| `phongProgram` | `phong.vert` | `phong.frag` |
| `blinnProgram` | `phong.vert` | `blinn.frag` |

V aplikácii existujú aj staršie programy pre základné farby, žltú farbu, les a hodiny. Požiadavka štyroch osvetľovacích modelov neznamená, že celý projekt má presne štyri objekty `ShaderProgram`.

`phong.vert` má vstupy poloha 0, farba 1 a normála 2. Prenáša `vertexColor`, `worldPosition` typu `vec4` a `worldNormal` typu `vec3`. Typy vstupov fragment shaderov musia zostať kompatibilné.

```glsl
vertexColor = color;
worldPosition = modelMatrix * vec4(position, 1.0);
mat3 normalMatrix = mat3(transpose(inverse(modelMatrix)));
worldNormal = normalize(normalMatrix * normal);
gl_Position = projectionMatrix * viewMatrix * worldPosition;
```

`phong.frag` počíta zrkadlovú zložku cez `reflect(-lightDirection, normal)`. `blinn.frag` používa `normalize(lightDirection + viewDirection)`. Oba majú exponent 32 a silu zrkadlovej zložky 0.5. Lambert má iba diffuse; ambientný program iba ambientnú zložku.

### Ako sa zachovávajú pôvodné farby

V ambientnom, Phongovom a Blinnovom programe existuje **float uniform `useVertexColor`**:

- V slnečnej sústave má hodnotu `1.0f` a používa RGB z modelov.
- V ostatných scénach má `0.0f` a testovacie gule používajú pôvodnú pevnú modrastú farbu.

Používateľ nechcel náhodné ani nové náhradné farby planét. Hodnoty sa čítajú priamo z hlavičiek; nemenia sa dáta modelov. Prepínač je potrebný preto, že testovacia guľa skutočné RGB neobsahuje.

Vo Phongovom a Blinnovom fragmente sa pri zapnutých farbách modelu používa:

```glsl
surfaceColor = vertexColor;
ambientColor *= surfaceColor;
```

Diffuse sa násobí `surfaceColor`. Výsledný jas sa mení podľa osvetlenia a môže pribudnúť biely odlesk. Zachovanie pôvodných farieb neznamená, že osvetlená planéta bude mať na každom mieste rovnaký jas ako pri starom neosvetlenom zobrazení.

Slnko používa biele ambientné svetlo intenzity 1, takže jeho farby zostávajú rovnomerne viditeľné. Používa sa už existujúci ambientný program s `basic.vert`, ktorý posiela farby; `phong.vert` pôvodne tento výstup nemal. Táto nekompatibilita sa už opravila.

### Svetlo a transformácie sústavy

`lightPosition` je globálny uniform v Lambertovom, Phongovom a Blinnovom fragmente. Neumiestňovať deklaráciu `uniform` dovnútra `main()`.

V scéne 7 je svetlo v počiatku. V slnečnej sústave sa poloha berie zo `solarSunObject.getTransformation()` cez `offsetX`, `offsetY`, `offsetZ`. Existujúca slučka `lightingPrograms` posiela túto pozíciu všetkým trom programom. Tento blok už raz pri ručnom kopírovaní vypadol; bol obnovený.

Slnko má vlastnú rotáciu okolo Y. Zem a Mesiac majú vlastnú rotáciu okolo X; ich obežné pohyby sú okolo Z, teda viditeľné ako kružnice v rovine obrazovky. Toto rozloženie si používateľ výslovne zvolil. Transformácia Mesiaca obsahuje obežnú transformáciu Zeme, takže nasleduje Zem a zároveň obieha okolo nej. Mierka a vlastná rotácia Slnka sa neprenášajú do veľkostí ani dráh planét.

## 11. Čo už bolo overené a ako overovať ďalšie zmeny

Na aktuálnom kóde pred commitom `c4f272b` prešli:

- `git diff --check` pre súbory v commite.
- Zostavenie Debug / x64 cez MSBuild.
- Kompilácia a linkovanie všetkých deviatich kombinácií shaderov aplikácie v skutočnom OpenGL 4.3 kontexte.
- Kontrola používaných uniformov a vstupov farby 1 / normály 2.
- Vykreslenie vzorky pôvodnej RGB farby Zeme cez Phong a Mesiaca cez Blinn do framebufferu a kontrola výsledku na GPU.
- Prepnutie `useVertexColor` späť na pevnú farbu testovacích gúľ a znovu na RGB modelu.
- Zachovanie pôvodnej farby Slnka v ambientnom programe.

GPU kontrola vzoriek farieb nie je screenshot celého pohybujúceho sa solárneho systému. Praktický test úzkeho okna a FOV tým nebol nahradený.

Projekt nemá nakonfigurovaný samostatný formatter, linter ani automatickú testovaciu sadu. Nepridávaj nové závislosti iba kvôli commitu.

Existujúca pomocná GPU kontrola je **mimo repozitára**:

```powershell
& 'C:\Users\ASUS\.cache\codex-runtimes\codex-primary-runtime\dependencies\python\python.exe' 'C:\Users\ASUS\.codex\build-reviews\zpg-planet-colors-20261023\check_shaders.py'
```

Používa už dostupnú GLFW DLL a skrytý OpenGL kontext; nevytvára súbory v projekte. Cesty platili pri tomto handoffe, v novom prostredí over dostupnosť.

Build možno zopakovať týmto príkazom s výstupmi mimo repozitára:

```powershell
Set-Location 'C:\Users\ASUS\source\repos\ZPG'
& 'C:\Program Files\Microsoft Visual Studio\18\Insiders\MSBuild\Current\Bin\MSBuild.exe' 'project\project.vcxproj' /t:Build /p:Configuration=Debug /p:Platform=x64 '/p:OutDir=C:\Users\ASUS\.codex\build-reviews\zpg-planet-colors-20261023\bin\' '/p:IntDir=C:\Users\ASUS\.codex\build-reviews\zpg-planet-colors-20261023\obj\' /m /nologo /v:minimal
```

Samotný C++ build GLSL nekompiluje; shadery sa kompilujú až cez OpenGL. Pri skoršom úplnom linkovaní sa objavovalo existujúce upozornenie `LNK4098` na kombináciu CRT knižníc. Posledné inkrementálne zostavenie pred commitom prešlo bez chýb aj upozornení.

## 12. Odporúčaný začiatok nového chatu

Najprv over `git status` a aktuálne súbory, pretože tento dokument zachytáva stav k uvedenému dátumu. Potom postupne pokračuj **bodom 2c – pridaním ďalšieho zdarma dostupného modelu do slnečnej sústavy**. Používateľa naveď na výber a prípravu jedného modelu, až potom na jeho zapojenie do existujúcej scény.

Nadväzujúca správa, ktorú môže používateľ poslať:

> Pokračujeme v mojom C++ OpenGL projekte ZPG, úloha 6 podľa LMS a Kelvinu. Prečítaj si `C:\Users\ASUS\source\repos\ZPG\docs\project-notes\HANDOFF_TASK_6.md` a aktuálny kód. Chcem pokračovať krok za krokom a vysvetľovať si zmeny. Osvetlenie a pôvodné farby planét už máme spravené. Najbližšie potrebujem pridať ďalší zdarma dostupný model do slnečnej sústavy a potom dokončiť overenie a odovzdanie podľa zoznamu v handoffe.
