# Plugin Multiplataforma de generació procedimental de mapes

Aquest projecte és un plugin multiplataforma per **Unreal Engine 5** i **Unity** que genera un mapa procedimental de videojocs d'estratègia 4x.

L'objectiu del projecte és crear una llibreria en C++ agnòstica que es pugui compilar i utilitzar-la per qualsevol dels dos motors. Això permet centralitzar la lògica de l'algorisme de generació, mantenint així la consistència entre els motors.

## Arquitectura

El projecte està estructurat en 3 capes principals:

- Core: Conté la lògica de generació del mapa, implementada en C++. Aquesta capa és independent del motor.
- Wrapper: Conté el codi específic per cada motor, per traduir les crides de la capa Core a les API dels motors.
- Motor: Scripts específics per Unreal Engine (blueprints) i Unity (C#) que utilitzen el wrapper per generar el mapa.

## Core
 
El core està implementat en C++, i es compila com una llibreria dinàmica amb CMake.
Per poder utilitzar les funcions de la llibreria s'utilitza `__declspec(dllexport)__` per exportar les funcions a Windows (no hi ha implementació per altres sistemes operatius).
 
### Enllaços a documentació

- [Tutorial de CMake](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)
- [Documentació de C++ per exportar funcions](https://learn.microsoft.com/en-us/cpp/cpp/dllexport-dllimport?view=msvc-170)

## Integració amb Unreal Engine 5

A unreal el codi s'integra directament com un Mòdul del plugin. Com Unreal ja compila el codi C++ nativament, no cal compilar la llibreria dinàmica per separat.

De totes maneres, s'ha de fer conversió de classes entre el codi del core i el plugin d'Unreal (`std::vector` a `TArray` o `char` a `FString`).

### Spawn dels objectes al mon

Per instanciar els objectes al nivell d'Unreal s'utilitza la funció `SpawnActorsAtLocations`, una funció creada directament a la llibreria del plugin d'Unreal. Aquesta funció rep una llista de localitzacions i un tipus d'actor, i s'encarrega d'instanciar els actors al món.

Per tal que la funció tingui context del món, s'utilitza `WorldContext = "WorldContextObject"`, des del que es crida `GetWorld()`.
A més a més, per assegurar que la funció rep un actor vàlid, s'utilitza `TSubclassOf<AActor>`.

### Enllaços a documentació

- [Exposar codi C++ a Blueprints](https://www.google.com/search?q=https://dev.epicgames.com/documentation/en-us/unreal-engine/exposing-cplusplus-to-blueprints-in-unreal-engine&authuser=1)
- [Spawn d'actors](https://dev.epicgames.com/documentation/en-us/unreal-engine/spawning-actors-in-unreal-engine)
- [Especificar WorldContextObject](https://dev.epicgames.com/documentation/en-us/unreal-engine/metadata-specifiers-in-unreal-engine)

## Integració amb Unity

Per la integració amb Unity, primer es compila la llibreria dinàmica des de CMake.
Després, es crea un script en C# que utilitza `DllImport` per importar les funcions de la llibreria.

### Wrapper de Unity

Per tal que C# pugui utilitzar les funcions de la llibreria en C++, s'ha envoltat la lògica del core en un bloc `extern "C"` per evitar el name mangling de C++.
Això permet que les funcions siguin cridades des de C# sense problemes.

Per utilitzar el codi de la llibreria en un MonoBehaviour de Unity, s'utilitza el `[DllImport]` per importar les funcions.

### Gestió de memòria

Unity no pot gestionar la memòria assignada per C++, pel què en el cas de Unity s'ha decidit que el motor ha de passar un buffer preassignat a les funcions del core per omplir-lo amb les dades generades.
Això evita problemes de fugues de memòria i facilita la gestió de la memòria entre els dos llenguatges.

### Spawn dels objectes al mon

Per instanciar els objectes a Unity, s'utilitza la funció `Instantiate` de Unity, passant les posicions generades del core.

### Enllaços a documentació

- [Unity Native Plugins](https://docs.unity3d.com/Manual/NativePlugins.html)
- [DllImport en C#](https://learn.microsoft.com/en-us/dotnet/api/system.runtime.interopservices.dllimportattribute?view=net-7.0)
- [Interop Marshalling](https://learn.microsoft.com/en-us/dotnet/framework/interop/interop-marshalling)
- [On assignar la memòria](https://www.reddit.com/r/C_Programming/comments/fnfp07/should_apis_allocate_memory_or_require_memory_to)
- [Extern "C"](https://learn.microsoft.com/en-us/cpp/cpp/extern-cpp?view=msvc-170)

## Compilació del projecte

Per compilar el projecte, cal tenir `CMake` instal·lat.
Des de la línia de comandes, s'ha d'entrar a la carpeta build del plugin (`/MapGeneratorPlugin/build`):

```bash
cmake ..
cmake --build . --config Release
```