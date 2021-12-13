#ifndef CORE_H
#define CORE_H

#include "Room.h"
#include "Entity.h"
#include "Array.h"
#include "BuildPart.h"

enum PROGRAM_MODE{
  PROGRAM_MODE_MENU = 0,
  PROGRAM_MODE_GAME,
  PROGRAM_MODE_SHOULD_QUIT,
};
enum BUILD_STAGE_MODE{
  BUILD_STAGE_BUILD,
  BUILD_STAGE_TRANSITION,
  BUILD_STAGE_END,
};
struct GameState{
  f32 timer;
  f32 gameOverTime;
  BUILD_STAGE_MODE mode;
  

  //////////////////////////////////////////////////
  //Drag stuff
  bool isCardDragged;
  i32 hoveredCardIndex;
  
  MATH::Vector2<f32> mouseLocalSpace;
  MATH::Vector2<f32> cardDragOffset;
  //////////////////////////////////////////////////
  
};
struct Core{
  bool gameOver;
  PROGRAM_MODE mode;
  GameState gameState;
  bool debugDraw;
  Room* currentRoom;

  Player player;

  Array<BuildPartCard> buildPartCards;
  Array<Bullet> bullets;
  
  f32 dt_visual;
  f32 dt_sim;

  ParticleEmitter windParticles;
};

extern Core core;

#endif
