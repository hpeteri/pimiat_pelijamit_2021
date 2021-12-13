#include "GlobalDefines.h"
#include "PLATFORM_Input_Keycodes.h"
//////////////////////////////////////////////////////////////////////
//Build stage
f32 GetBuildStageYSplitPoint(){
  return  offscreenState.renderTarget.height / 3; 
}
MATH::Vector2<f32> GetPresumedCenter(){
  MATH::Vector2<f32> c;
  c.x = offscreenState.renderTarget.width / 2;
  c.y = GetBuildStageYSplitPoint() / 2;
  return c;
}
void DoBuildStageControls(){
  f32 mouse_x = core.gameState.mouseLocalSpace.x;
  f32 mouse_y = core.gameState.mouseLocalSpace.y;

  bool outOfTime = core.gameState.timer >= BUILD_TIME;
  
  if(IsKeyUp(&input, KEYCODE_L_BUTTON) || outOfTime){
    if(core.gameState.isCardDragged){
      auto card = core.buildPartCards[core.gameState.hoveredCardIndex];
      core.gameState.isCardDragged = false;
      if(mouse_y <= GetBuildStageYSplitPoint()){
        card->isLocked = true;
      }   
    }
  }else if(!IsKeyHeld(&input, KEYCODE_L_BUTTON)){
    core.gameState.hoveredCardIndex = -1;
    
        
    for(i32 i = core.buildPartCards.count - 1; i >= 0; i--){
      auto card = *core.buildPartCards[i];
        
      OrientedRectangle cardRect;
      cardRect.p0 = card.position;
      cardRect.rotation = card.rotation;
      cardRect.size.x = CARD_WIDTH;
      cardRect.size.y = CARD_HEIGHT;

      if(IsPointInside({mouse_x, mouse_y}, cardRect)){
        core.gameState.hoveredCardIndex = i;
        core.gameState.cardDragOffset.x = card.position.x - mouse_x;
        core.gameState.cardDragOffset.y = card.position.y - mouse_y;
        
        break;
      }
    }
  }
  
  if(IsKeyDown(&input, KEYCODE_L_BUTTON)){
    auto it = core.buildPartCards[core.gameState.hoveredCardIndex];
    if(it){
      auto card = *it;
      core.buildPartCards.RemoveAndKeepOrder(core.gameState.hoveredCardIndex);

      
      core.buildPartCards.Add(card);
      core.gameState.hoveredCardIndex = core.buildPartCards.count - 1;
      core.gameState.isCardDragged = true;
    }
  }
  if(core.gameState.isCardDragged){
    auto card = core.buildPartCards[core.gameState.hoveredCardIndex];
    
    if(mouse_y <= GetBuildStageYSplitPoint()){
      card->isFlipped = true;
    }
    if(!card->isLocked){
      card->position.x = mouse_x + core.gameState.cardDragOffset.x;
      card->position.y = mouse_y + core.gameState.cardDragOffset.y;
    
      if(IsKeyHeld(&input, 'A')){
        card->rotation += core.dt_sim * ROTATION_SPEED;
      }
      if(IsKeyHeld(&input, 'D')){
        card->rotation -= core.dt_sim * ROTATION_SPEED;
      }
    }
  }  
}

void DoEngine(PlayerPart* part, RigidBody* rb){
  f32 dir_x = MATH::Cos(part->rotation + rb->rotation);
  f32 dir_y = MATH::Sin(part->rotation + rb->rotation);
  
  ApplyForce(rb, dir_x, dir_y, part->forceToApply);
  ApplyTorque(rb, -part->forceToApply, part->distanceToBody / 200, part->rotationToBody - part->rotation);
}
void DoGun(PlayerPart* part, RigidBody* rb){
  if(part->timer < 1 / GUN_FIRE_RATE) return;

  part->timer = 0;
  
  f32 dir_x = MATH::Cos(part->rotation + rb->rotation);
  f32 dir_y = MATH::Sin(part->rotation + rb->rotation);
  ApplyForce(rb, dir_x, dir_y, -part->forceToApply);
  ApplyTorque(rb, part->forceToApply, part->distanceToBody / 200, part->rotationToBody - part->rotation);

  Circle circle = {0};
  circle.r = BULLET_SIZE;

  f32 dir_x1 = MATH::Cos(part->rotationToBody + rb->rotation) * part->distanceToBody;
  f32 dir_y1 = MATH::Sin(part->rotationToBody + rb->rotation) * part->distanceToBody;

  auto shot_id = CreateRigidBody(&core.currentRoom->world, 1, 0,
                                 rb->position.x + dir_x1,
                                 rb->position.y + dir_y1);

  auto collider_id = CreateCollider(&core.currentRoom->world, shot_id, circle, IS_TRIGGER);

  ApplyForce(GetRigidBodyWithID(&core.currentRoom->world, shot_id), dir_x, dir_y, part->forceToApply * 100);
  Bullet bullet;
  
  bullet.rigidBody_id = shot_id;
  bullet.collider_id = collider_id;
  auto it = core.bullets.Add(bullet);
}

void SetParticleEmitterProps(ParticleEmitter* emitter, PART_TYPE type){
  ParticleProperties props;
  if(type == PART_TYPE_ENGINE){
    props.angle.min = 0;
    props.angle.max = 0;
    
    props.fadeIn.min = 0.1f;
    props.fadeIn.max = 0.2f;
    
    props.lifeTime.min = 0.1f;
    props.lifeTime.max = 0.1f;
 
    props.fadeOut.min = 0.5f;
    props.fadeOut.max = 0.7f;
  
    props.speed.min = 30;
    props.speed.max = 50;
  
    props.fadeInSize.min = 16;
    props.fadeInSize.max = 32;

    props.size.min = 16;
    props.size.max = 32;

    props.fadeOutSize.min = 16;
    props.fadeOutSize.max = 32;
    
    props.fadeInColor.min = props.fadeInColor.max =  {0.5f, 0.5f, 0.5f, 1};    
    props.color.min = props.color.max = {0.5f, 0.5f, 0.5f, 0.7f};
    props.fadeOutColor.min = props.fadeOutColor.max = {0, 0, 0, 0};

    props.spriteIndex.min = 0;
    props.spriteIndex.max = 0;
  
    emitter->particlesPerSecond = 10;
  }else if(type == PART_TYPE_GUN){
    props.angle.min = 0;
    props.angle.max = 0;
    
    props.fadeIn.min = 0.1f;
    props.fadeIn.max = 0.2f;
    
    props.lifeTime.min = 0.2f;
    props.lifeTime.max = 0.2f;
 
    props.fadeOut.min = 0.2f;
    props.fadeOut.max = 0.2f;
  
    props.speed.min = 10;
    props.speed.max = 15;
  
    props.fadeInSize.min = 32;
    props.fadeInSize.max = 64;

    props.size.min = 16;
    props.size.max = 32;

    props.fadeOutSize.min = 8;
    props.fadeOutSize.max = 16;
    
    props.fadeInColor.min = {1, 1, 1, 0.5f};
    props.fadeInColor.max = {1, 1, 1, 0.5f};
    props.color.min = {1, 1, 1, 1};
    props.color.max = {1, 1, 1, 1};

    props.pos_x.min = -PART_WIDTH / 2;
    props.pos_x.max = PART_WIDTH / 2;
    props.pos_y.min = -PART_WIDTH / 2;
    props.pos_y.max = PART_WIDTH / 2;
    
    props.fadeOutColor.min = props.fadeOutColor.max = {1, 1, 1, 0};

    props.spriteIndex.min = 0;
    props.spriteIndex.max = 0;
    
    emitter->particlesPerSecond = GUN_FIRE_RATE;
  }
  emitter->perParticle = props;
}
void DoBuildStageSim(){
  f32 dt = core.dt_sim;
  //////////////////////////////////////////////////
  core.gameState.timer += dt;

  if(core.gameState.mode == BUILD_STAGE_BUILD){
    DoBuildStageControls();

    if(core.gameState.timer >= BUILD_TIME){
      core.gameState.mode = BUILD_STAGE_TRANSITION;
      core.gameState.timer = 0;

      //REMOVE THIS, this should be after transition has ended, but
      //we dont care about animations yet
      core.gameState.mode = BUILD_STAGE_END;
      
      auto presumedCenter = GetPresumedCenter();
      
      f32 totalMass = 1;
      MATH::Vector2<f32> centerOfMass = {0,0};

      for(u32 i = 0; i < core.buildPartCards.count; i++){
        auto card = *core.buildPartCards[i];
        if(!card.isLocked) continue;
        
        totalMass += card.mass;
        centerOfMass += (card.position - presumedCenter) * card.mass;
      }
      if(totalMass){
        centerOfMass.x /= totalMass;
        centerOfMass.y /= totalMass;
      }
      u64 e_id = CreateRigidBody(&core.currentRoom->world,
                                 totalMass,
                                 0,
                                 presumedCenter.x, presumedCenter.y);
      
      core.player.rigidBody_id = e_id;

      
      //Part collider
      OrientedRectangle rect;
      rect.size.x = PART_WIDTH;
      rect.size.y = PART_HEIGHT;

      f32 min_x = MATH::Inf;
      f32 max_x = -MATH::Inf;
      f32 min_y = MATH::Inf;
      f32 max_y = -MATH::Inf;
            
      for(u32 i = 0; i < core.buildPartCards.count; i++){
        auto card = *core.buildPartCards[i];
        if(!card.isLocked) continue;

        rect.p0 = card.position - (presumedCenter + centerOfMass);
        rect.p0.x /= 2.0f;
        rect.p0.y /= 2.0f;
        
        rect.rotation = card.rotation;        
        CreateCollider(&core.currentRoom->world, e_id, rect, IS_COLLISION_CALLBACK);
     
        min_x = MIN(rect.p0.x, min_x);
        max_x = MAX(rect.p0.x, max_x);                    
        min_y = MIN(rect.p0.y, min_y);
        max_y = MAX(rect.p0.y, max_y);
        
        PlayerPart part = {0};
        part.distanceToBody = MATH::Length(rect.p0);
        part.rotationToBody = MATH::Atan2(rect.p0.y, rect.p0.x);
        part.rotation = card.rotation + MATH::PI / 2;
        part.activationKeyIndex = card.binding - PART_BINDING_W;
        part.spriteIndex = (card.type - PART_TYPE_ENGINE);
        part.particleEmitter = CreateParticleEmitter(malloc, free);
        SetParticleEmitterProps(&part.particleEmitter, card.type);
        part.forceToApply = PART_FORCE_TO_APPLY;
        
        if(card.type == PART_TYPE_ENGINE)
          part.OnActivate = &DoEngine;
        else if(card.type == PART_TYPE_GUN)
          part.OnActivate = &DoGun;
        else if(card.type == PART_TYPE_MASS)
          part.OnActivate = nullptr;
        
        core.player.parts.Add(part);
      }
      
      SetInertia_Rectangle(GetRigidBodyWithID(&core.currentRoom->world, e_id),
                           max_x - min_x,
                           max_y - min_y);

    }
  } 
}
void DrawBuildStageBG(){
  //////////////////////////////////////////////////
  //Draw line on the middle
  offscreenState.material = colorMaterial;
  offscreenState.stateBits = 0;
  RENDERER::IM::Immediate_Begin(&renderer.imRenderer, RENDERER::TOPOLOGY_TRIANGLES, offscreenState);

  f32 cy = GetBuildStageYSplitPoint();
      
  RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                               0, cy,
                               offscreenState.renderTarget.width,
                               cy,
                               5,
                               (u32)~0);
    
  RENDERER::IM::Immediate_End(&renderer.imRenderer);
  RENDERER::IM::Immediate_Flush(&renderer.imRenderer);
}
void DrawBuildStageCards(){
  offscreenState.material = diffuseMaterial;
  offscreenState.material.diffuseTexture = cardTexture;
  offscreenState.stateBits = RENDERER::BLEND_BIT;

  auto batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);

  
  for(u32 i = 0; i < core.buildPartCards.count; i++){
    auto card = *core.buildPartCards[i];

    RENDERER::BATCH::Instance instance = {0};
    f32 sizeIncrease = (core.gameState.hoveredCardIndex == i && !card.isLocked) ? 10 : 0;
    
    instance.model = 
      MATH::Translate(card.position.x, card.position.y, 1) *
      MATH::RotateZ(card.rotation) *
      MATH::Scale(CARD_WIDTH + sizeIncrease, CARD_HEIGHT + sizeIncrease, 1);

    //Usedifferent color for hovered card
    
    instance.color = (u32)~0;
      
    if(!card.isFlipped){

      instance.diffuseLayer = 0;
      if(!RENDERER::BATCH::PushInstance(batch, instance))
        batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
   
      
    }else{
      //Draw card type
      instance.diffuseLayer = card.type;
      if(!RENDERER::BATCH::PushInstance(batch, instance))
        batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);

      //Draw card binding
      instance.diffuseLayer = card.binding;      
      if(!RENDERER::BATCH::PushInstance(batch, instance))
        batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);

      
      //Draw card lock
      if(card.isLocked){
        instance.diffuseLayer = 1;
        if(!RENDERER::BATCH::PushInstance(batch, instance))
          batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);

      }
    }     
  }
  //////////////////////////////////////////////////////////////////////
  //Draw Center
  //
  auto presumedCenter = GetPresumedCenter();
  
  RENDERER::BATCH::Instance instance = {0};
  f32 totalMass = 1;
  MATH::Vector2<f32> centerOfMass = {0, 0};

  for(u32 i = 0; i < core.buildPartCards.count; i++){
    auto card = *core.buildPartCards[i];
    if(!card.isLocked) continue;
        
    totalMass += card.mass;
    centerOfMass += (card.position - presumedCenter) * card.mass;
  }
  if(totalMass != 0.0f){
    centerOfMass.x /= totalMass;
    centerOfMass.y /= totalMass;
  }
  
  instance.model = 
    MATH::Translate(centerOfMass.x + presumedCenter.x, centerOfMass.y + presumedCenter.y, 1) * 
    MATH::RotateZ(0) *
    MATH::Scale(16, 16, 1);

  
  instance.color = 0xff0000ff;
  
  if(!RENDERER::BATCH::PushInstance(batch, instance))
    batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
  
}
void DrawBuildStageUI(){
  //////////////////////////////////////////////////
  //Draw timer
  offscreenState.stateBits = 0;
  offscreenState.material = fontMaterial;
   
  f32 scale = 2.0f;
  u32 color = 0xffffffff;
  f32 shadowPad = 6;

  char title[256];
  sprintf(title, "%.2f\n", core.gameState.timer);
  
  
  RENDERER::IM::Immediate_Begin(&renderer.imRenderer, RENDERER::TOPOLOGY_TRIANGLES, offscreenState);

  f32 cx = offscreenState.renderTarget.width / 2;
  f32 cy = offscreenState.renderTarget.height;

  MATH::Vector2<f32> textSize = GetTextSize(title,
                                            scale,
                                            &titleFont);

      
  RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                               cx - textSize.x / 2 + shadowPad,
                               cy - textSize.y / 2 - shadowPad,                                 
                               scale,
                               0xff000000,
                               title,
                               &titleFont);
  
  RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                               cx - textSize.x / 2,
                               cy - textSize.y / 2,                                 
                               scale,
                               color,
                               title,
                               &titleFont);

  
  RENDERER::IM::Immediate_End(&renderer.imRenderer);

}
void DrawBuildStage(){
  if(core.gameState.mode == BUILD_STAGE_BUILD){
    DrawBuildStageBG();
    DrawBuildStageCards();
    RENDERER::BATCH::Flush(&renderer.batchRenderer);

    DrawBuildStageUI();
    RENDERER::IM::Immediate_Flush(&renderer.imRenderer);
    
  }else{ 
    DrawBuildStageUI();
  }
}
void DrawGame(){  
  offscreenState.material = diffuseMaterial;
  offscreenState.material.diffuseTexture = playerTexture;
  offscreenState.stateBits = RENDERER::BLEND_BIT;

  auto batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
  for(u32 i = 0; i < core.player.parts.count; i++){
    auto part = *core.player.parts[i];;  
    f32 offset_x = MATH::Cos(core.player.rotation + part.rotationToBody) * part.distanceToBody;
    f32 offset_y = MATH::Sin(core.player.rotation + part.rotationToBody) * part.distanceToBody;

    RENDERER::BATCH::Instance instance = {0};
    
    instance.model = 
      MATH::Translate(core.player.position.x + offset_x, core.player.position.y + offset_y, 1) * 
      MATH::RotateZ(core.player.rotation + part.rotation) *
      MATH::Scale(PART_WIDTH, PART_HEIGHT, 1);
    
    instance.color = (u32)~0;
    instance.diffuseLayer = part.spriteIndex;

    if(!RENDERER::BATCH::PushInstance(batch, instance))
      batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);

  }
  for(u32 i = 0; i < core.bullets.count; i++){
    auto bullet = core.bullets[i];
    auto rb = GetRigidBodyWithID(&core.currentRoom->world, bullet->rigidBody_id);
   
    RENDERER::BATCH::Instance instance = {0};
    
    instance.model = 
      MATH::Translate(rb->position.x, rb->position.y, 1) *
      MATH::Scale(PART_WIDTH, PART_HEIGHT, 1);
    
    instance.color = (u32)~0;
    instance.diffuseLayer = 3;

    if(!RENDERER::BATCH::PushInstance(batch, instance))
      batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
  }
  RENDERER::BATCH::Flush(&renderer.batchRenderer);
  { //Draw Walls
    offscreenState.material.diffuseTexture = cardTexture;
    batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
    RENDERER::BATCH::Instance instance = {0};

    instance.color = (u32)~0;
    instance.diffuseLayer = 0;

    //////////////////////////////////////////////////
    //Left
    instance.model = 
      MATH::Translate(0, offscreenState.renderTarget.height / 2, 1) * 
      MATH::Scale(WALL_SIZE, offscreenState.renderTarget.height, 1);
    
    if(!RENDERER::BATCH::PushInstance(batch, instance))
      batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);

    //////////////////////////////////////////////////
    //Right 
    instance.model = 
      MATH::Translate(offscreenState.renderTarget.width, offscreenState.renderTarget.height / 2, 1) * 
      MATH::Scale(WALL_SIZE, offscreenState.renderTarget.height, 1);
    
    if(!RENDERER::BATCH::PushInstance(batch, instance))
      batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
 
    //////////////////////////////////////////////////
    //Bottom
    instance.model = 
      MATH::Translate(offscreenState.renderTarget.width / 2.0f, 0, 1) * 
      MATH::Scale(offscreenState.renderTarget.width, WALL_SIZE, 1);
    
    if(!RENDERER::BATCH::PushInstance(batch, instance))
      batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
 
    
    //////////////////////////////////////////////////
    //Top
    instance.model = 
      MATH::Translate(offscreenState.renderTarget.width / 2.0f, offscreenState.renderTarget.height, 1) * 
      MATH::Scale(offscreenState.renderTarget.width, WALL_SIZE, 1);
    
    if(!RENDERER::BATCH::PushInstance(batch, instance))
      batch = RENDERER::BATCH::BeginBatch(&renderer.batchRenderer, "quad", offscreenState);
 
  }
  


  
  for(u32 i = 0; i < core.player.parts.count; i++){
    auto part = core.player.parts[i];  
    DrawParticleEmitter(&part->particleEmitter);
  }
  DrawParticleEmitter(&core.windParticles);

  if(core.gameOver){
    //////////////////////////////////////////////////
    offscreenState.material = fontMaterial;
    offscreenState.stateBits = 0;
    f32 scale0 = 3;
    
    u32 color0 = 0xffffffff;
    f32 shadowPad = 9;
  
  
    f32 scale1 = 1;
    u32 color1 = 0xffffffff;
    
    const char* title0 = "GAME OVER";
    const char* title1 = "Press 'Q' to go to menu";
    char title3[256];
    sprintf(title3, "You managed to survive %.2f seconds", core.gameState.gameOverTime);
  
      
    RENDERER::IM::Immediate_Begin(&renderer.imRenderer, RENDERER::TOPOLOGY_TRIANGLES, offscreenState);

    f32 cx = offscreenState.renderTarget.width / 2;
    f32 cy = offscreenState.renderTarget.height / 2;

    MATH::Vector2<f32> textSize = GetTextSize(title0,
                                              scale0,
                                              &titleFont);

    cy += textSize.y;
      
    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 cx - textSize.x / 2 + shadowPad,
                                 cy - textSize.y / 2 - shadowPad,                                 
                                 scale0,
                                 0xff000000,
                                 title0,
                                 &titleFont);

    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 cx - textSize.x / 2,
                                 cy - textSize.y / 2,                                 
                                 scale0,
                                 color0,
                                 title0,
                                 &titleFont);

    
    cy -= textSize.y + 10;
    //////////////////////////////////////////////////////////////////////
    textSize = GetTextSize(title3,
                           scale1,
                           &titleFont);

    cy += textSize.y;
      
    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 cx - textSize.x / 2,
                                 cy - textSize.y / 2,                                 
                                 scale1,
                                 color1,
                                 title3,
                                 &titleFont);
    
    cy -= textSize.y + 10;
    //////////////////////////////////////////////////////////////////////
    textSize = GetTextSize(title1,
                           scale1,
                           &titleFont);


    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 cx - textSize.x / 2,
                                 cy - textSize.y / 2,                                 
                                 scale1,
                                 color1,
                                 title1,
                                 &titleFont);

    RENDERER::IM::Immediate_End(&renderer.imRenderer);
  }
}
//////////////////////////////////////////////////////////////////////
//Game
void DoPlayerControls(){
  RigidBody* player = GetRigidBodyWithID(&core.currentRoom->world,
                                         core.player.rigidBody_id);


  if(core.gameOver){
    if(IsKeyUp(&input, 'Q')){
      core.mode = PROGRAM_MODE_MENU;
    }
  }
  
  core.player.position = player->position;
  core.player.rotation = player->rotation;
  bool isKeyDowns[4];
  isKeyDowns[0] = IsKeyHeld(&input, 'W');
  isKeyDowns[1] = IsKeyHeld(&input, 'A');
  isKeyDowns[2] = IsKeyHeld(&input, 'S');
  isKeyDowns[3] = IsKeyHeld(&input, 'D');
  
  for(u32 i = 0; i < core.player.parts.count; i++){
    auto it = core.player.parts[i];
    it->timer += core.dt_sim;
    
    if(isKeyDowns[it->activationKeyIndex] && it->OnActivate && !core.gameOver){
      it->OnActivate(it, player);
      it->particleEmitter.paused = false;
    }else{
      it->particleEmitter.paused = true;
    }
  }
  
}
void DrawWindParticles(f32 dt){
  UpdateParticleEmitter(&core.windParticles, dt);
}
void DoGameLoopSim(){
  f32 dt = core.dt_sim;
  core.gameState.timer += dt;
  //////////////////////////////////////////////////
  
  DoPlayerControls();
   
  for(u32 i = 0; i < core.player.parts.count; i++){
    auto it = core.player.parts[i];
    f32 c = MATH::Cos(it->rotationToBody + core.player.rotation);
    f32 s = MATH::Sin(it->rotationToBody + core.player.rotation);
    it->particleEmitter.position = core.player.position;
    it->particleEmitter.position.x += c * it->distanceToBody;
    it->particleEmitter.position.y += s * it->distanceToBody;
    
    it->particleEmitter.rotation = core.player.rotation + it->rotation + MATH::PI;
    UpdateParticleEmitter(&it->particleEmitter, dt);
  }
  DrawWindParticles(dt);
}
