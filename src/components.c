#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define COMPONENT_POS 1

// note on components
// all components
typedef struct Component {
  int id;
  int type;
  struct Component* next;
  void* componentData;
} Component;


typedef struct SpriteComponent {
  void* sprite;
} SpriteComponent;

typedef struct PosComponent {
  Vector2 pos;
} PosComponent;

typedef struct BodyComponent {
  Vector2 velocity;
} BodyComponent;

typedef struct AreaComponent {
  Rectangle area;
} AreaComponent;


typedef struct GameObj {
  int id;
  struct GameObj *next;
  Component* components;
} GameObj;

typedef struct Game
{
  int gravity;
  GameObj *gameObjects;
} Game;

Component* pos() {
  Component* component = (Component*) malloc(sizeof(Component));
  PosComponent* pos = (PosComponent*) malloc(sizeof(PosComponent));
  memset(pos, 0, sizeof(PosComponent));
  component->id = 0;
  component->type = COMPONENT_POS;
  component->next = NULL;
  component->componentData = pos;
  return component;
}

void addComponent(GameObj *gameObj, Component* component) {
  if (gameObj == NULL) {
    return;
  }
  // find next attribute of last component pointer
  Component** lastComponentPtr = &gameObj->components;
  while (*lastComponentPtr != NULL) {
    lastComponentPtr = &((*lastComponentPtr)->next);
  }
  // set next ptr of last component
  *lastComponentPtr = component;
}

void addGameObject(Game *game, GameObj *obj) {
  if (game->gameObjects == NULL)
  {
    game->gameObjects = obj;
  }
  else
  {
    GameObj *currentGameObject = game->gameObjects;
    while (currentGameObject->next != NULL)
    {
      currentGameObject = currentGameObject->next;
    }
    currentGameObject->next = obj;
  }
  obj->next = NULL;
}

Component* findComponent(GameObj* gameObject, int componentType) {
  if (gameObject == NULL || gameObject->components == NULL) {
    return NULL;
  }
  Component *currentComponent = gameObject->components;
  while (currentComponent != NULL) {
    if (currentComponent->type == componentType) {
      return currentComponent;
    }
    currentComponent = currentComponent->next;
  }
  return NULL;
}


void setPosition(GameObj* gameObject, float x, float y) {
  Component* component = findComponent(gameObject, COMPONENT_POS);
  if (component != NULL) {
    PosComponent* pos = (PosComponent*) component->componentData;
    pos->pos.x = x;
    pos->pos.y = y;
  }
}


int main()
{
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "basic window");
  SetTargetFPS(60);

  /* -------------------------------------------------------------------------- */
  Game game;
  game.gameObjects = NULL;

  // Create and add GameObj instances
  GameObj *obj1 = (GameObj *)malloc(sizeof(GameObj));
  memset(obj1, 0, sizeof(GameObj));
  obj1->id = 1;
  addGameObject(&game, obj1);
  addComponent(obj1, pos());

  setPosition(obj1, GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight));

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())
  {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();



    ClearBackground(RAYWHITE);

    GameObj *current = game.gameObjects;
    while (current != NULL)
    {
      Component* posComponent = findComponent(current, COMPONENT_POS);
      if (posComponent != NULL) {
        PosComponent* posComponent2 = (PosComponent*) posComponent->componentData;
        DrawCircleV(posComponent2->pos, 20, RED);
      }
      current = current->next;
    }

    // On mouse click change the obj position
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      Vector2 mousePos = GetMousePosition();
      setPosition(obj1, mousePos.x, mousePos.y);
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}