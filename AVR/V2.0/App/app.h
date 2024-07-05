

#ifndef __APP_H_
#define __APP_H_

#include "main.h"

#define TASKS_MAX		4			// ��������


//������ȣ���־λ
typedef struct {
	uint8_t m_runFlag;				/*!<�������б��:0-�����У�1���� */
	uint16_t m_timer;				/*!< ��ʱ�� */
	uint16_t m_itvTime;				/*!<���������Ÿ�ʱ�� */
	void(*m_pTaskHook)(void);		/*!<Ҫ���е������� */
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
