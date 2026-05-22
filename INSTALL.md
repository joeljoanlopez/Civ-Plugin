# Manual d'Instal·lació i Ús: Strategy Map Generation Plugin

## Taula de Continguts

1. [Requisits del Sistema](#1-requisits-del-sistema)
2. [Compilació del Nucli C++](#2-compilació-del-nucli-c)
3. [Integració amb Unity](#3-integració-amb-unity)
4. [Integració amb Unreal Engine 5](#4-integració-amb-unreal-engine-5)
5. [Ús Bàsic: Unity](#5-ús-bàsic-unity)
6. [Ús Bàsic: Unreal Engine 5](#6-ús-bàsic-unreal-engine-5)
7. [Configuració dels Paràmetres](#7-configuració-dels-paràmetres)
8. [Execució dels Tests](#8-execució-dels-tests)
9. [Resolució de Problemes](#9-resolució-de-problemes)

---

## 1. Requisits del Sistema

### Nucli C++

| Requisit | Versió mínima |
|----------|--------------|
| CMake | 3.14 |
| Compilador C++ | GCC 10 / Clang 12 / MSVC 2019 (suport C++20) |
| Connexió a internet | Necessària per descarregar GoogleTest |

### Integració Unity

| Requisit | Versió mínima |
|----------|--------------|
| Unity Editor | Unity 6 LTS |
| .NET | 4.x o .NET Standard 2.1 |
| Biblioteca compilada | `MapGenCore.dll` (Windows), `libMapGenCore.so` (Linux) o `libMapGenCore.dylib` (macOS) |

### Integració Unreal Engine 5

| Requisit | Versió mínima |
|----------|--------------|
| Unreal Engine | 5.0 |
| Visual Studio | 2022 (Windows) / clang++ (Linux) |
| Unreal Build Tool | Inclòs amb UE5 |

---

## 2. Compilació del Nucli C++

El directori `StrategyMapGenerationPlugin/` inclou un `Makefile` que simplifica el flux habitual:

```bash
cd StrategyMapGenerationPlugin

# Inicialitzar els git hooks i generar els fitxers CMake
make configure

# Compilar
make compile
```

La biblioteca generada es troba a `build/bin/libMapGenCore.so` (Linux), `build/bin/libMapGenCore.dylib` (macOS) o `build\bin\Release\MapGenCore.dll` (Windows).

**Altres targets disponibles:**

| Target | Acció |
|--------|-------|
| `make configure` | Inicialitza git hooks + genera fitxers CMake |
| `make compile` | Compila la biblioteca i els executables |
| `make test` | Compila i executa els tests unitaris |
| `make deploy` | Compila i copia la biblioteca a `Civ_Unity_Wrapper/Assets/Plugins/` |
| `make rebuild` | Neteja i torna a compilar des de zero |
| `make clean` | Neteja els artefactes de compilació |

> **Nota:** `make deploy` automatitza la integració amb Unity copiant la biblioteca directament al directori de plugins del projecte Unity.

### 2.3 Verificació de la compilació

Executar l'eina de verificació de determinisme:

```bash
# Linux
./build/bin/DeterminismCheck

# Windows
.\build\bin\Release\DeterminismCheck.exe
```

Ha de mostrar que dos mapes generats amb la mateixa llavor són idèntics.

---

## 3. Integració amb Unity

### 3.1 Preparació dels fitxers

1. Compilar el nucli seguint la [Secció 2](#2-compilació-del-nucli-c).

2. Copiar la biblioteca al projecte Unity:

```bash
# Linux
cp build/bin/libMapGenCore.so  <ProjecteUnity>/Assets/Plugins/libMapGenCore.so

# Windows
copy build\bin\Release\MapGenCore.dll  <ProjecteUnity>\Assets\Plugins\MapGenCore.dll
```

3. Copiar els scripts de C#:

```bash
cp Civ_Unity_Wrapper/Assets/Plugins/MapGeneratorWrapper.cs  <ProjecteUnity>/Assets/Plugins/
cp Civ_Unity_Wrapper/Assets/Plugins/MapTileInstancer.cs     <ProjecteUnity>/Assets/Plugins/
```

---

## 4. Integració amb Unreal Engine 5

### 4.1 Copiar el plugin

Copiar directament des del repositori:

```bash
cp -r Civ_Unreal_Wrapper/Plugins/MapGenPlugin/  <ProjecteUE5>/Plugins/MapGenPlugin/
```

> **Important:** el plugin inclou el codi font del nucli C++ amb rutes relatives. La carpeta `StrategyMapGenerationPlugin/` ha de ser accessible des del plugin. Si el projecte UE5 és independent, cal copiar també la carpeta `StrategyMapGenerationPlugin/` al nivell del plugin i ajustar les rutes a `MapGenPlugin.Build.cs`.

### 4.2 Activar el plugin

1. Obrir Unreal Editor
2. Anar a **Edit → Plugins**
3. Cercar **MapGenPlugin** i activar-lo
4. Reiniciar l'editor quan se sol·liciti
5. UBT compilarà el plugin automàticament

---

## 5. Ús Bàsic: Unity

### 5.1 Configuració de l'escena

1. Crear un `GameObject` buit a l'escena.
2. Afegir el component **MapGeneratorWrapper** des de l'Inspector.
3. Afegir el component **MapTileInstancer** al mateix `GameObject` (o a un de fill).

### 5.2 Configuració de MapGeneratorWrapper

| Paràmetre | Descripció | Valor per defecte |
|-----------|-----------|-------------------|
| **Width** | Amplada de la graella (caselles) | 8 |
| **Height** | Alçada de la graella (caselles) | 6 |
| **Seed** | Llavor de generació | 1234 |
| **Plate Count** | Nombre de plaques tectòniques | 6 |
| **Land Ratio** | Proporció terra/mar `[0, 1]` | 0.5 |
| **Noise Octaves** | Octaves de soroll fBm | 3 |

### 5.3 Regenerar el mapa en temps d'editor

Amb el component **MapGeneratorWrapper** seleccionat:

1. Clic dret sobre el component a l'Inspector
2. Seleccionar **Regenerate Map**

Per restaurar els terrenys als valors per defecte:

- Clic dret → **Reset Terrain Types to Defaults**
- Clic dret → **Reset Climate Settings to Defaults**

### 5.4 Subscripció a l'event de generació

```csharp
using Plugins;

public class MeuScript : MonoBehaviour
{
    private MapGeneratorWrapper wrapper;

    void Awake()
    {
        wrapper = GetComponent<MapGeneratorWrapper>();
        wrapper.OnMapGenerated += OnMapReady;
    }

    private void OnMapReady(MapGeneratorWrapper.MapGenTileData[] tiles)
    {
        foreach (var tile in tiles)
        {
            Debug.Log($"Casella ({tile.q},{tile.r}): {(tile.isLand == 1 ? "terra" : "mar")}, " +
                      $"altura={tile.height:F2}, terreny={wrapper.GetTerrainName(tile.terrain)}");
        }
    }

    void OnDestroy()
    {
        if (wrapper != null)
            wrapper.OnMapGenerated -= OnMapReady;
    }
}
```

### 5.5 Configuració de MapTileInstancer

1. A l'Inspector de **MapTileInstancer**, expandir la llista **Terrain Prefabs**
2. Per a cada índex de terreny (0 = Deep Ocean, 1 = Ocean, etc.), afegir un o més prefabs
3. El component selecciona un prefab a l'atzar de la llista per a cada casella d'aquell terreny

Paràmetres addicionals:

| Paràmetre | Descripció |
|-----------|-----------|
| **Hex Size** | Radi del hexàgon en unitats Unity |
| **Height Scale** | Multiplicador d'altura visual |
| **Tile Scale** | Escala uniforme de cada prefab |

---

## 6. Ús Bàsic: Unreal Engine 5

### 6.1 Configuració amb Blueprints

1. Crear un nou **Actor** Blueprint.
2. Afegir el component **MapGeneratorWrapper** des del panell de components.
3. Afegir el component **MapTileInstancerComponent**.
4. Compilar el Blueprint.

### 6.2 Configuració dels paràmetres

Al panell **Details** de `MapGeneratorWrapper`:

- Configurar Width, Height, Seed, Plate Count, Land Ratio i Noise Octaves
- A la secció **Terrain Types**, definir els tipus de terreny amb els seus rangs climàtics

### 6.3 Lligar l'event OnMapGenerated

Al Blueprint Event Graph:

1. Seleccionar el component `MapGeneratorWrapper`
2. A **Details**, afegir un binding a **On Map Generated**
3. A l'event, iterar sobre l'array de `Tiles` per processar cada casella

### 6.4 Configuració de MapTileInstancerComponent

1. Al panell **Details**, expandir **Terrain Meshes**
2. Per a cada índex de terreny, afegir un `Static Mesh` a la llista corresponent
3. Configurar **Tile Scale** i **Height Scale** per ajustar les dimensions

### 6.5 Generació des de C++ (UE5)

```cpp
#include "MapGeneratorWrapper.h"

void AMyActor::BeginPlay()
{
    Super::BeginPlay();

    UMapGeneratorWrapper* Gen = NewObject<UMapGeneratorWrapper>(this);
    Gen->Width = 20;
    Gen->Height = 15;
    Gen->Seed = 42;
    Gen->PlateCount = 8;
    Gen->LandRatio = 0.6f;

    Gen->OnMapGenerated.AddDynamic(this, &AMyActor::HandleMapGenerated);
    Gen->GenerateMap();
}

void AMyActor::HandleMapGenerated(const TArray<FMapGenTileData>& Tiles)
{
    for (const FMapGenTileData& Tile : Tiles)
    {
        UE_LOG(LogTemp, Log, TEXT("Casella (%d,%d): terra=%d, altura=%.2f"),
               Tile.Q, Tile.R, Tile.bIsLand ? 1 : 0, Tile.Height);
    }
}
```

---

## 7. Configuració dels Paràmetres

### 7.1 Paràmetres de Generació Principal

| Paràmetre | Rang recomanat | Efecte |
|-----------|---------------|--------|
| `seed` | Qualsevol enter | Canvia completament el mapa. Mateixa llavor = mateix mapa |
| `width` / `height` | 4 – 100 | Mida de la graella. Valors grans augmenten el temps de generació |
| `plateCount` | 2 – 20 | Més plaques = masses continentals més fragmentades i variades |
| `landRatio` | 0.2 – 0.8 | Proporció de caselles de terra. 0.5 = 50% terra, 50% mar |
| `noiseOctaves` | 1 – 6 | Més octaves = terrenys més detallats. Cada octava duplica la freqüència |

### 7.2 Paràmetres de Soroll (TerrainNoiseSettings)

| Paràmetre | Valor per defecte | Efecte |
|-----------|------------------|--------|
| `noiseScale` | 0.1 | Escala espacial del soroll. Valors petits = variació lenta i suau |
| `initialAmplitude` | 1.0 | Amplitud de la primera octava |
| `initialFrequency` | 2.0 | Freqüència de la primera octava |
| `amplitudeDecay` | 0.5 | Factor de reducció d'amplitud per octava (0.5 = la meitat cada vegada) |
| `frequencyMultiplier` | 2.0 | Factor d'augment de freqüència per octava |
| `noiseStrength` | 0.5 | Intensitat total del soroll sobre l'altura base |

### 7.3 Paràmetres Climàtics (ClimateSettings)

| Paràmetre | Valor per defecte | Efecte |
|-----------|------------------|--------|
| `equatorNormalizedRow` | 0.5 | Posició de l'equador. 0.5 = centre. 0 = fila superior, 1 = fila inferior |
| `elevationTempPenalty` | 0.5 | Reducció de temperatura per unitat d'altura. Simula el refredament altitudinal |
| `temperatureNoiseStrength` | 0.12 | Variació aleatòria de temperatura (evita zones climàtiques massa uniformes) |
| `moistureNoiseStrength` | 0.15 | Variació aleatòria d'humitat |

### 7.4 Definició de Tipus de Terreny Personalitzats

Cada tipus de terreny necessita:

1. **`name`** — identificador llegible
2. **`isWater`** — si és aquàtic (no usa el model de Whittaker)
3. **`baseHeight`** — altura de referència per la interpolació de smoothstep
4. **`maxHeight`** — altura màxima per al sistema de selecció per altura (fallback)
5. **Rangs climàtics** — `minTemperature`, `maxTemperature`, `minMoisture`, `maxMoisture`

Exemple de terreny personalitzat "Sabana":

```csharp
// Unity
new TerrainTypeDefinition {
    name = "Sabana",
    maxHeight = 1e9f,
    baseHeight = 0.65f,
    isWater = false,
    minTemperature = 0.55f,
    maxTemperature = 0.9f,
    minMoisture = 0.1f,
    maxMoisture = 0.45f
}
```

> **Nota:** els terrenys aquàtics ignoren els rangs climàtics. L'assignació de bioma usa únicament la distància euclidiana en l'espai (temperatura, humitat) per als terrenys de terra.

---

## 8. Execució dels Tests

Des de `StrategyMapGenerationPlugin/`:

```bash
make test
```

Compila (si cal) i executa tots els tests unitaris. Sortida esperada: tots els tests en estat `PASSED`.

Per filtrar per suite específica, executar l'executable directament:

```bash
# Linux
./build/bin/UnitTests --gtest_filter="HexCoord*"
./build/bin/UnitTests --gtest_filter="TectonicsGenerator*"
./build/bin/UnitTests --gtest_filter="MapGenerationAPI*"
```

---

## 9. Resolució de Problemes

### La biblioteca no es troba en temps d'execució (Unity)

**Símptoma:** `DllNotFoundException: MapGenCore`

**Solució:**
1. Verificar que `MapGenCore.dll` / `libMapGenCore.so` es troba a `Assets/Plugins/`
2. Seleccionar la biblioteca a Unity i confirmar que la plataforma correcta és marcada a **Plugin Import Settings**
3. En Linux, verificar que la biblioteca té permisos d'execució: `chmod +x libMapGenCore.so`

### Error de compilació a Unreal: "cannot open source file"

**Símptoma:** UBT no troba els headers del nucli C++

**Solució:**  
Verificar les rutes relatives a `MapGenPlugin.Build.cs`. Si el plugin és independent (fora del repositori original), copiar la carpeta `StrategyMapGenerationPlugin/` adjacent al plugin i actualitzar `PluginDirectory` al fitxer de build.
