#ifndef TASKLISTENER_H
#define TASKLISTENER_H

#include "TaskList.h"
#include "Logger.h"

/*
	ѕоток запускаетс€ параллельно основному
	“ут происходит проверка, и установка статусов задачам
	 - «апуск на выполнение
	 - —мена статуса 
	 - ѕодготовка ответа клиенту
	 по завершению, задача не удал€етс€, а переходит в статус: FREE
	 задачи со статусом: FREE, переиспользуютс€ по новой  

*/

class TaskListener: public TaskListAttachBase
{
	private:
		std::thread thTaskListener;
		std::mutex CurTaskMutex;	
		void StartListening();
	
	public:
		void thStartListening()  {
			thTaskListener = std::thread(&TaskListener::StartListening, this); 
		}	
		
		void thWaitForCompletion() {
			if (thTaskListener.joinable()) {
				thTaskListener.join(); 
			}
		}		
		
		~TaskListener() {
			thWaitForCompletion(); 
		}	
};

#endif
