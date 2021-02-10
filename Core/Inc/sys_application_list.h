/*
 * sys_application_list.h
 *
 *  Created on: Aug 13, 2020
 *      Author: kimi
 */

#ifndef INC_SYS_APPLICATION_LIST_H_
#define INC_SYS_APPLICATION_LIST_H_

/********** Include **********/

/********** Define **********/

enum applicationIdType {
	APL_OPENING = 0,
	APL_LAUNCHER,
	APL_PUZZLE,
	APL_HOCKEY,
	APL_NUM
};

/********** Type **********/

typedef void (*init_func_ptr_t)(void);
typedef void (*main_func_ptr_t)(void);

typedef struct applicationTableType {
	init_func_ptr_t init_func;
	main_func_ptr_t main_func;
} application_table_t;

/********** Constant **********/

extern const application_table_t ApplicationTable[APL_NUM];

/********** Variable **********/

/********** Function Prototype **********/


#endif /* INC_SYS_APPLICATION_LIST_H_ */
