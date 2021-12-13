#include "GlobalDefines.h"
struct MenuItem{
  const char* title;
  f32 transitionTime;
  void (*Action)();
};

f32 GetBuildStageYSplitPoint(); //Game.cpp

void SetPlayGame(){
  core.mode = PROGRAM_MODE_GAME;
  GameState state = {0};
  state.mode = BUILD_STAGE_BUILD;

  core.gameOver = false;
  core.gameState = state;
  core.gameState.timer = 0;
  core.buildPartCards.count = 0;   
  for(u32 i = 0; i < CARDS_TO_ROLL; i++){
    core.buildPartCards.Add(RollNewBuildPartCard(CARD_HEIGHT,
                                                 GetBuildStageYSplitPoint() + CARD_HEIGHT,
                                                 offscreenState.renderTarget.width - CARD_HEIGHT,
                                                 offscreenState.renderTarget.height - CARD_HEIGHT));
  }

  core.player.parts.count = 0;
  RemoveRigidBody(&core.currentRoom->world, core.player.rigidBody_id);
  
  //Create force generator to push the ship around
  f32 rad = MATH::Random() * 2 * MATH::PI;
  f32 dir_x = MATH::Cos(rad);
  f32 dir_y = MATH::Sin(rad);
  
  CreateForceGenerator(&core.currentRoom->world,
                       dir_x, dir_y,
                       PART_FORCE_TO_APPLY / 50.0f);
  
  if(!core.windParticles.particles.data){
    core.windParticles = CreateParticleEmitter(malloc, free);
    core.windParticles.position.x = offscreenState.renderTarget.width / 2.0f;
    core.windParticles.position.y =  offscreenState.renderTarget.height / 2.0f;
    core.windParticles.rotation = rad;

    ParticleProperties props = {0};
    props.angle.min = 0;
    props.angle.max = 0;
    
    props.fadeIn.min = 0.1f;
    props.fadeIn.max = 1.2f;
    
    props.lifeTime.min = 2.0f;
    props.lifeTime.max = 5.0f;
 
    props.fadeOut.min = 0.5f;
    props.fadeOut.max = 1.7f;
  
    props.speed.min = 50;
    props.speed.max = 200;
  
    props.fadeInSize.min = 0;
    props.fadeInSize.max = 0;

    props.size.min = 8;
    props.size.max = 16;

    props.fadeOutSize.min = 0;
    props.fadeOutSize.max = 0;
    
    props.fadeInColor.min = props.fadeInColor.max =  {0.5f, 0.5f, 0.5f, 0.5};
    props.fadeInColor.min.w = 0;
    props.color.min = {0.5f, 0.5f, 0.5f, 0.7f};
    props.color.max = {1.0f, 1.0f, 1.0f, 0.7f};
    props.fadeOutColor.min = props.fadeOutColor.max = {0, 0, 0, 0};

    props.spriteIndex.min = 0;
    props.spriteIndex.max = 0;

    props.pos_x.min = offscreenState.renderTarget.width / -2.0f;
    props.pos_x.max = offscreenState.renderTarget.width / 2.0f;
    props.pos_y.min = offscreenState.renderTarget.height / -2.0f;
    props.pos_y.max = offscreenState.renderTarget.height / 2.0f;
    
    core.windParticles.perParticle = props;
    core.windParticles.particlesPerSecond = 40;
  }
  core.windParticles.particles.count = 0;
  
  BuildPartCard RollNewPartCard();
}
void SetShouldQuit(){
  core.mode = PROGRAM_MODE_SHOULD_QUIT;
}
MenuItem menuItems[] = {
  {"Play", 1, SetPlayGame} ,
  {"Quit", 0, SetShouldQuit}
};
u32 selectedItem;

void UpdateMainMenu(){
  f32 dt = core.dt_visual;
  /////////////////////////////////////////////////////////
  
  u32 itemCount = sizeof(menuItems) / sizeof(*menuItems); 
  if(IsKeyUp(&input, 'W')){
    selectedItem = (selectedItem + 1) % itemCount;
  }
  if(IsKeyUp(&input, 'S')){
    selectedItem = (selectedItem + itemCount - 1) % itemCount;
  }
  if(IsKeyUp(&input, ' ')){
    menuItems[selectedItem].Action();
  }
  
  for(u32 i = 0; i < sizeof(menuItems) / sizeof(*menuItems); i++){
    MenuItem* item = &menuItems[i];
    if(i == selectedItem){
      item->transitionTime = MIN(item->transitionTime + dt, MENU_ITEM_TRANSITION_TIME_MAX);
    }else{
      item->transitionTime = MAX(item->transitionTime - dt, 0);    
    }
  }
};
void DrawMainMenu(){
  offscreenState.material = fontMaterial;
  offscreenState.stateBits = 0;
  
  f32 scale = 1;
  u32 selectedColor = 0xff00ffff;
  u32 otherColor = 0xffffffff;
    
  RENDERER::IM::Immediate_Begin(&renderer.imRenderer, RENDERER::TOPOLOGY_TRIANGLES, offscreenState);
  f32 cx = offscreenState.renderTarget.width / 2;
  f32 cy = offscreenState.renderTarget.height / 2;

  f32 pad = 10;
  f32 titleScale = 3;
  u32 titleColor = 0xff00126f;
  f32 shadowPad = 9;
  MATH::Vector2<f32> textSize = GetTextSize(GAME_NAME,
                                            titleScale,
                                            &titleFont);

      
  RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                               cx - textSize.x / 2 + shadowPad,
                               offscreenState.renderTarget.height - pad * 5 - textSize.y - shadowPad,                                 
                               titleScale,
                               0xff000000,
                               GAME_NAME,
                               &titleFont);
  
  RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                               cx - textSize.x / 2,
                               offscreenState.renderTarget.height - pad * 5 - textSize.y,                                 
                               titleScale,
                               titleColor,
                               GAME_NAME,
                               &titleFont);
  
  
  MATH::Vector2<f32> tutTextSize = GetTextSize(TUTORIAL_STRING,
                                            scale * 0.5f,
                                            &titleFont);

      
  RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                               cx - tutTextSize.x / 2,
                               offscreenState.renderTarget.height - pad * 5 - textSize.y - pad - tutTextSize.y,                                 
                               scale * 0.5f,
                               (u32)~0,
                               TUTORIAL_STRING,
                               &titleFont);
  
  
  for(u32 i = 0; i < sizeof(menuItems) / sizeof(*menuItems); i++){
    MenuItem item = menuItems[i];

    f32 transitionScale = item.transitionTime / MENU_ITEM_TRANSITION_TIME_MAX;
    textSize = GetTextSize(item.title,
                           scale + transitionScale,
                           &titleFont);
    MATH::Vector2<f32> fullSize = GetTextSize(item.title,
                                              scale + 1,
                                              &titleFont);
    
      
    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 cx - textSize.x / 2,
                                 cy - textSize.y / 2 - 2 * (scale + transitionScale),
                                 scale + transitionScale,
                                 0xff000000,
                                 item.title,
                                 &titleFont);
    
    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 cx - textSize.x / 2,
                                 cy - textSize.y / 2,                                 
                                 scale + transitionScale,
                                 i == selectedItem ? selectedColor : otherColor,
                                 item.title,
                                 &titleFont);

    cy -= fullSize.y + pad;
  }

  const f32 creditsScale = 0.75f;
  textSize = GetTextSize(CREDITS_TEXT,
                         creditsScale,
                         &titleFont);
  
  
  RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                               pad,
                               pad + textSize.y,
                               creditsScale,
                               0xff000000,
                               CREDITS_TEXT,
                               &titleFont);
  
  RENDERER::IM::Immediate_End(&renderer.imRenderer);
}
