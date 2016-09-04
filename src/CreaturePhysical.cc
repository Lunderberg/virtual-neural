#include "CreaturePhysical.hh"

CreatureView CreaturePhysical::AsView() const {
  CreatureView output;

  output.max_hp = max_hp;
  output.hp = hp;
  output.max_food = max_food;
  output.food = food;
  output.size = size;

  return output;
}

CreatureSelfView CreaturePhysical::AsSelfView() const {
  CreatureSelfView output;

  output.max_hp = max_hp;
  output.hp = hp;
  output.max_food = max_food;
  output.food = food;
  output.size = size;

  return output;
}

std::ostream& operator<<(std::ostream& os, const CreaturePhysical& creature) {
  os << "Creature #" << creature.id << "\n"
     << "\tLocation: " << creature.position << "\n"
     << "\tHP: " << creature.hp << "/" << creature.max_hp << "\n"
     << "\tFood: " << creature.food << "/" << creature.max_food << "\n"
     << "\tSize: " << creature.size;
  return os;
}
