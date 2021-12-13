void DrawQuadTreePartition(QuadTreePartition<u64>* partition){
  if(partition->isPartitioned){
    DrawQuadTreePartition(partition->tl);
    DrawQuadTreePartition(partition->tr);
    DrawQuadTreePartition(partition->bl);
    DrawQuadTreePartition(partition->br);  
  }else{
    const f32 lineWidth = 1;
    const u32 color = (u32)~0;
  
    auto min = partition->min;
    auto max = partition->max;

    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 min.x, min.y,
                                 min.x, max.y,
                                 lineWidth, color);
    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 min.x, max.y,
                                 max.x, max.y,
                                 lineWidth, color);
  
    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 max.x, max.y,
                                 max.x, min.y,
                                 lineWidth, color);
  
    RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                                 max.x, min.y,
                                 min.x, min.y,
                                 lineWidth, color);
  }
}
void DrawQuadTreePartitionSize(QuadTreePartition<u64>* partition){
  if(partition->isPartitioned){
    DrawQuadTreePartitionSize(partition->tl);
    DrawQuadTreePartitionSize(partition->tr);
    DrawQuadTreePartitionSize(partition->bl);
    DrawQuadTreePartitionSize(partition->br);  
  }else{
    const f32 lineWidth = 1;
    const u32 color = (u32)~0;
  
    auto min = partition->min;
    auto max = partition->max;
    char buffer[100];
    sprintf(buffer, "%d", partition->container.count);
    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 min.x, min.y,
                                 1,
                                 0xff000000,
                                 buffer,
                                 &titleFont);
  }
}
void DrawQuadTreeSize(PhysicsWorld* world){
  offscreenState.renderTarget = offscreenState.renderTarget;
  offscreenState.material = fontMaterial;
  offscreenState.stateBits = RENDERER::BLEND_BIT;
  RENDERER::IM::Immediate_Begin(&renderer.imRenderer,
                                RENDERER::TOPOLOGY::TOPOLOGY_TRIANGLES,
                                offscreenState); 
  DrawQuadTreePartitionSize(world->quadTree.root);
  
  //physicsWorld
  RENDERER::IM::Immediate_End(&renderer.imRenderer);
}
void DrawQuadTree(PhysicsWorld* world){
  offscreenState.renderTarget = offscreenState.renderTarget;
  offscreenState.material = colorMaterial;
  offscreenState.stateBits = 0;

  RENDERER::IM::Immediate_Begin(&renderer.imRenderer,
                                RENDERER::TOPOLOGY::TOPOLOGY_TRIANGLES,
                                offscreenState);
  
  DrawQuadTreePartition(world->quadTree.root);
  
  //physicsWorld
  RENDERER::IM::Immediate_End(&renderer.imRenderer);
}

void DrawColliderAABB(MATH::Vector2<f32> min, MATH::Vector2<f32> max){
  const u32 color = 0xffff0055;
  const u32 lineWidth = 1;
  
  RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                               min.x, min.y,
                               min.x, max.y,
                               lineWidth, color);
  
  RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                               min.x, max.y,
                               max.x, max.y,
                               lineWidth, color);
  
  RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                               max.x, max.y,
                               max.x, min.y,
                               lineWidth, color);
  
  RENDERER::IM::Immediate_Line(&renderer.imRenderer,
                               max.x, min.y,
                               min.x, min.y,
                               lineWidth, color);
  
}
void DrawColliders(PhysicsWorld* world){
  
  offscreenState.material = colorMaterial;
  offscreenState.stateBits = 0;
  const u32 color = 0xff00ff00;
  const u32 triggerColor = 0xff0000ff;
  const u32 lineWidth = 3;
  
  RENDERER::IM::Immediate_Begin(&renderer.imRenderer,
                                RENDERER::TOPOLOGY::TOPOLOGY_TRIANGLES,
                                offscreenState);
  
  
  for(u32 i = 0; i < world->circleColliders.count; i++){
    auto it = world->circleColliders[i];
    auto offset = MATH::RotateZ(it->collider.p0, it->rotation);
    DrawColliderAABB(it->aabb.min, it->aabb.max);
    RENDERER::IM::Immediate_Circle_Outline(&renderer.imRenderer,
                                           it->position.x + offset.x,
                                           it->position.y + offset.y,
                                           it->collider.r,
                                           lineWidth,
                                           it->flags & IS_TRIGGER ? triggerColor : color);

  }
  
  MATH::Vector2<f32> points[4];
  
  for(u32 i = 0; i < world->boxColliders.count; i++){
    auto it = world->boxColliders[i];
    DrawColliderAABB(it->aabb.min, it->aabb.max);
    GetGlobalPoints(it->collider, points, it->position, it->rotation);
    RENDERER::IM::Immediate_Outline(&renderer.imRenderer,
                                    4, points,
                                    lineWidth,
                                    it->flags & IS_TRIGGER ? triggerColor : color);
    /*
    RENDERER::IM::Immediate_Circle_Outline(&renderer.imRenderer,
                                           it->position.x + it->collider.p0.x,
                                           it->position.y + it->collider.p0.y,
                                           it->collider.r,
                                           1,
                                           (u32)~0);
    */
  }  
  RENDERER::IM::Immediate_End(&renderer.imRenderer);

  offscreenState.material = fontMaterial;
  offscreenState.stateBits = RENDERER::BLEND_BIT;
      
  RENDERER::IM::Immediate_Begin(&renderer.imRenderer,
                                RENDERER::TOPOLOGY::TOPOLOGY_TRIANGLES,
                                offscreenState);

  for(u32 i = 0; i < world->boxColliders.count; i++){
    auto it = world->boxColliders[i];
    char buffer[256];
    sprintf(buffer, "%lld", it->id);
    RENDERER::IM::Immediate_Text(&renderer.imRenderer,
                                 it->aabb.min.x,
                                 it->aabb.min.y,
                                 0.25f,
                                 color,
                                 buffer,
                                 &titleFont);
  }
  RENDERER::IM::Immediate_End(&renderer.imRenderer);
}
