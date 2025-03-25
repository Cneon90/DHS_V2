#include "TaskExecutor.h"

#define TIME_DELETE_TASK		3


[[noreturn]]
void TaskListener::StartListening()
{
	for(;;) {
		std::unique_lock<std::mutex> lock(CurTaskMutex); 
		vPortSleepMs(100); /*20*/
		
		
		TasksList* tasks = getTasksList();    // Плучаем список всех задач 
		if(tasks == nullptr) { 
			Logger::log(Logger::LogLevel::log_ERROR, "EXECUTOR | TASKS LIST NULL");
			continue; 
		}	
		
		size_t TaskCount = tasks->getCount(); // Получаем количество задач
		if(TaskCount <= 0  ) {
//		    Logger::log(Logger::LogLevel::log_INFO, "tasks count = 0");
			continue; 
		}
	
		// Execute task
		for(int i=0; i<TaskCount; ++i) {
//			vPortSleepMs(5);/*50*/
			try {
				TaskaBase* task = tasks->getTaskID(i); 

				if(task == nullptr) { 
					Logger::log(Logger::LogLevel::log_ERROR, "EXECUTOR |Nullptr task %d", i);
					continue; 
				}
				
				TaskState current_state = task->getState(); 				
//				bool _completed = task->getCompleted();
//				bool _sendRes   = task->getSendResult();				
			    int current_taskID = task->getID();	
				if(current_taskID <= 0) { 
					Logger::log(Logger::LogLevel::log_ERROR, "EXECUTOR |Task ID = 0 (num = %d)", i);
					continue; 
				}
				
				if(current_state == ts_Wait)   
				{ // WAIT
//					vPortSleepMs(50);
//					task->setProcessed();
//					Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |WAIT| task %d, client - %d", i, task->getID());
					task->startThread();
//					continue;
					task->setState(ts_Process);
//					vPortSleepMs(50);

				} else if(current_state == ts_Process) 
				{  // Process;
//					vPortSleepMs(50);
//					Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |PROCESS| task %d, client - %d", i, current_taskID);
					
					/*Ждем когда задача поменяет статус (меняет статус в Task_base::prepareResponse() */
//					if(_completed) {
//						task->setState(ts_Ready);
//						continue;	
//					}
//					vPortSleepMs(50);
					
				} else if(current_state == ts_Ready) 
				{ // Ready 
//					vPortSleepMs(50);
//					Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |READY| task %d, client - %d", i, current_taskID);
//					if(_sendRes) {
//						task->setState(ts_Success);
//					}
					
					/* Добавить таймер, если долго нет результата, значит он уже никому не нужен, можно завершить задачу*/ 
					uint64_t TimeCurrent = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
//					Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |DELETE FOR %d SECOND | task %d, client - %d", TimeCurrent - task->getTimeStart() , i, current_taskID);
					if (TimeCurrent - task->getTimeStart() >= TIME_DELETE_TASK  ) {
						Logger::log(Logger::LogLevel::log_ERROR, "EXECUTOR |READY| task %d, client - %d | TIMEOUT" , task->getID());
						task->setState(ts_Success);
//						continue;
					}	
					
					
//					vPortSleepMs(50);
				} else if( current_state == ts_Success ) 
				{ // SUCCESS
//					vPortSleepMs(50);
//					Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |SUCCESS| Mark delete task %d, client - %d", i, current_taskID);
					task->setState(ts_Used);
//					vPortSleepMs(50);
				} 
				else if ( current_state == ts_Used ) 
				{ // USED (wait)
				
					if(! task->IsTaskCompleted()) {
//						Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |USED - PROCESS | task %d, client - %d", i, current_taskID);	
					} else { 
//						Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |USED - WAIT | task %d, client - %d", i, current_taskID);	
						task->setState(ts_free);
//						task->task_reset();	
					}
				} else if ( current_state == ts_free) 
				{// FREE (ready for reuse)
//					Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |FREE | task %d, client - %d", i, current_taskID);		 
				 
				}
				
//				Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |Done task #%d state %d", i, task->getState());
				
			} catch(...) { 
				Logger::log(Logger::LogLevel::log_ERROR, "EXECUTOR |Done task #%d", i);	
			}
			
//			Logger::log(Logger::LogLevel::log_DEBUG, "Execute task #%d END");	
		}
//		Logger::log(Logger::LogLevel::log_DEBUG, "EXECUTOR |Total tasks: %d", tasks->getCount());		    
	}
	
	Logger::log(Logger::LogLevel::log_ERROR, "EXECUTOR |thead STOP!!");
	
//	thWaitForCompletion();	
}
