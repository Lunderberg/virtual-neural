#include "SimpleMinds.hh"

#include <iostream>

#include "GVector.hh"

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

  bool food_is_nearby = view.food_views.size();
  if(!food_is_nearby) {
    return {CreatureAction::Move, GVector<2>(1,1) };
  }

  GVector<2> closest_food(1e100,1e100);
  for(auto& food : view.food_views) {
    if(food.rel_position.Mag() < closest_food.Mag()) {
      closest_food = food.rel_position;
    }
  }

  if(closest_food.Mag() > view.self.size) {
    return {CreatureAction::Move, closest_food};
  } else if (view.self.food < 0.95*view.self.max_food) {
    return CreatureAction::Eat;
  } else {
    return CreatureAction::Sleep;
  }
}
