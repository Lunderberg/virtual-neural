#ifndef _CREATUREACTION_H_
#define _CREATUREACTION_H_

#include "GVector.hh"

struct CreatureAction {
  enum Action { Move, Attack, Mate, Eat, Sleep };

  CreatureAction(Action action, GVector<2> direction={0,0})
    : action(action), direction(direction) { }

  Action action;
  GVector<2> direction;
};

#endif /* _CREATUREACTION_H_ */
