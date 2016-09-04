#include "WorldState.hh"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>

WorldState::WorldState(double height, double width)
  : height(height), width(width), num_updates(0),
    next_creature_id(0) { }

WorldState::~WorldState(){ }

void WorldState::AddCreature(std::function<CreatureAction(CurrentView)> mind,
                             GVector<2> pos) {
  creatures.emplace_back();
  CreaturePhysical& new_creature = creatures.back();
  minds[next_creature_id] = std::move(mind);

  new_creature.id = next_creature_id;
  new_creature.position = pos;
  new_creature.max_hp = 100;
  new_creature.hp = 100;
  new_creature.max_food = 100;
  new_creature.food = 100;
  new_creature.size = 20;

  next_creature_id++;

  signal_creature_born.Emit(new_creature);
}

void WorldState::Update(){
  auto actions = GetAllActions();
  assert(actions.size() == creatures.size());
  auto mating_pairs = FindMatingPairs(actions);
  DoSoloActions(actions);
  DoMovement(actions);
  MakeNewCreatures(mating_pairs);

  DecayFood();
  RemoveEmptyFood();
  RemoveDeadCreatures();
  SpawnFood();

  num_updates++;
}

CurrentView WorldState::GetViewFrom(const CreaturePhysical& viewer) const {
  CurrentView output;

  output.self = viewer.AsSelfView();

  for(const auto& other : creatures) {
    if(&other != &viewer) {
      GVector<2> disp = rel_position(viewer.position, other.position);
      if(disp.Mag() < viewer.ViewDistance()) {
        CreatureView view = other.AsView();
        view.rel_position = disp;
        output.creature_views.push_back(view);
      }
    }
  }

  for(const auto& food : food_locations) {
    GVector<2> disp = rel_position(viewer.position, food.position);
    if(disp.Mag() < viewer.ViewDistance()) {
      FoodView view = food.AsView();
      view.rel_position = disp;
      output.food_views.push_back(view);
    }
  }

  return output;
}

std::vector<CreatureAction> WorldState::GetAllActions() {
  std::vector<CreatureAction> output;

  for(const auto& creature : creatures) {
    auto view = GetViewFrom(creature);
    auto& mind = minds.at(creature.id);
    output.push_back(mind(view));
  }

  return output;
}

void WorldState::DoSoloActions(std::vector<CreatureAction>& actions) {
  // All solo actions
  for(unsigned int i=0; i<actions.size(); i++) {
    auto& creature = creatures[i];
    auto& action = actions[i];

    switch(action.action) {
      case CreatureAction::Attack:
        CreatureAttack(creature, action.direction);
        break;

      case CreatureAction::Eat:
        CreatureEat(creature);
        break;

      case CreatureAction::Sleep:
        CreatureSleep(creature);
        break;

      default:
        break;
    }
  }
}

void WorldState::DoMovement(std::vector<CreatureAction>& actions) {
  // Lastly, all movement.
  // Can't do this with the "solo" actions,
  //   because then creatures early in the order would have advantage on attacking,
  //   then moving out of the way.
  for(unsigned int i=0; i<actions.size(); i++) {
    auto& creature = creatures[i];
    auto& action = actions[i];
    if(action.action == CreatureAction::Move) {
      CreatureMove(creature, action.direction);
    }
  }
}

std::vector<std::pair<int,int> > WorldState::FindMatingPairs(std::vector<CreatureAction>& actions) {
  std::vector<std::pair<int,int> > output;

  for(unsigned int i=0; i<actions.size(); i++) {
    auto& creatureA = creatures[i];
    auto& actionA = actions[i];

    if(actionA.action == CreatureAction::Mate) {
      creatureA.hp -= creatureA.MatingHPCost();

      for(unsigned int j=i+1; j<actions.size(); j++) {
        auto& creatureB = creatures[j];
        auto& actionB = actions[j];

        if(actionB.action == CreatureAction::Mate) {
          auto targetA = creatureA.position + actionA.direction.UnitVector()*creatureA.MatingRange();
          auto targetB = creatureB.position + actionB.direction.UnitVector()*creatureB.MatingRange();
          if(rel_position(targetA, creatureB.position).Mag() < creatureA.MatingSize() &&
             rel_position(targetB, creatureA.position).Mag() < creatureB.MatingSize()) {

            output.push_back({i,j});
          }
        }
      }
    }
  }

  return output;
}

void WorldState::MakeNewCreatures(const std::vector<std::pair<int,int> >& mating_pairs) {
  for(const auto& pair : mating_pairs) {
    CreaturePhysical& a = creatures[pair.first];
    CreaturePhysical& b = creatures[pair.second];
    auto& mind_a = minds.at(a.id);
    auto& mind_b = minds.at(b.id);
    GVector<2> new_pos = apply_torus(a.position + rel_position(a.position,b.position)/2);

    // TODO: Make a mixed mind, consisting of some average of the two minds given.
    AddCreature(mind_a, new_pos);
  }
}

void WorldState::CreatureMove(CreaturePhysical& creature, GVector<2> direction) {
  creature.hp -= creature.MovementHPCost();
  if(direction.Mag() > creature.MaxMovement()) {
    direction *= (creature.MaxMovement()/direction.Mag());
  }
  creature.position = apply_torus(creature.position + direction);
}

void WorldState::CreatureAttack(CreaturePhysical& creature, GVector<2> direction) {
  auto target = GetAttackTarget(creature, direction);
  creature.hp -= creature.AttackHPCost();
  if(target) {
    target->hp -= creature.AttackDamage();
    signal_creature_attacked.Emit(creature, *target);
  }
}

void WorldState::CreatureEat(CreaturePhysical& creature) {
  auto foods = GetFoodTargets(creature);
  creature.hp -= creature.EatHPCost();
  for(auto& food : foods) {
    food->quantity -= creature.BiteSize();
    creature.food += creature.BiteSize();
    creature.food = std::min(creature.food, creature.max_food);

    signal_food_eaten.Emit(creature, *food);
  }
}

void WorldState::CreatureSleep(CreaturePhysical& creature) {
  double hp_recoverable = std::min(creature.food/creature.RecoveryEfficiency(),
                                   creature.RecoveryRate());
  creature.hp -= creature.SleepHPCost();
  creature.hp += hp_recoverable;
  creature.food -= hp_recoverable*creature.RecoveryEfficiency();
}

CreaturePhysical* WorldState::GetAttackTarget(CreaturePhysical& creature, GVector<2> direction) {
  CreaturePhysical* output = NULL;

  auto attack_dist = creature.size * creature.AttackRange();
  auto attack_center = (creature.position + direction.UnitVector()*attack_dist);
  attack_center = apply_torus(attack_center);

  double max_distance = creature.size * creature.AttackSize();

  for(auto& other : creatures) {
    auto disp = rel_position(attack_center, other.position);
    if(&creature != &other &&
       disp.Mag() < max_distance) {
      output = &other;
      max_distance = disp.Mag();
    }
  }

  return output;
}

std::vector<FoodLocation*> WorldState::GetFoodTargets(CreaturePhysical& creature) {
  std::vector<FoodLocation*> output;

  for(auto& food : food_locations) {
    auto disp = rel_position(creature.position, food.position);
    if(disp.Mag() < creature.size) {
      output.push_back(&food);
    }
  }

  return output;
}

void WorldState::RemoveEmptyFood() {
  for(auto& food : food_locations) {
    if(food.quantity <= 0) {
      signal_food_depleted.Emit(food);
    }
  }

  food_locations.erase(
    std::remove_if(food_locations.begin(), food_locations.end(),
                   [](const FoodLocation& f) { return f.quantity <= 0; }),
    food_locations.end()
  );
}

void WorldState::RemoveDeadCreatures() {
  for(auto& creature : creatures) {
    if(creature.AboutToDie()) {
      signal_creature_died.Emit(creature);
    }
  }

  creatures.erase(
    std::remove_if(creatures.begin(), creatures.end(),
                   [](const CreaturePhysical& c) { return c.AboutToDie(); }),
    creatures.end()
  );
}

void WorldState::DecayFood() {
  for(auto& food : food_locations) {
    food.quantity -= food.FoodDecay();
  }
}

void WorldState::SpawnFood() {
  double total_food = 0;
  for(auto& food : food_locations) {
    total_food += food.quantity;
  }

  // TODO, make this configurable somewhere.
  if(total_food < 300) {
    GVector<2> location(rand()*width/RAND_MAX, rand()*height/RAND_MAX);
    food_locations.emplace_back(location, 50);
    signal_food_spawned.Emit(food_locations.back());
  }
}

GVector<2> WorldState::rel_position(GVector<2> from, GVector<2> to) const {
  GVector<2> disp = to - from;
  return apply_torus_disp(disp);
}

GVector<2> WorldState::apply_torus(GVector<2> vec) const {
  vec.X() -= std::floor(vec.X()/width)*width;
  vec.Y() -= std::floor(vec.Y()/height)*height;
  return vec;
}

GVector<2> WorldState::apply_torus_disp(GVector<2> vec) const {
  GVector<2> offset(width/2, height/2);
  return apply_torus(vec+offset) - offset;
}

std::ostream& operator<<(std::ostream& os, const WorldState& world) {
  os << "Dimensions: " << world.width << "x" << world.height << "\n"
     << "Tick: " << world.num_updates;

  for(auto& creature : world.creatures) {
    os << "\n" << creature;
  }

  for(auto& food : world.food_locations) {
    os << "\n" << food;
  }

  return os;
}
