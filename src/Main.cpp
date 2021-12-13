//PLATFORM
#include "PLATFORM_Window.h"
#include "PLATFORM_FileSystem.h"
#include "PLATFORM_Timing.h"

//RENDERER
#include "RENDERER_Renderer.h"
//////////////////////////////////////////////////////////////////////
#include "Core.h"
#include "InputLayer.h"
#include "PhysicsWorld.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

//////////////////////////////////////////////////////////////////////
//Globals
PLATFORM::PlatWindow window;

RENDERER::Renderer     renderer;
RENDERER::RenderTarget mainRenderTarget;
RENDERER::RendererState offscreenState;

//Materials
RENDERER::Material     fontMaterial;
RENDERER::Material     colorMaterial;
RENDERER::Material     diffuseMaterial;


RENDERER::TextureArray cardTexture;
RENDERER::TextureArray playerTexture;
RENDERER::TextureArray particleTexture;



RENDERER::Font         titleFont;
Core core;
InputLayer input;

#include "DebugDraw.cpp" //Draw quadtree parititons and colliders
#include "BuildPart.cpp"
#include "Menu.cpp"
#include "Game.cpp"
#include "ParticleEmitter.cpp"


Room tempRoom;
//////////////////////////////////////////////////////////////////////
bool CreatePlatformWindow(PLATFORM::PlatWindow* window){
  return true; 
}

void InitMeshPrimitives(){
  RENDERER::Vertex quadV[] = {
    RENDERER::Vertex{-0.5f, -0.5f, 1, 0, 1, 0xffffffff},
    RENDERER::Vertex{-0.5f, 0.5f, 1, 0, 0, 0xffffffff},
    RENDERER::Vertex{0.5f,  0.5f, 1, 1, 0, 0xffffffff},
    RENDERER::Vertex{0.5f,  -0.5f, 1, 1, 1, 0xffffffff}};
  RENDERER::Index quadI[] = {0,1,2,0,2,3};
    
  RENDERER::Mesh mesh;
  mesh.vertexCount = sizeof(quadV) / sizeof(*quadV);
  mesh.vertices = quadV;

  mesh.indexCount = sizeof(quadI) / sizeof(*quadI);;
  mesh.indices = quadI;
  
  RENDERER::BATCH::AddMesh(&renderer.batchRenderer, "quad", mesh);  
}
void InitMaterials(){
  PLATFORM::FileContent frag = PLATFORM::ReadEntireFile("..\\data\\shaders\\MegaShader.frag.glsl",
                                                        PLATFORM::GetExecutableDir(), malloc, free);
  PLATFORM::FileContent vert = PLATFORM::ReadEntireFile("..\\data\\shaders\\MegaShader.vert.glsl",
                                                        PLATFORM::GetExecutableDir(), malloc, free);

  RENDERER::ShaderSource vertSource = RENDERER::ShaderSource{RENDERER::VERTEX_SHADER, (char*)vert.content};
  RENDERER::ShaderSource fragSource = RENDERER::ShaderSource{RENDERER::FRAGMENT_SHADER, (char*)frag.content};
  
  RENDERER::ShaderSources sources = {vertSource, fragSource};

  
  fontMaterial = RENDERER::CreateMaterial(RENDERER::MATERIAL_FLAGS_DIFFUSE_TEXTURE, sources);
  colorMaterial = RENDERER::CreateMaterial(0, sources);
  diffuseMaterial = RENDERER::CreateMaterial(RENDERER::MATERIAL_FLAGS_DIFFUSE_TEXTURE, sources);
  
  
  free(vert.content);
  free(frag.content);
}
void Init_Globals(){
  InitInputLayer(&input, malloc, free);

  RENDERER::BACKEND::Init(&renderer, {malloc, free});
  InitMeshPrimitives();

  InitMaterials();
  MATH::SetSeed(PLATFORM::GetCycleCounter());

  { //Offscreen rendertarget
    RENDERER::InitRenderTarget(&offscreenState.renderTarget, PART_WIDTH * 20, PART_HEIGHT * 20);
    offscreenState.projection = MATH::Ortho(0,
                                            offscreenState.renderTarget.width,
                                            offscreenState.renderTarget.height,
                                            0,
                                            0.1f,
                                            10000);
  }
  { //Title font
    auto fontFile = PLATFORM::ReadEntireFile("../data/font/DejaVuSansMono.ttf", PLATFORM::GetExecutableDir(), malloc, free);
    if(!RENDERER::InitFont(&titleFont, 32, fontFile.content, malloc, free)){
      printf("Failed to load title font\n");
    }
    fontMaterial.diffuseTexture = titleFont.texture;
    
    free(fontFile.content);
  }
  { //Card texture
    const char* fileNames[] = {
      "..\\data\\textures\\card_back.png",
      "..\\data\\textures\\card_lock.png",
      "..\\data\\textures\\card_W.png", // 2
      "..\\data\\textures\\card_A.png",
      "..\\data\\textures\\card_S.png",
      "..\\data\\textures\\card_D.png",
      "..\\data\\textures\\card_engine.png", // 6
      "..\\data\\textures\\card_gun.png",
      "..\\data\\textures\\card_mass.png",
     
    };
    i32 width, height, channels;      

    //Load first file to get texture dimensions
    auto textureFile = PLATFORM::ReadEntireFile(*fileNames, PLATFORM::GetExecutableDir(), malloc, free);
    auto pixelData =  stbi_load_from_memory((const stbi_uc*)textureFile.content, textureFile.size, &width, &height, &channels, 4);

    RENDERER::InitTextureArray(&cardTexture, width, height, sizeof(fileNames) / sizeof(*fileNames), 1, RENDERER::RGBA_8);
    RENDERER::BindTexture(cardTexture);
    RENDERER::SetTextureFilter(cardTexture, RENDERER::TEXTURE_FILTER_NEAREST);
    UpdateTextureData(cardTexture, width, height, 0, pixelData);
    
    stbi_image_free(pixelData);
    free(textureFile.content);
    
    for(u32 i = 1 ; i < sizeof(fileNames) / sizeof(*fileNames); i++){
      textureFile = PLATFORM::ReadEntireFile(fileNames[i], PLATFORM::GetExecutableDir(), malloc, free);

      pixelData =  stbi_load_from_memory((const stbi_uc*)textureFile.content, textureFile.size, &width, &height, &channels, 4);
      UpdateTextureData(cardTexture, width, height, i, pixelData);
 
      stbi_image_free(pixelData);
      free(textureFile.content);
    }
    colorMaterial.diffuseTexture = cardTexture;
  }
  { //Card texture
    const char* fileNames[] = {
      "..\\data\\textures\\part_engine.png",
      "..\\data\\textures\\part_gun.png",
      "..\\data\\textures\\part_mass.png",
      "..\\data\\textures\\bullet_0.png"  
    };
    
    i32 width, height, channels;      
    //Load first file to get texture dimensions
    auto textureFile = PLATFORM::ReadEntireFile(*fileNames, PLATFORM::GetExecutableDir(), malloc, free);
    auto pixelData =  stbi_load_from_memory((const stbi_uc*)textureFile.content, textureFile.size, &width, &height, &channels, 4);

    RENDERER::InitTextureArray(&playerTexture, width, height, sizeof(fileNames) / sizeof(*fileNames), 1, RENDERER::RGBA_8);
    RENDERER::BindTexture(playerTexture);
    RENDERER::SetTextureFilter(playerTexture, RENDERER::TEXTURE_FILTER_NEAREST);

    UpdateTextureData(playerTexture, width, height, 0, pixelData);
    
    stbi_image_free(pixelData);
    free(textureFile.content);
    
    for(u32 i = 1 ; i < sizeof(fileNames) / sizeof(*fileNames); i++){
      
      textureFile = PLATFORM::ReadEntireFile(fileNames[i], PLATFORM::GetExecutableDir(), malloc, free);
      pixelData =  stbi_load_from_memory((const stbi_uc*)textureFile.content, textureFile.size, &width, &height, &channels, 4);
      UpdateTextureData(playerTexture, width, height, i, pixelData);
 
      stbi_image_free(pixelData);
      free(textureFile.content);
    }
    colorMaterial.diffuseTexture = playerTexture;
  }
  {
    { //Particle texture
      const char* fileNames[] = {
        "..\\data\\textures\\particle_0.png"
      };
    
      i32 width, height, channels;      
      //Load first file to get texture dimensions
      auto textureFile = PLATFORM::ReadEntireFile(*fileNames, PLATFORM::GetExecutableDir(), malloc, free);
      auto pixelData =  stbi_load_from_memory((const stbi_uc*)textureFile.content, textureFile.size, &width, &height, &channels, 4);

      RENDERER::InitTextureArray(&particleTexture, width, height, sizeof(fileNames) / sizeof(*fileNames), 1, RENDERER::RGBA_8);
      RENDERER::BindTexture(particleTexture);
      RENDERER::SetTextureFilter(particleTexture, RENDERER::TEXTURE_FILTER_NEAREST);
      UpdateTextureData(particleTexture, width, height, 0, pixelData);
    
      stbi_image_free(pixelData);
      free(textureFile.content);
    
      for(u32 i = 1 ; i < sizeof(fileNames) / sizeof(*fileNames); i++){
        textureFile = PLATFORM::ReadEntireFile(fileNames[i], PLATFORM::GetExecutableDir(), malloc, free);

        pixelData =  stbi_load_from_memory((const stbi_uc*)textureFile.content, textureFile.size, &width, &height, &channels, 4);
        UpdateTextureData(particleTexture, width, height, i, pixelData);
 
        stbi_image_free(pixelData);
        free(textureFile.content);
      }
      colorMaterial.diffuseTexture = particleTexture;
    }
  }
};

void Simulate(){
  if(core.mode == PROGRAM_MODE_MENU){
    UpdateMainMenu();
  }else if(core.mode == PROGRAM_MODE_GAME){
    if(core.gameState.mode != BUILD_STAGE_END){
      DoBuildStageSim(); 
    }else{
      DoGameLoopSim();

      f32 dt = core.dt_sim;
      PhysicsUpdate(&core.currentRoom->world, dt);

      for(u32 i = 0; i < core.currentRoom->world.triggerEvents.count; i++){
        PhysicsWorldEvent e = *core.currentRoom->world.triggerEvents[i];
        if(e.rigidBody_id_b == 0){
          RemoveRigidBody(&core.currentRoom->world, e.rigidBody_id_a);
        }
      }
      for(u32 i = 0; i < core.currentRoom->world.collisionEvents.count; i++){
        PhysicsWorldEvent e = *core.currentRoom->world.collisionEvents[i];
        if(e.rigidBody_id_a == core.player.rigidBody_id && !
           core.gameOver){
          
          core.gameOver = true;
          core.gameState.gameOverTime = core.gameState.timer;
          
        }
      }      
      //Handle triggers and collisions
      for(u32 i = 0; i < core.bullets.count; i++){
        auto bullet = core.bullets[i];
        bullet->lifeTime += dt;

        if(bullet->lifeTime > 5 || !GetRigidBodyWithID(&core.currentRoom->world,
                                                      bullet->rigidBody_id)){
          RemoveRigidBody(&core.currentRoom->world, bullet->rigidBody_id);
          RemoveCollider(&core.currentRoom->world, bullet->collider_id);
          core.bullets.Remove(i);
          i--;
        }
      }

      ResetPerFrame(&core.currentRoom->world);
      

      if(IsKeyUp(&input, 'C')){
        core.debugDraw = !core.debugDraw;
      }
    }
  }
};
void Draw(){
  if(core.mode == PROGRAM_MODE_MENU){
    DrawMainMenu();
  }else if(core.mode == PROGRAM_MODE_GAME){
    if(core.gameState.mode != BUILD_STAGE_END){
      DrawBuildStage();
    }else{
      //Debug draw physics world
      if(core.debugDraw){
        offscreenState.material = colorMaterial;
        DrawQuadTreeSize(&core.currentRoom->world);
        DrawQuadTree(&core.currentRoom->world); 
      }

      DrawGame();
      
      RENDERER::BATCH::Flush(&renderer.batchRenderer);
      if(core.debugDraw){ 
        DrawColliders(&core.currentRoom->world);
      }
    }
  }
  
}
void UpdateFPS(u64 &elapsedTime, u32 &frameCount){
  frameCount ++;
  if(elapsedTime >= PLATFORM::MICRO_SECONDS_IN_A_SECOND){
    elapsedTime -= PLATFORM::MICRO_SECONDS_IN_A_SECOND;
    //printf("%d FPS\n", frameCount);
    frameCount = 0;
  }
}
int main(){
  if(!PLATFORM::Create_Window("Pimiät Pelijamit 2021 Henrik Peteri", CreatePlatformWindow, &window)){
    return 1;
  }  
  if(!Create_GLContext(&window))
    return 1;

  PLATFORM::Init_Time();

  Init_Globals();
  
  core.currentRoom = &tempRoom;

  core.buildPartCards.Init(malloc, free);
  core.bullets.Init(malloc, free);
  core.player.parts.Init(malloc, free);

  
  InitPhysicsWorld(&core.currentRoom->world,
                   0, 0,
                   offscreenState.renderTarget.width,
                   offscreenState.renderTarget.height,
                   malloc, free);
  
  Rectangle rect;
  rect.p0 = {0, 0};
  rect.size.x = 20;
  rect.size.y = 50;

  {
    //Bottom
    //These need to be static rigid bodies
    rect.p0.x = offscreenState.renderTarget.width / 2; 
    rect.size.y = WALL_SIZE;
    rect.size.x = offscreenState.renderTarget.width;
    CreateCollider(&core.currentRoom->world, 0, rect);

    //Top
    rect.p0.y = offscreenState.renderTarget.height;
    CreateCollider(&core.currentRoom->world, 0, rect);

    rect.p0.y /= 2;
    rect.p0.x = 0;
    rect.size.x = WALL_SIZE;
    rect.size.y = offscreenState.renderTarget.height;
    CreateCollider(&core.currentRoom->world, 0, rect);

    rect.p0.x = offscreenState.renderTarget.width;
    rect.size.x = WALL_SIZE;
    rect.size.y = offscreenState.renderTarget.height;
    CreateCollider(&core.currentRoom->world, 0, rect);
  }
  
  u32 frameCount = 0;
  
  u64 startTime = PLATFORM::GetClockTime_MicroS();
  u64 elapsedTime = 0;
  
  
  bool running = true;
  while(running){
    UpdateFPS(elapsedTime, frameCount);

    if(core.mode == PROGRAM_MODE_SHOULD_QUIT){
      break;
    }
    
    window.UpdateEventBuffer();
    for(unsigned int i = 0; i < window.eventBuffer.count; i++){
      auto event = window.eventBuffer.events[i];
      switch(event.type){
      case(PLATFORM::EVENT_QUIT):
        {
          running = false;
          break;
        }
      case(PLATFORM::EVENT_SIZE):
        {
          
          mainRenderTarget.width = event.size.x;
          mainRenderTarget.height = event.size.y;
          break;
        }
      case(PLATFORM::EVENT_KEY_DOWN):
        {
          HandleVirtualKey(&input, event.key.keycode, true);
          break;
        }
      case(PLATFORM::EVENT_KEY_UP):
        {
          HandleVirtualKey(&input, event.key.keycode, false);
          break;
        }
      case(PLATFORM::EVENT_MOUSE_BUTTON_DOWN):
        {
          HandleVirtualKey(&input, event.key.keycode, true);
          break;
        }
      case(PLATFORM::EVENT_MOUSE_BUTTON_UP):
        {
          HandleVirtualKey(&input, event.key.keycode, false);
          break;
        }
      case(PLATFORM::EVENT_MOUSE_MOTION):
        {
          f32 offx = (f32)event.mouse.x;
          f32 offy = mainRenderTarget.height - (f32)event.mouse.y;

          input.input.mouse.x = offx;
          input.input.mouse.y = offy;

          f32 scale = MIN(mainRenderTarget.width / (f32)offscreenState.renderTarget.width,
                          mainRenderTarget.height / (f32)offscreenState.renderTarget.height);
          
          auto scaledWidth = offscreenState.renderTarget.width * scale;
          auto scaledHeight = offscreenState.renderTarget.height * scale;

          f32 x_pad = mainRenderTarget.width - scaledWidth;
          f32 y_pad = mainRenderTarget.height - scaledHeight;

          offx -= x_pad / 2;
          offy -= y_pad / 2;

          
          core.gameState.mouseLocalSpace.x = (offx / scaledWidth) * offscreenState.renderTarget.width;
          core.gameState.mouseLocalSpace.y = (offy / scaledHeight) * offscreenState.renderTarget.height;
          //////////////////////////////////////////////////
          
          break;
        }
      default:
        break;
      };
    }
    if(!running) break;
    //////////////////////////////////////////////////////////////////////
    //Sim

    Simulate();
    
    //////////////////////////////////////////////////////////////////////
    //Draw
    
    RENDERER::SetClearColor(0, 0, 0, 1);
    RENDERER::SetRenderTarget(mainRenderTarget);
    RENDERER::ClearRenderTarget();

    RENDERER::SetRenderTarget(offscreenState.renderTarget);
    RENDERER::SetClearColor(0xff / 255.0f, 0xae / 255.0f, 0xb9 / 255.0f, 1);
    RENDERER::ClearRenderTarget();


    Draw();
    
    RENDERER::IM::Immediate_Flush(&renderer.imRenderer);
    RENDERER::BATCH::Flush(&renderer.batchRenderer);

    
    
    {//Blit to middle to screen
      
      f32 middle_x = mainRenderTarget.width / 2.0f;
      f32 middle_y = mainRenderTarget.height / 2.0f;

      f32 scale = MIN(mainRenderTarget.width / (f32)offscreenState.renderTarget.width,
                      mainRenderTarget.height / (f32)offscreenState.renderTarget.height);

      auto scaledHalfWidth = offscreenState.renderTarget.width * scale / 2;
      auto scaledHalfHeight = offscreenState.renderTarget.height * scale / 2;

      RENDERER::BlitRenderTarget(offscreenState.renderTarget,
                                 0.0f, 0.0f,
                                 offscreenState.renderTarget.width,
                                 offscreenState.renderTarget.height,
                                 mainRenderTarget,
                                 middle_x - scaledHalfWidth,
                                 middle_y - scaledHalfHeight,
                                 middle_x + scaledHalfWidth,
                                 middle_y + scaledHalfHeight,
                                 RENDERER::COLOR_BUFFER_BIT);
    }
    
    PLATFORM::SwapWindowBuffers(&window);
    //End of frame    
    UpdateInput_EndOfFrame(&input);

    
    //////////////////////////////////////////////////////////////////////
    u64 endTime = PLATFORM::GetClockTime_MicroS();
    
    core.dt_visual = core.dt_sim = PLATFORM::MS_TO_SEC(endTime - startTime);
    //core.dt_visual = core.dt_sim = 0.016f;
    elapsedTime  += (endTime - startTime);
    startTime = endTime;
    //////////////////////////////////////////////////////////////////////
  }
  PLATFORM::Destroy_Window(&window);
  return 0; 
}
