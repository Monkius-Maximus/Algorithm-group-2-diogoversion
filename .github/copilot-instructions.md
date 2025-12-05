# Copilot Instructions for Zombie Rampage

## Project Overview

**Zombie Rampage** is a 2D tile-based RPG game written in C, transitioning from terminal (text-based) to graphical version using **Raylib**. The game features player progression, procedural map generation, and enemy AI using graph algorithms.

**Key Constraint:** Windows-only (currently), compiles with GCC + Raylib.

---

## Architecture & Core Modules

### Data Layer (Structs & Headers)

| Module | Key Struct | Purpose | Pattern |
|--------|-----------|---------|---------|
| **Personagem** | `Player` | Player state (HP, XP, class, position, inventory) | POO-inspired struct with class enum |
| **Mapa** | `Map` | 2D grid (10×10 tiles) + enemy array + encounter detection | Matrix + entity tracking |
| **Inventario** | `Inventory` | Dynamic vector of items (malloc-based) | Growable array pattern |
| **Inimigo** | `Inimigo` | Enemy state (separate from map—only referenced) | Linked via `Map.inimigos[]` array |
| **Batalha** | — | Turn-based combat logic (player vs enemy) | Not fully GUI-integrated yet |

**Key Design Decision:** Entities (players, enemies) store their own position (`pos_x`, `pos_y`), AND the map stores tile types separately. This decouples rendering from logic.

### Critical Functions & Data Flow

1. **Map Generation (`map_init`)** → Random walls (15%), then spawns 7 enemies + 5 items on TILE_EMPTY
2. **Player Movement (`map_move_player`)** → Validates move, updates position, clears old tile, marks new tile
3. **Enemy AI (`map_move_enemies`)** → **BFS pathfinding** to chase player (static function `bfs_find_next_step`)
4. **Encounters (`map_check_encounter`)** → Detects player-enemy collision, triggers battle (TODO: fully GUI-integrated)

### Algorithm Pattern: Breadth-First Search (BFS)

Enemies pursue the player using BFS:
- `bfs_find_next_step()` (Mapa.c, line 153) computes shortest path from enemy → player
- Returns the next (x, y) step toward player
- Called every frame for each active enemy

**Integration:** `map_move_enemies()` called in game loop; ensure enemies spawn at safe distance to avoid immediate encounters.

---

## Build & Compilation

### Makefile Configuration

- **RAYLIB_PATH** = `C:/raylib/raylib` (adjust if installed elsewhere; use forward slashes `/`)
- **Output:** `zombie_gui.exe`
- **Include Paths:** `-Iinclude -I$(RAYLIB_PATH)/src`
- **Linked Libraries:** `-lraylib -lopengl32 -lgdi32 -lwinmm`

### Build Commands (Powershell)

```powershell
# Compile and link
make

# Clean build artifacts
make clean

# Run compiled binary
.\zombie_gui.exe
```

---

## Graphical System (Raylib Integration)

### Rendering Pipeline

Located in `main.c`:

1. **DrawGame()** iterates map grid, switches on tile types:
   - `TILE_WALL` → Dark gray rectangle
   - `TILE_ZOMBIE` → Red rectangle
   - `TILE_ITEM` → Yellow rectangle
   - `TILE_EMPTY` → Light gray grid lines

2. **Player** drawn as blue rectangle at `jogador->pos_x * TILE_SIZE, jogador->pos_y * TILE_SIZE`

3. **Constants:**
   - `TILE_SIZE = 40` pixels per tile
   - Screen dimensions = `MAP_W * TILE_SIZE × MAP_H * TILE_SIZE`

### Key Raylib Calls

- `InitWindow()`, `SetTargetFPS(60)` → Setup
- `BeginDrawing()`, `ClearBackground()`, `EndDrawing()` → Frame cycle
- `IsKeyPressed(KEY_*)` → Input polling (W/A/S/D or arrow keys)
- `DrawRectangle()`, `DrawRectangleLines()` → Tile rendering
- `CloseWindow()` → Cleanup

---

## Code Conventions & Patterns

### Portuguese Naming (Intentional)

Variables, functions, and comments use Portuguese (e.g., `mapa`, `jogador`, `inimigo`). Maintain this for consistency with existing team code.

### Struct-Based OOP

- No inheritance; use `enum` for polymorphism (e.g., `Classe` enum for player types)
- Free dynamic memory with `inventory_free()` or equivalent when done
- Pass pointers to modify; use `const` for read-only access

### File I/O (Arquivos Module)

Player stats can be saved/loaded via binary files (planned feature). Wrap file operations in `Arquivos.c` functions rather than direct `fopen()`.

### Inventory Management

Dynamic allocation pattern:
- `inventory_init()` creates empty array
- `inventory_add()` grows capacity with `realloc()` if needed
- `inventory_remove_index()` shrinks array
- Always call `inventory_free()` before exit

---

## Game Loop & State Management

### Current Flow (main.c)

```
InitWindow → map_init + player placement 
  ↓
while (!WindowShouldClose):
  1. Poll input (W/A/S/D) → dx, dy
  2. map_move_player() → update position, check validity
  3. map_check_encounter() → detect collision
  4. DrawGame() → render frame
  ↓
CloseWindow
```

### Known TODOs / Integration Points

- **Battle GUI:** Currently prints to terminal; needs Raylib UI overlay
- **Enemy Movement:** `map_move_enemies()` not yet called in game loop—add after player move
- **Item Collection:** Logic exists but no GUI feedback
- **Save/Load:** Arquivos.c functions exist but not wired to main

---

## Common Tasks & Approaches

### Adding a New Tile Type

1. Add enum value to `Tile` in Mapa.h (e.g., `TILE_TRAP = 4`)
2. Update `map_init()` spawn logic
3. Add case in `DrawGame()` switch (main.c)
4. Update `map_print()` for terminal debug output

### Modifying Enemy AI

- Edit `bfs_find_next_step()` in Mapa.c (pathfinding algorithm)
- Adjust enemy speed: call `map_move_enemies()` less frequently (use frame counter)
- Add new enemy types: extend `crear_inimigo()` randomization in `map_init()`

### Fixing Player Movement Bugs

- Check `map_move_player()` boundary validation
- Verify `TILE_SIZE` calculations in rendering vs. logic
- Remember: `pos_x`, `pos_y` are grid coordinates, not pixels

---

## Debugging Tips

- Use `map_print()` to dump ASCII representation of current state
- Check `player_print_stats()` for character attributes
- Verify `mapa->grid[y][x]` vs. entity positions don't diverge
- GCC compile warnings are often legitimate (undefined behavior in tile rendering)

---

## External Dependencies

- **Raylib** (C library for 2D graphics, audio, input)
- **Standard C** (stdio, stdlib, string, time)
- **Windows OpenGL** (via Raylib integration)

**No package manager.** Raylib is manually installed and referenced via `RAYLIB_PATH` in Makefile.

---

## Team & Branches

- **Repo:** `Algorithm-group-2-diogoversion` (Monkius-Maximus)
- **Current Branch:** `copilot/implement-2d-graphics-system`
- **Contributors:** bgprs, dsr, gcfc, fves, lsc

Keep commits focused; discuss major refactors in Discord before pushing to main.
