

#ifndef __APP_H_
#define __APP_H_

#include "main.h"

#define TASKS_MAX		4			// 任务数量


//任务调度，标志位
typedef struct {
	uint8_t m_runFlag;				/*!<程序运行标记:0-不运行，1运行 */
	uint16_t m_timer;				/*!< 计时器 */
	uint16_t m_itvTime;				/*!<任务运行闻隔时间 */
	void(*m_pTaskHook)(void);		/*!<要运行的任务函数 */
}_TASK_InfoType;

void avr_app(void);

typedef enum {

	LEVEL0,
	LEVEL1,
	LEVEL2,
	LEVEL3,
	LEVEL4,
	LEVEL5,
	LEVEL6,
	LEVEL7,
	LEVEL8,
	LEVEL9,
	LEVEL10,
	LEVEL11
}LEVEL;

void TASK_Process(void);
void TASK_Remarks(void);
void Init_Task(void);
#endif
