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

## Spoločné overenie

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
