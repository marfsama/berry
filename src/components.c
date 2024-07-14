#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>

#define COMPONENT_POS 1
#define COMPONENT_SPRITE 2

// base structure for a component
typedef struct Component {
  // unique id of component
  int id;
  // type of the component
  int type;
  // next component in list
  struct Component* next;
  // component struct depending on the type
  void* componentData;
} Component;

// position component. only has a position at the moment
typedef struct PosComponent {
  Vector2 pos;
} PosComponent;

// sprite component. does sprite animation too.
typedef struct SpriteComponent {
  // texture with image data
  Texture2D texture;
  // current frame number
  int frame;
  // rectangle of the texture which should be drawn,
  // unused
  Rectangle quad;
  // animation speed multiplier
  float animSpeed;
  // temp stuff to do sprite animation. later this will be done in sprite loading code
  int firstFrame;
  int numFrames;
  // time the current animation is already displayed
  float frameTime;
} SpriteComponent;

typedef struct BodyComponent {
  Vector2 velocity;
} BodyComponent;

typedef struct AreaComponent {
  Rectangle area;
} AreaComponent;

// basic object in game
typedef struct GameObj {
  // unique id of the object
  int id;
  // next object in list
  struct GameObj *next;
  // head of components linked list
  Component* components;
} GameObj;

// basic game data
typedef struct Game {
  int gravity;
  // head of game objects linked list
  GameObj *gameObjects;
} Game;

// creates a pos component
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

// creates a sprite component
Component* sprite(const char* resourceName, int width, int height, int firstFrame, int numFrames) {
 Texture2D texture = LoadTexture(resourceName);
  Component* component = (Component*) malloc(sizeof(Component));
  SpriteComponent* spriteComponent = (SpriteComponent*) malloc(sizeof(SpriteComponent));
  memset(spriteComponent, 0, sizeof(SpriteComponent));

  spriteComponent->texture = texture;
  spriteComponent->quad.width = width;
  spriteComponent->quad.height = height;
  spriteComponent->firstFrame = firstFrame;
  spriteComponent->numFrames = numFrames;
  component->id = 0;
  component->type = COMPONENT_SPRITE;
  component->next = NULL;
  component->componentData = spriteComponent;
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

// TODO: this should be a callback of some sort
// sprite() registers the callback
void draw(Game* game) {
  GameObj *current = game->gameObjects;
  while (current != NULL) {
    Component* posComponent = findComponent(current, COMPONENT_POS);
    if (posComponent != NULL) {
      PosComponent* posComponent2 = (PosComponent*) posComponent->componentData;
      Component* spriteComponent = findComponent(current, COMPONENT_SPRITE);
      if (spriteComponent != NULL) {
        SpriteComponent* spriteComponent2 = (SpriteComponent*) spriteComponent->componentData;
        float x = posComponent2->pos.x - spriteComponent2->quad.width/2;
        float y = posComponent2->pos.y - spriteComponent2->quad.height/2;
        DrawTextureRec(spriteComponent2->texture, spriteComponent2->quad, {x,y}, WHITE);
      }
      else {
        int x = posComponent2->pos.x;
        int y = posComponent2->pos.y;
        DrawCircle(x, y, 20, WHITE);
      }
    }
    current = current->next;
  }
}

// TODO: this should be a callback of some sort
// sprite() registers the callback
void update(Game* game) {
  float deltaTime = GetFrameTime();
  GameObj *current = game->gameObjects;
  while (current != NULL) {
    Component* spriteComponent = findComponent(current, COMPONENT_SPRITE);
    if (spriteComponent != NULL) {
      SpriteComponent* spriteComponent2 = (SpriteComponent*) spriteComponent->componentData;
      spriteComponent2->frameTime += deltaTime;
      if (spriteComponent2->frameTime > 0.25f) {
        // next animation
        spriteComponent2->frameTime = 0;
        int frame = spriteComponent2->frame;
        frame++;
        if (frame > spriteComponent2->firstFrame + spriteComponent2->numFrames) {
          frame = spriteComponent2->firstFrame;
        }
        spriteComponent2->frame = frame;
        // recalculate sprite from sprite atlas
        int cols = spriteComponent2->texture.width / spriteComponent2->quad.width;
        int x = (frame % cols) * spriteComponent2->quad.width;
        int y = (frame / cols) * spriteComponent2->quad.height;
        spriteComponent2->quad.x = x;
        spriteComponent2->quad.y = y;
        std::cout << "update " << frame << " - " << x << "x" << y << "\n";
      }

    }
    current = current->next;
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
  addComponent(obj1, sprite("resources/goblinsword.png", 64, 64, 0, 8));

  setPosition(obj1, GetRandomValue(0, screenWidth), GetRandomValue(0, screenHeight));

  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())
  {
    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();
      ClearBackground(RAYWHITE);
      draw(&game);
    EndDrawing();

    update(&game);


    // On mouse click change the obj position
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      Vector2 mousePos = GetMousePosition();
      setPosition(obj1, mousePos.x, mousePos.y);
    }

    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}