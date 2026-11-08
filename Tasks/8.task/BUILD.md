# Zostavenie a obsah odovzdania

Aktívny projekt je `project/project.vcxproj`, riešenie `ZPG.slnx`. Zostavenie: Visual Studio, C++20, x64. Pracovný adresár pri spustení musí byť `project`, pretože shadery a modely používajú relatívne cesty.

Knižnice sa podľa zadania neodovzdávajú. GLFW, GLAD, GLM a `stb_image.h` treba doplniť rovnako ako v pôvodnom projekte; existujúce cesty projektu očakávajú GLFW/GLAD/GLM pod koreňovým priečinkom `Libraries`. Assimp pridáva koreňový `vcpkg.json` a konfigurácia MSBuild. `stb_image.h` patrí priamo do `project`. `opengl32.lib` poskytuje Windows SDK.

ZIP obsahuje zdroje aplikácie, shadery, modely s textúrami, pôvodné podklady Blenderu, skripty na prípravu modelov a screenshoty. Neobsahuje `Libraries`, `vcpkg_installed`, binárne knižnice, buildy ani `stb_image.h`. Video je samostatný MP4 súbor vedľa ZIP-u.

V aplikácii otvorí F5 učebňu so skyboxom. Súhrn splnenia bodov a zdroje modelov sú v `jar0193.md`. Autor potvrdil nahratie videa na spoločný Discord.

Do Kelvina sú pripravené `jar0193_8.zip`, `jar0193.md` a šesť screenshotov `jar0193_1a.png`, `jar0193_1b.png`, `jar0193_1c.png`, `jar0193_2a.png`, `jar0193_2b.png`, `jar0193_2c.png`. Súhrn a screenshoty sú tiež v ZIP-e. MP4 zostáva samostatne dostupné v tomto priečinku.
