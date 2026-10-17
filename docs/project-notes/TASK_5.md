# Úloha 5 - Kamera, prechádzanie lesa a hodiny jedným draw callom

Autor projektu: Martin Jarabica, jar0193

Obdobie: 15. až 17. október 2026.

Podklady: zadanie z LMS a Kelvinu, aktuálne zdrojové súbory, história Git a konzultácie pri riešení.

Poznámky pripravené s pomocou AI. Ukážky sú vybrané fragmenty projektu; niektoré vynechávajú okolitý kód a nie sú samostatným programom.

## 1. Týždenný prehľad

Cieľom bolo pridať kameru, aby sme sa mohli pohybovať v trojrozmernej scéne a rozhlížať sa myšou. Les sme rozšírili na 74 stromov, doplnili zeminu, zelené kríky a dvoje hodiny. Celé hodiny sa vykresľujú jedným draw callom na jednu inštanciu, pričom jednotlivé ručičky otáča vertex shader podľa aktuálneho času.

## 2. Čo sme spravili

- Pridali sme triedu `Camera` s polohou, smerom pohľadu, pohľadovou a perspektívnou projekčnou maticou.
- Vo vertex shaderoch používame poradie `projectionMatrix * viewMatrix * modelMatrix * position`.
- Pohyb kamery ovládame cez WASD. Rýchlosť násobíme časom medzi snímkami.
- Pri držaní pravého tlačidla myši meníme smer pohľadu. Kurzor nezamykáme ani nevraciame do stredu okna.
- Obmedzili sme zvislý uhol kamery a priebežne upravujeme pomer strán projekcie podľa framebufferu.
- Les obsahuje 74 stromov s náhodnou polohou, otočením a mierkou, 25 kríkov a zeminu.
- V `tree.vert` rozlišujeme drevo a listy pomocou `gl_VertexID` a upravujeme intenzitu farby podľa normály.
- Pridali sme `bush.vert`, ktorý nahrádza dúhové farby kríkov zelenou farbou s rozdielnou intenzitou.
- Načítali sme učiteľov model hodiniek z `Models/clock.h` bez úpravy jeho dát.
- Pridali sme `clock.vert`, ktorý podľa pôvodnej farby vrcholu vyberá transformáciu ručičky.
- Ručičky nastavujeme podľa miestneho systémového času vrátane zlomku sekundy.
- Pridali sme samostatnú scénu s hodinami na klávese 6 a dve inštancie hodín do lesa.
- Kamerové matice posielame priamo z `Application`. Návrhový vzor Observer sme vo výslednej verzii nepoužili; v tomto zadaní je voliteľný.
- Pripravili sme odovzdávací priečinok `Tasks/5.task` a tieto poznámky k úlohe.

## 3. Ako sme to spravili (Ukážky kódu & Vysvetlenie)

### 3.1 Na čo slúžia tri matice

V predchádzajúcej úlohe sme menili predovšetkým samotný objekt. Teraz rozlišujeme umiestnenie objektu, pohľad kamery a spôsob premietania na obrazovku.

| Matica | Význam |
|---|---|
| `modelMatrix` | Umiestni vrcholy modelu do sveta: posunie, otočí a zmení jeho mierku. |
| `viewMatrix` | Vyjadrí svet vzhľadom na polohu a natočenie kamery. |
| `projectionMatrix` | Pripraví perspektívne premietnutie vrátane zorného poľa, pomeru strán a rozsahu hĺbky. |

Vertex shader používa:

```glsl
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix
        * vec4(position, 1.0);
}
```

Pri tomto zápise sa matice na vrchol uplatnia sprava doľava. Najprv sa vrchol umiestni do sveta, potom do priestoru kamery a nakoniec do clip priestoru. Následné delenie súradníc hodnotou `w` vykoná grafická pipeline, čím vznikne perspektívne zmenšovanie vzdialených objektov.

Počas skúšania sme opravovali aj poradie matíc a názov uniformy. `projectionMatrix` a `ProjectionMatrix` sú dva odlišné názvy; C++ musí používať presný názov zo shaderu.

### 3.2 Trieda Camera a pohľadová matica

Kamera si uchováva tieto základné údaje:

```cpp
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

glm::vec3 eye;
glm::vec3 target;
glm::vec3 up;

float alpha;
float fi;
```

`eye` je poloha kamery. Premenná `target` v našom projekte obsahuje smer pohľadu, preto cieľový bod pre `lookAt` získame súčtom `eye + target`. Vektor `up` určuje, ktorý smer považujeme za horný.

```cpp
eye = glm::vec3(0.0f, 0.0f, 3.0f);
target = glm::vec3(0.0f, 0.0f, -1.0f);
up = glm::vec3(0.0f, 1.0f, 0.0f);

alpha = glm::radians(90.f);
fi = glm::radians(-90.f);
```

Na začiatku je kamera na kladnej osi Z a pozerá smerom k zápornej osi Z. Pohľadovú maticu získame takto:

```cpp
glm::mat4 Camera::getCamera()
{
    viewMatrix = glm::lookAt(eye, eye + target, up);
    return viewMatrix;
}
```

Pohľadová matica nemení pôvodné dáta modelu vo VBO. Pri vykresľovaní mení súradnice tak, ako ich vidí kamera. Pohyb kamery doprava preto spôsobí, že nehybný objekt na obraze ustupuje doľava.

### 3.3 Perspektívna projekcia a zmena veľkosti okna

```cpp
void Camera::updateProjection(int width, int height)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    projectionMatrix = glm::perspective(
        glm::radians(60.0f),
        static_cast<float>(width) / static_cast<float>(height),
        0.1f,
        100.0f
    );
}
```

Prvý parameter je zvislý zorný uhol, druhý je pomer šírky a výšky. Posledné dva parametre určujú blízku a vzdialenú orezávaciu rovinu. Blízku rovinu máme na `0.1f`, nie na nule.

Pred vykreslením zisťujeme skutočnú veľkosť framebufferu:

```cpp
glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
camera.updateProjection(framebufferWidth, framebufferHeight);
```

Callback na zmenu veľkosti zároveň nastavuje `glViewport`. Tým sa pri zmene rozmerov okna upraví kresliaca plocha aj pomer strán projekcie. Pri minimalizácii môže byť rozmer nulový; podmienka v `updateProjection` zabráni deleniu nulou a ponechá poslednú platnú projekciu.

### 3.4 Pohyb cez WASD

```cpp
void Camera::moveForward(float distance)
{
    glm::vec3 forward(target.x, 0.0f, target.z);
    eye += glm::normalize(forward) * distance;
}

void Camera::moveRight(float distance)
{
    glm::vec3 right = glm::normalize(glm::cross(target, up));
    eye += right * distance;
}
```

Pri pohybe dopredu používame iba zložky X a Z. Kamera preto pri pohľade hore nezačne stúpať; jej výška sa nemení. Nejde však o simuláciu chôdze s gravitáciou alebo kolíziami.

`glm::cross(target, up)` vytvorí smer doprava. Normalizácia nastaví dĺžku smerového vektora na 1, takže vzdialenosť pohybu určujeme samostatným parametrom.

Vo vykresľovacej slučke reagujeme na klávesy:

```cpp
if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
{
    camera.moveForward(cameraSpeed * deltaTime);
}

if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
{
    camera.moveForward(-cameraSpeed * deltaTime);
}

if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
{
    camera.moveRight(cameraSpeed * deltaTime);
}

if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
{
    camera.moveRight(-cameraSpeed * deltaTime);
}
```

`deltaTime` je čas medzi dvoma snímkami. Pri `cameraSpeed = 2.0f` chceme prejsť približne dve jednotky za sekundu, bez ohľadu na počet vykreslených snímok. Záporná vzdialenosť obráti smer pohybu.

### 3.5 Otáčanie kamery a funkcia clamp

Pohľad opisujeme dvoma uhlami. `alpha` sa meria od kladnej osi Y a `fi` určuje smer v rovine XZ.

```cpp
void Camera::rotate(float deltaAlpha, float deltaFi)
{
    alpha += deltaAlpha;
    fi += deltaFi;

    alpha = std::clamp(
        alpha,
        glm::radians(1.0f),
        glm::radians(179.0f)
    );

    fi = std::remainder(fi, glm::radians(360.0f));

    target.x = std::sin(alpha) * std::cos(fi);
    target.z = std::sin(alpha) * std::sin(fi);
    target.y = std::cos(alpha);
}
```

`std::clamp` udrží hodnotu medzi dolnou a hornou hranicou. Pri pohľade používame rozsah 1 až 179 stupňov, aby smer pohľadu nebol rovnobežný s vektorom `up`. Taká poloha by spôsobila problém pri zostavovaní súradnicových osí kamery.

Pri `alpha = 90°` sa pozeráme vodorovne. Menší uhol smeruje pohľad hore, väčší dole. `std::remainder` udržiava vodorovný uhol v obmedzenom rozsahu bez obmedzenia počtu otočení.

### 3.6 Pravé tlačidlo myši bez zamknutia kurzora

Polohu myši čítame priamo v slučke cez `glfwGetCursorPos`. Nepoužívame callback na pohyb myši ani `glfwSetWindowUserPointer`.

```cpp
double mouseX;
double mouseY;
glfwGetCursorPos(window, &mouseX, &mouseY);

bool looking =
    glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE &&
    glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

if (looking && wasLooking)
{
    float deltaAlpha =
        static_cast<float>(mouseY - previousMouseY) * mouseSensitivity;

    float deltaFi =
        static_cast<float>(mouseX - previousMouseX) * mouseSensitivity;

    camera.rotate(deltaAlpha, deltaFi);
}

previousMouseX = mouseX;
previousMouseY = mouseY;
wasLooking = looking;
```

Pred slučkou nastavíme predchádzajúcu polohu cez `glfwGetCursorPos`, `wasLooking` na `false` a citlivosť na `0.003f`. Rozdiel aktuálnej a predchádzajúcej polohy určuje, o koľko sa kamera otočí.

Podmienka `looking && wasLooking` zabráni skoku pri prvom stlačení pravého tlačidla alebo po návrate do okna. Kurzor zostáva voľný. Posun myši už vyjadruje rozdiel polôh, preto ho tu ešte nenásobíme časom medzi snímkami.

### 3.7 Posielanie kamery shader programom

```cpp
colorProgram.setUniform("viewMatrix", camera.getCamera());
colorProgram.setUniform("projectionMatrix", camera.getProjectionMatrix());

yellowProgram.setUniform("viewMatrix", camera.getCamera());
yellowProgram.setUniform("projectionMatrix", camera.getProjectionMatrix());

treeProgram.setUniform("viewMatrix", camera.getCamera());
treeProgram.setUniform("projectionMatrix", camera.getProjectionMatrix());

clockProgram.setUniform("viewMatrix", camera.getCamera());
clockProgram.setUniform("projectionMatrix", camera.getProjectionMatrix());

bushProgram.setUniform("viewMatrix", camera.getCamera());
bushProgram.setUniform("projectionMatrix", camera.getProjectionMatrix());
```

Každý shader program má vlastné hodnoty uniforiem. Nastavenie matice v `colorProgram` preto automaticky nenastaví rovnakú maticu v `treeProgram`. Existujúce preťaženie `setUniform` pre `glm::mat4` skontroluje existenciu uniformy, aktivuje program a odošle 16 hodnôt matice.

`Camera` má zo skoršej ukážky LMS doprednú deklaráciu `class ShaderProgram;` a uložený ukazovateľ `m_shaderProgram`. V aktuálnej implementácii cez tento ukazovateľ shader neaktualizuje. Skutočné odosielanie matíc zabezpečuje vyššie uvedený kód v `Application`; samotný ukazovateľ ešte nie je návrhový vzor Observer.

### 3.8 Rozšírenie lesa a zdieľanie modelov

Les obsahuje 74 objektov stromov a 25 objektov kríkov. Polohy X a Z generujeme v rozsahu -8 až 8, otočenie v rozsahu celej otáčky a mierky stromov v rozsahu 0.25 až 0.45. Kríky používajú mierky 1 až 2, pretože ich pôvodný model má inú veľkosť.

Príklad rozdelenia pre uhol:

```cpp
std::uniform_real_distribution<float> forestAngle(0.0f, 6.283185f);
```

Hodnota približne `6.283185` predstavuje `2 * pi`, teda 360 stupňov vyjadrených v radiánoch. Rozdelenie určuje rozsah; konkrétnu hodnotu získame jeho zavolaním s generátorom náhodných čísel.

Všetky stromy používajú jeden `Model` s vlastným VBO a VAO. Každý `DrawableObject` si naň iba odkazuje a má vlastnú transformáciu. Rovnako zdieľame model kríkov. Zemina je vytvorená z modelu `plain` so zväčšením a posunom na výšku `-1.0f`.

Pred pridávaním stromov a kríkov do scény rezervujeme miesto vo vektoroch. `Scene` si ukladá ukazovatele na objekty; keby sa vektor neskôr premiestnil pri zväčšení kapacity, tieto ukazovatele by prestali platiť.

### 3.9 Farba stromov a zelené kríky

V `tree.vert` rozlišujeme časti konkrétneho učiteľovho modelu podľa poradia vrcholov:

```glsl
if (gl_VertexID > 36443)
    vertexColor = vec3(0.1, 0.6, 0.1);
else
    vertexColor = vec3(0.4, 0.2, 0.05);

float shade = mix(0.3, 1.0, abs(normal.y));
vertexColor *= shade;
```

`gl_VertexID` je identifikátor práve spracúvaného vrcholu. Hranica `36443` funguje pre tento konkrétny model, preto ju nemožno automaticky preniesť na ľubovoľný iný strom.

Výraz s `mix` je matematicky rovnaký ako pôvodné `0.3 + 0.7 * abs(normal.y)`. Pre hodnoty `abs(normal.y)` od 0 do 1 nastaví intenzitu od 0.3 do 1. Napríklad hodnota 0.5 dá intenzitu 0.65.

Kríky dostali vlastný shader `bush.vert`:

```glsl
float shade = mix(0.3, 1.0, abs(normal.y));
vertexColor = vec3(0.08, 0.35, 0.10) * shade;

gl_Position = projectionMatrix * viewMatrix * modelMatrix
    * vec4(position, 1.0);
```

Predtým všeobecný shader používal absolútne hodnoty zložiek normály ako RGB, takže povrch vyzeral dúhovo. Teraz má celý krík zelený základ a normála ovplyvňuje iba intenzitu. Ide o jednoduché zvýraznenie tvaru podľa lokálnej normály; zatiaľ nepočítame skutočné osvetlenie ani tiene.

### 3.10 Formát modelu hodín a atribúty vo VAO

Model hodín má v jednom vrchole deväť hodnôt:

```text
x, y, z, nx, ny, nz, r, g, b
```

Pozor na rozdiel oproti modelom planét z predchádzajúcej úlohy. Tie majú pri rovnakom počte hodnôt poradie poloha, farba, normála. Hodiny majú poradie poloha, normála, farba.

```cpp
const GLsizei clockVertexCount = static_cast<GLsizei>(
    sizeof(clockModel) / (9 * sizeof(float)));

Model clockMesh(clockModel, clockVertexCount, 9);
DrawableObject clockObject(clockMesh, clockProgram);
```

Pole z hlavičky sa volá `clockModel`, preto samotný objekt triedy `Model` nazývame `clockMesh`. Dáta obsahujú 44 826 vrcholov, teda 14 942 trojuholníkov.

Existujúca trieda `Model` nastaví atribút 0 na prvé tri hodnoty, atribút 1 na ďalšie tri a pri deviatich hodnotách aj atribút 2 na posledné tri. Preto `clock.vert` číta farbu takto:

```glsl
layout (location = 0) in vec3 position;
layout (location = 2) in vec3 color;
```

Nesprávna lokácia by spôsobila, že shader číta normálu ako farbu a nerozozná správne jednotlivé ručičky.

### 3.11 Rozlíšenie ručičiek pôvodnou farbou

| Pôvodná farba v modeli | Časť modelu | Použitá matica | Výsledná farba |
|---|---|---|---|
| Modrá `(0, 0, 1)` | Hodinová ručička | `hourMatrix` | Čierna |
| Zelená `(0, 1, 0)` | Minútová ručička | `minuteMatrix` | Čierna |
| Červená `(1, 0, 0)` | Sekundová ručička | `secondMatrix` | Červená |
| Žltá `(1, 1, 0)` | Logo na ciferníku | Bez samostatnej rotácie | Tyrkysová |
| Ostatné farby | Telo, ciferník a čísla | Bez samostatnej rotácie | Pôvodná farba |

Ukážka výberu transformácie vo vertex shaderi:

```glsl
vec4 transformedPosition = vec4(position, 1.0);
vertexColor = color;

if (color == vec3(0.0, 0.0, 1.0))
{
    transformedPosition = hourMatrix * transformedPosition;
    vertexColor = vec3(0.0);
}
else if (color == vec3(0.0, 1.0, 0.0))
{
    transformedPosition = minuteMatrix * transformedPosition;
    vertexColor = vec3(0.0);
}
else if (color == vec3(1.0, 0.0, 0.0))
{
    transformedPosition = secondMatrix * transformedPosition;
    vertexColor = vec3(1.0, 0.0, 0.0);
}
else if (color == vec3(1.0, 1.0, 0.0))
{
    vertexColor = vec3(5.0, 195.0, 222.0) / 255.0;
}

gl_Position = projectionMatrix * viewMatrix * modelMatrix
    * transformedPosition;
```

`color` je pôvodný vstupný atribút, podľa ktorého vyberáme časť. `vertexColor` je výsledná zobrazovaná farba. Preto môžeme modrou identifikovať hodinovú ručičku a napriek tomu ju vykresliť čiernou.

Na skúšku rozlíšenia môžeme v prvých dvoch vetvách dočasne nastaviť `vertexColor = color`. Potom uvidíme modrú hodinovú a zelenú minútovú ručičku. V konečnej verzii ostávajú obe čierne. Pri tomto modeli je presné porovnanie vhodné, lebo identifikačné farby obsahujú priamo hodnoty 0 a 1.

### 3.12 Aktuálny čas a plynulé otáčanie

Pre planéty používame čas behu aplikácie a vlastné rýchlosti. Hodiny majú ukazovať miestny skutočný čas, preto používame `std::chrono::system_clock` a prevod na `std::tm`.

```cpp
const auto now = std::chrono::system_clock::now();
const auto wholeSeconds =
    std::chrono::floor<std::chrono::seconds>(now);

const std::time_t timestamp =
    std::chrono::system_clock::to_time_t(wholeSeconds);

std::tm localTime{};

if (localtime_s(&localTime, &timestamp) != 0)
{
    std::cerr << "Unable to read local time\n";
    std::exit(EXIT_FAILURE);
}

float seconds = static_cast<float>(localTime.tm_sec)
    + std::chrono::duration<float>(now - wholeSeconds).count();

float minutes = static_cast<float>(localTime.tm_min)
    + seconds / 60.0f;

float hours = static_cast<float>(localTime.tm_hour % 12)
    + minutes / 60.0f;
```

Zlomok sekundy zabezpečí plynulý pohyb sekundovej ručičky. Sekundy pripočítame do minút a minúty do hodín, takže ani ostatné ručičky neskáču iba po celých dielikoch. Napríklad pri čase 3:30 má hodinová ručička hodnotu 3.5, teda je medzi trojkou a štvorkou.

Pred slučkou vytvoríme tri objekty `Rotation` s osou Z. V slučke nastavujeme ich uhly a odosielame matice:

```cpp
const float clockwiseTurn = glm::radians(-360.0f);

hourHandRotation.setAngle(clockwiseTurn * (hours / 12.0f));
minuteHandRotation.setAngle(clockwiseTurn * (minutes / 60.0f));
secondHandRotation.setAngle(clockwiseTurn * (seconds / 60.0f));

clockProgram.setUniform("hourMatrix", hourHandRotation.getMatrix());
clockProgram.setUniform("minuteMatrix", minuteHandRotation.getMatrix());
clockProgram.setUniform("secondMatrix", secondHandRotation.getMatrix());
```

Podiel vyjadruje časť celej otáčky. Sekunda 15 znamená štvrtinu otáčky, teda uhol -90 stupňov. Záporné znamienko zabezpečí otáčanie v smere hodinových ručičiek pri pohľade na ciferník. Funkcia `localtime_s` v tomto zápise zodpovedá použitému prostrediu MSVC na Windows.

### 3.13 Prečo stačí jeden draw call na celé hodiny

`Model::draw()` obsahuje jedno volanie:

```cpp
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, vertexCount);
```

Tým odošleme na vykreslenie celý model vrátane tela aj všetkých ručičiek. Vertex shader pre každý vrchol rozhodne, či má použiť hodinovú, minútovú, sekundovú alebo iba spoločnú modelovú transformáciu. Model preto nerozdeľujeme na štyri samostatné objekty a štyri draw cally.

Vlastná rotácia ručičky prebehne v lokálnych súradniciach modelu pred spoločnou `modelMatrix`. Stred ručičiek je v tomto modeli pripravený pre rotáciu okolo lokálnej osi Z. VBO zostáva rovnaké; každý snímok meníme iba uniformné matice.

Samostatná scéna hodín má jeden draw call. Dvoje hodiny v lese znamenajú dva draw cally na hodiny; stromy, kríky a ďalšie objekty majú vlastné volania.

### 3.14 Otočenie modelu a dvoje hodiny v lese

```cpp
Scale clockFacing(1.0f, 1.0f, -1.0f);
clockObject.setTransformation(clockFacing);

DrawableObject forestClock1(clockMesh, clockProgram);
forestClock1.setTransformation(clockFacing);

Transformation& firstClockTransform =
    forestClock1.getTransformation();

firstClockTransform.offsetX = -1.5f;
firstClockTransform.offsetY = -0.25f;
firstClockTransform.offsetZ = 0.0f;
firstClockTransform.scaleFactor = 0.75f;
```

Druhé hodiny používajú polohu `(1.5, -0.25, -3.0)` a rovnakú mierku `0.75`. Obe inštancie zdieľajú `clockMesh`, `clockProgram` a dodatočnú transformáciu `clockFacing`.

Záporná mierka na osi Z je odraz, ktorý obráti smer hĺbky modelu a neprevráti ciferník v osiach X a Y. Absolútne hodnoty mierok sú rovnaké, preto sa nemenia proporcie hodín. Keby sme rôzne zmenili veľkosť osí X a Y, kruhový ciferník by sa zmenil na elipsu.

`DrawableObject` skombinuje vlastnú transformáciu s priradenou transformáciou a výsledok pošle do `modelMatrix`. Výška stredu -0.25 pri polomere približne 0.75 umiestni spodok modelu na úroveň zeme -1.

### 3.15 Rozdelenie zodpovedností a súbory

| Trieda alebo súbor | Úloha v aktuálnej verzii |
|---|---|
| `Application` | Vytvorí prostriedky a scény, spracuje vstupy, čas a odosielanie kamerových matíc. |
| `Camera` | Uchováva polohu, smer a projekciu; počíta pohľadovú maticu. |
| `Scene` | Uchováva odkazy na objekty a pri `draw()` ich postupne vykreslí. |
| `DrawableObject` | Spája model, shader program a transformáciu konkrétnej inštancie. |
| `Model` | Uchováva VBO a VAO, nastavuje formát atribútov a volá `glDrawArrays`. |
| `Shader` a `ShaderProgram` | Kompilujú a linkujú shadery, vyberajú program a nastavujú uniformy. |
| `Transformation`, `Translation`, `Rotation`, `Scale`, `CompositeTransformation` | Vytvárajú a skladajú matice; využívame ich aj pre hodiny. |
| `tree.vert`, `bush.vert`, `clock.vert` | Riešia farby vegetácie alebo transformácie častí hodín. |

Vykreslenie prechádza reťazcom `Application -> Scene -> DrawableObject -> Model -> glDrawArrays`. To, že `Application` vytvára objekty a riadi slučku, je v tejto verzii vedomé rozdelenie úloh.

Nové shadery sú fyzicky v koreňovom priečinku projektu, preto ich otvárame ako `"tree.vert"`, `"bush.vert"` a `"clock.vert"`. Ostatné používajú napríklad cestu `"shaders/basic.frag"`. Ide o relatívne cesty vzhľadom na pracovný adresár procesu, ktorý musí týmto cestám zodpovedať.

V projekte Visual Studia sú `.vert` súbory evidované ako `None`, nie ako `ClCompile`. Shader kompiluje OpenGL počas behu aplikácie, nie C++ kompilátor. Virtuálny filter v Solution Exploreri nemení skutočné umiestnenie súboru.

## 4. Code Review & Poznatky

- Zachovali sme rozdelenie modelu a jeho inštancií. V lese preto nevytvárame nové VBO pre každý strom ani nové dáta modelu pre každé hodiny.
- Dopredná deklarácia umožní v hlavičke použiť ukazovateľ na inú triedu bez načítania celej jej definície. Sama osebe však nezabezpečí komunikáciu medzi kamerou a shaderom.
- Observer sme zvažovali, ale výsledná aplikácia posiela matice priamo shader programom. Takto sa dá dobre sledovať, kedy sa hodnoty nastavujú. Uložený ukazovateľ `m_shaderProgram` v kamere ostáva nevyužitý a v budúcom refaktoringu ho možno odstrániť alebo zmysluplne zapojiť.
- Kamera je zatiaľ spoločná pre všetky scény. Pri prepnutí scény sa jej poloha automaticky neobnovuje.
- Súčasné nezávislé spracovanie WASD znamená, že diagonálny pohyb je rýchlejší než pohyb jediným smerom. Prípadné zjednotenie rýchlosti by vyžadovalo spojiť smery a výsledný vektor normalizovať.
- Uhlový limit chráni `lookAt` aj normalizáciu vodorovného smeru pred krajnými polohami. Kontrola rozmerov chráni projekciu pri minimalizácii okna.
- Farby vrcholov môžu niesť aj informáciu o časti modelu. Pri hodinách ich používame ako identifikátory, pričom výslednú farbu môžeme nastaviť nezávisle.
- Deväť hodnôt na vrchol ešte nehovorí, v akom poradí sú uložené atribúty. Pri hodinách sme museli skontrolovať pôvodný model a farbu čítať z lokácie 2.
- Jednoduché tieňovanie vegetácie používa lokálnu normálu. Zatiaľ nereaguje na skutočnú polohu slnka ani na otočenie povrchu vo svetovom priestore ako plný osvetľovací model.
- `Application::run()` sa postupne zväčšuje. V budúcnosti možno prípravu scén rozdeliť do metód, pričom treba zachovať životnosť zdieľaných modelov, programov a požičaných transformácií.
- Pri porovnávaní so spolužiakovým riešením sme prebrali princíp rozlíšenia častí hodín farbou a výpočtu uhla z času. Konkrétne lokácie atribútov a správa scén sa museli prispôsobiť nášmu projektu.
- Chyba, pri ktorej Visual Studio skúšalo prekladať `bush.vert` ako C++, sa opravila nastavením súboru na `None`. Pôvodné hlavičky modelov zachovávame bez úprav.
- Funkčnosť pohybu, zobrazenia lesa a hodín bola overovaná pri postupnom spúšťaní používateľom. Projekt má zostavenie cez MSBuild; úspešný C++ build sám neoveruje vizuálny výsledok ani kompiláciu GLSL počas behu. Samostatný formatter, linter a automatické testy v repozitári zatiaľ nie sú nakonfigurované.

Odovzdávací súhrn je v `Tasks/5.task/jar0193.md`. Obsahuje stručné vyhodnotenie bodov zadania; tento súbor slúži ako podrobnejšie študijné poznámky k tomu, čo sa v úlohe menilo a prečo.
