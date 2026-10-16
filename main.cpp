#include "raylib.h"
#include "raymath.h"
#include "rlgl.h" 

// --- World & Physics and uski Configuration ---
#define WORLD_X 30
#define WORLD_Y 20  // 6 blocks ke ground baki sab air
#define WORLD_Z 30

#define PLAYER_SPEED 6.0f
#define PLAYER_JUMP_FORCE 8.5f
#define GRAVITY 22.0f
#define REACH 6.0f

// Block ka number
enum BlockID { AIR = 0, GRASS = 1, DIRT = 2, STONE = 3 };

struct Player {
    Vector3 position;
    Vector3 velocity;
    bool isGrounded;
};

// --- Global World ka Data ---
int world[WORLD_X][WORLD_Y][WORLD_Z] = { 0 };

// --- Mere Helpers ---
Color GetBlockColor(int id) {
    if (id == GRASS) return (Color){ 40, 160, 40, 255 };  
    if (id == DIRT)  return (Color){ 110, 75,  40, 255 };  
    if (id == STONE) return (Color){ 120, 120, 120, 255 }; 
    return BLANK;
}

bool IsValidCoord(int x, int y, int z) {
    return (x >= 0 && x < WORLD_X && y >= 0 && y < WORLD_Y && z >= 0 && z < WORLD_Z);
}

bool IsPointInSolid(Vector3 p) {
    int x = (int)round(p.x);
    int y = (int)round(p.y);
    int z = (int)round(p.z);
    
    if (y < 0) return true; 
    if (!IsValidCoord(x, y, z)) return false; 
    
    return world[x][y][z] != AIR;
}

// --- OPTIMIZATION: Face Culling ---
// Only draw karo un blocks ko jo touching air (exposed to the player hai)
bool IsBlockExposed(int x, int y, int z) {
    // Blocks on the jo edge of the world boundary are always exposed
    if (x == 0 || x == WORLD_X - 1) return true;
    if (y == 0 || y == WORLD_Y - 1) return true;
    if (z == 0 || z == WORLD_Z - 1) return true;

    // If koi bhi side vala block is AIR, this block is exposed and must be drawn
    if (world[x+1][y][z] == AIR) return true;
    if (world[x-1][y][z] == AIR) return true;
    if (world[x][y+1][z] == AIR) return true;
    if (world[x][y-1][z] == AIR) return true;
    if (world[x][y][z+1] == AIR) return true;
    if (world[x][y][z-1] == AIR) return true;

    return false; // Surrounded by solid blocks, don't draw! okyyyyyyyy
}

// --- High-Precision vala Raycaster ---
struct RayResult {
    bool hit;
    Vector3 pos;
    Vector3 normal;
};

RayResult CastRay(Ray ray) {
    RayResult result = { false, {0,0,0}, {0,0,0} };
    float step = 0.05f; 
    Vector3 current = ray.position;

    for (float i = 0; i < REACH; i += step) {
        current = Vector3Add(current, Vector3Scale(ray.direction, step));
        int x = (int)round(current.x);
        int y = (int)round(current.y);
        int z = (int)round(current.z);

        if (IsValidCoord(x, y, z) && world[x][y][z] != AIR) {
            result.hit = true;
            result.pos = (Vector3){(float)x, (float)y, (float)z};
            
            Vector3 prev = Vector3Subtract(current, Vector3Scale(ray.direction, step));
            result.normal = (Vector3){ (float)((int)round(prev.x) - x), 
                                       (float)((int)round(prev.y) - y), 
                                       (float)((int)round(prev.z) - z) };
            return result;
        }
    }
    return result;
}

int main() {
    // 1. Core Engine Initialization
    SetConfigFlags(FLAG_MSAA_4X_HINT); 
    InitWindow(1280, 720, "Minecraft Pro Clone - 40x40 Base");
    InitAudioDevice();

    // 1. Load Textures                                                                 //meow
    Texture2D texGrass = LoadTexture("grass.png");
    Texture2D texDirt = LoadTexture("dirt.png");
    Texture2D texStone = LoadTexture("stone.png");

    // 2. Make karo only ONE Master Model
    Model masterBlock = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));               //meow

    SetTargetFPS(60);

    // 2. Audio Engine
    Music music = LoadMusicStream("bgm.mp3");
    music.looping = true; 
    PlayMusicStream(music);
    SetMusicVolume(music, 0.4f);

    // 3. Generate the 6-layer deep ground (40x40)
    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            world[x][0][z] = STONE; // 1 Stone
            world[x][1][z] = DIRT;  // 2 Dirt
            world[x][2][z] = DIRT;
            world[x][3][z] = GRASS; // 3 Grass
            world[x][4][z] = GRASS;
            world[x][5][z] = GRASS;
            // From y=6 to y=45 is purely AIR (40 blocks tall building space)
        }
    }

    // 4. Player Setup
    Player p = { { 20.0f, 8.0f, 20.0f }, { 0, 0, 0 }, false }; // Spawn in middle of 40x40
    Camera3D camera = { 0 };
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy = 75.0f; 
    camera.projection = CAMERA_PERSPECTIVE;

    // Inventory State
    int currentBlockInHand = DIRT; 

    DisableCursor();

    // 5. Main Game Loop
    while (!WindowShouldClose()) {
        UpdateMusicStream(music);
        float dt = GetFrameTime();

        // --- Block Selection Input (Z, X, C) ---
        if (IsKeyPressed(KEY_Z)) currentBlockInHand = GRASS;
        if (IsKeyPressed(KEY_X)) currentBlockInHand = DIRT;
        if (IsKeyPressed(KEY_C)) currentBlockInHand = STONE;

        // --- Camera & Raycasting ---
        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        Ray ray = { camera.position, Vector3Normalize(Vector3Subtract(camera.target, camera.position)) };
        RayResult hit = CastRay(ray);

        // Break Block
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hit.hit) {
            world[(int)hit.pos.x][(int)hit.pos.y][(int)hit.pos.z] = AIR;
        }
        
        // Place Block (Uses the currentBlockInHand variable)
        if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && hit.hit) {
            Vector3 spawn = Vector3Add(hit.pos, hit.normal);
            if (IsValidCoord(spawn.x, spawn.y, spawn.z) && 
                Vector3Distance(spawn, p.position) > 1.2f) {
                world[(int)spawn.x][(int)spawn.y][(int)spawn.z] = currentBlockInHand; 
            }
        }

        // ---  Movement kare vali Physics ---
        Vector3 forward = Vector3Normalize((Vector3){ camera.target.x - camera.position.x, 0, camera.target.z - camera.position.z });
        Vector3 right = Vector3Normalize((Vector3){ -forward.z, 0, forward.x }); 
        
        Vector3 direction = { 0 };

        if (IsKeyDown(KEY_W)) direction = Vector3Add(direction, forward);
        if (IsKeyDown(KEY_S)) direction = Vector3Subtract(direction, forward);
        if (IsKeyDown(KEY_A)) direction = Vector3Subtract(direction, right); 
        if (IsKeyDown(KEY_D)) direction = Vector3Add(direction, right);      

        if (Vector3Length(direction) > 0) direction = Vector3Normalize(direction);
        
        p.velocity.x = direction.x * PLAYER_SPEED;
        p.velocity.z = direction.z * PLAYER_SPEED;
        p.velocity.y -= GRAVITY * dt;

        if (p.isGrounded && IsKeyPressed(KEY_SPACE)) p.velocity.y = PLAYER_JUMP_FORCE;

        Vector3 nextPos = Vector3Add(p.position, Vector3Scale(p.velocity, dt));
        
        if (!IsPointInSolid((Vector3){ nextPos.x, p.position.y - 1.5f, p.position.z })) p.position.x = nextPos.x;
        if (!IsPointInSolid((Vector3){ p.position.x, p.position.y - 1.5f, nextPos.z })) p.position.z = nextPos.z;
        
        if (IsPointInSolid((Vector3){ p.position.x, nextPos.y - 1.6f, p.position.z })) {
            p.velocity.y = 0;
            p.isGrounded = true;
        } else {
            p.position.y = nextPos.y;
            p.isGrounded = false;
        }

        camera.position = p.position;

        // --- Rendering vala part  ---
        BeginDrawing();
        ClearBackground((Color){ 135, 206, 235, 255 }); 

        BeginMode3D(camera);
            
            // Set even thicker line width for the block borders
            //rlSetLineWidth(3.5f); 

            for (int x = 0; x < WORLD_X; x++) {
                for (int y = 0; y < WORLD_Y; y++) {
                    for (int z = 0; z < WORLD_Z; z++) {
                        // Only draw if it's NOT air AND it is exposed
                        if (world[x][y][z] != AIR && IsBlockExposed(x, y, z)) {
                            Vector3 bPos = {(float)x, (float)y, (float)z};
                            
                            int currentBlock = world[x][y][z];
    
                            // Swap the texture "jacket" based on the block type
                            if (currentBlock == GRASS) {
                                masterBlock.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texGrass;
                            } else if (currentBlock == DIRT) {
                                masterBlock.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texDirt;
                            } else if (currentBlock == STONE) {
                                masterBlock.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texStone;
                            }

                            // Draw the master model at the current coordinates
                            DrawModel(masterBlock, bPos, 1.0f, WHITE);
                            //DrawCube(bPos, 1.0f, 1.0f, 1.0f, GetBlockColor(world[x][y][z]));
                            //DrawCubeWires(bPos, 1.0f, 1.0f, 1.0f, BLACK); 
                        }
                    }
                }
            }
            
            //rlSetLineWidth(1.0f);

            if (hit.hit) {
                rlSetLineWidth(4.0f);
                DrawCubeWires(hit.pos, 1.01f, 1.01f, 1.01f, WHITE);
                rlSetLineWidth(1.0f);
            }

        EndMode3D();

        // --- 2D UI Overlay ---
        DrawCircle(GetScreenWidth()/2, GetScreenHeight()/2, 3, WHITE); 
        DrawCircleLines(GetScreenWidth()/2, GetScreenHeight()/2, 4, BLACK); 

        // FPS Counter
        DrawFPS(20, 20);

        // Determine string name for UI
        const char* blockNameStr = "UNKNOWN";
        Color uiTextColor = WHITE;
        if (currentBlockInHand == GRASS) { blockNameStr = "GRASS"; uiTextColor = GREEN; }
        if (currentBlockInHand == DIRT)  { blockNameStr = "DIRT"; uiTextColor = ORANGE; }
        if (currentBlockInHand == STONE) { blockNameStr = "STONE"; uiTextColor = LIGHTGRAY; }

        // Block In Hand UI with a semi-transparent background box for readability
        DrawRectangle(15, 50, 380, 40, (Color){ 0, 0, 0, 150 });
        DrawText(TextFormat("The Block In Your Hand Is : %s", blockNameStr), 25, 60, 20, uiTextColor);

        EndDrawing();
    }
    
    // 6. Cleanup
    // Stop Music
    UnloadMusicStream(music);
    CloseAudioDevice();

    // Only ONE model to unload!
    UnloadModel(masterBlock);
    
    // Unload Textures
    UnloadTexture(texGrass);
    UnloadTexture(texDirt);
    UnloadTexture(texStone);

    
    CloseWindow();
    return 0;
}