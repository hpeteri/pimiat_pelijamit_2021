#include "BuildPart.h"
#include "Math_Random.h"


BuildPartCard RollNewBuildPartCard(f32 x0, f32 y0, f32 x1, f32 y1){
  BuildPartCard card;
  memset(&card, 0, sizeof(card));

  //Roll part position and rotation
  card.position.x = MATH::RandomInRange_f32(x0, x1);
  card.position.y = MATH::RandomInRange_f32(y0, y1);
  card.rotation = MATH::RandomInRange_f32(0, MATH::PI * 2);

  card.mass = 1;
  card.type = (PART_TYPE)MATH::RandomInRange_i32(PART_TYPE_ENGINE, PART_TYPE_COUNT - 1);
  card.binding = (PART_BINDING)MATH::RandomInRange_i32(PART_BINDING_W, PART_BINDING_COUNT - 1);
  return card;
}
