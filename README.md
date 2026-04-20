# CommandCenter++

## Descripción
Motor de comandos en C++ para controlar una entidad con estado interno, inspirado en arquitecturas de videojuegos. Los comandos se registran por nombre y se ejecutan dinámicamente con argumentos.

## Compilación
```bash
g++ -std=c++20 -Wall -o main main.cpp
```

## Ejecución
```bash
./main
```

## Estructura del proyecto
- `main.cpp` — código fuente completo

## Clases principales

**Entity** — entidad con estado interno (vida, posición, nivel, recursos). Métodos: `move`, `heal`, `damage`, `levelUp`, `addRecurso`, `reset`, `status`.

**CommandCenter** — motor de comandos. Registra, ejecuta y elimina comandos. Mantiene historial con estado antes/después de cada ejecución.

## Tipos de comandos implementados

**Función libre** — `cmdStatus`, `cmdLevelUp` operan directamente sobre Entity.

**Lambda** — `heal`, `damage`, `move`, `addrecurso` capturan la entidad por referencia y validan sus argumentos.

**Functor** — `ResetCommand` cuenta cuántas veces se ejecutó. `LimitedHealCommand` bloquea el comando tras alcanzar un límite de usos.

## Macros disponibles
- `heal_and_status` — cura y muestra estado
- `full_damage_sequence` — aplica daño dos veces y muestra estado
- `reset_and_levelup` — reinicia y sube de nivel dos veces
- `prepare_battle` — cura, agrega recursos, mueve y muestra estado

## Características
- Validación de argumentos en todos los comandos
- Historial con snapshot antes/después de cada ejecución
- Eliminación dinámica de comandos en tiempo de ejecución
- Detención de macro si un comando interno no existe
- Iteradores explícitos (`std::map::iterator`, `std::list::iterator`)
- C++20 con `std::format`
