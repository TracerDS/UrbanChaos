//	EnemySetup.h
//	Guy Simmons, 23rd August 1998.

#ifndef	ENEMYSETUP_H
#define	ENEMYSETUP_H

#include	"Mission.h"

//---------------------------------------------------------------

void	do_enemy_setup(EventPoint *the_ep, bool do_adjust=false);
CBYTE	*get_enemy_message(EventPoint *ep, CBYTE *msg);

//---------------------------------------------------------------

#endif
