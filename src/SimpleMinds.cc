#include "SimpleMinds.hh"

#include <cmath>

#include "GVector.hh"

CreatureAction sleep_silently(CurrentView /*view*/) {
  return CreatureAction::Sleep;
}

CreatureAction run_right(CurrentView /*view*/) {
  return {CreatureAction::Move, GVector<2>(1,0) };
}

CreatureAction run_left(CurrentView /*view*/) {
  return {CreatureAction::Move, GVector<2>(-1,0) };
}

CreatureAction run_and_eat(CurrentView view) {
  if(view.self.food>0 && view.self.hp < view.self.max_hp/2) {
    return CreatureAction::Sleep;
  }

  auto closest_food = view.ClosestFood();

  if(std::isnan(closest_food.X())) {
    return {CreatureAction::Move, GVector<2>(1,1) };
  } else if(closest_food.Mag() > view.self.size) {
    return {CreatureAction::Move, closest_food};
  } else if (view.self.food < 0.95*view.self.max_food) {
    return CreatureAction::Eat;
  } else {
    return CreatureAction::Sleep;
  }
}

CreatureAction run_eat_kill(CurrentView view) {
  if(view.self.food>0 && view.self.hp < view.self.max_hp/2) {
    return CreatureAction::Sleep;
  }

  auto closest_creature = view.ClosestCreature();
  auto closest_food = view.ClosestFood();

  // If something is within range, move to it and attack
  if(closest_creature.Mag() < view.self.size*1.3) {
    return {CreatureAction::Attack, closest_creature};
  } else if (closest_creature.Mag() > view.self.size*1.3) {
    return {CreatureAction::Move, closest_creature};
  }

  // If food within range, move to it and eat
  if(std::isnan(closest_food.X())) {
    return {CreatureAction::Move, GVector<2>(1,-1) };
  } else if(closest_food.Mag() > view.self.size) {
    return {CreatureAction::Move, closest_food};
  } else if (view.self.food < 0.95*view.self.max_food) {
    return CreatureAction::Eat;
  } else {
    return CreatureAction::Sleep;
  }
}
