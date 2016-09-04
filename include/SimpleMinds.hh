#ifndef _SIMPLEMINDS_H_
#define _SIMPLEMINDS_H_

#include "CreatureAction.hh"
#include "CurrentView.hh"

CreatureAction sleep_silently(CurrentView view);
CreatureAction run_right(CurrentView view);
CreatureAction run_left(CurrentView view);
CreatureAction run_and_eat(CurrentView view);
CreatureAction run_eat_kill(CurrentView view);

#endif /* _SIMPLEMINDS_H_ */
