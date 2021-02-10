/*
 * sys_application_list.c
 *
 *  Created on: Aug 13, 2020
 *      Author: kimi
 */

/********** Include **********/

#include "common_type.h"

#include "sys_application_list.h"

#include "apl_opening.h"
#include "apl_launcher.h"
#include "apl_puzzle.h"
#include "apl_hockey.h"

/********** Define **********/

/********** Type **********/

/********** Constant **********/

const application_table_t ApplicationTable[APL_NUM] = {
		{AplOpeningInit, AplOpeningMain},
		{AplLauncherInit, AplLauncherMain},
		{AplPuzzleInit, AplPuzzleMain},
		{AplHockeyInit, AplHockeyMain}
};

/********** Variable **********/

/********** Function Prototype **********/

/********** Function **********/


