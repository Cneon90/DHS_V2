#include "TaskList.h"

int TasksList::getCount() { 
	std::unique_lock<std::mutex> lock(tasksMutex); // ��������� �������	
	return TaskList.size(); 
}  

// ---- TasksList ---------------------------------------------------
void TasksList::RemoveFromStack(int _index) { 
	try{
		std::unique_lock<std::mutex> lock(tasksMutex); // ������ � ������� ��������
    if (_index >= 0 && _index < TaskList.size()) { 
//            delete TaskList[_index]; 
		try {
			TaskList.erase(TaskList.begin() + _index); 	
		} catch (...) {
			printf("*** Error delete ****");
		}
        
    } else { printf("Error: Index out of range!"); }
	} catch(...) { 
	
	}	
}
	
	
TaskaBase* TasksList::getTaskID(int _index) 
{ 
	try{
		std::unique_lock<std::mutex> lock(tasksMutex);  
	    if (_index >= 0 && _index < TaskList.size()) { 
//			Logger::log(Logger::LogLevel::log_DEBUG, "GET Task INDEX %d", _index );
			return TaskList.at(_index);
	    }
	} catch(...) {
	    Logger::log(Logger::LogLevel::log_ERROR, "GET Task INDEX %d", _index );
		return nullptr;	
	}
	
    return nullptr; 
}

// Add task in Task List 
bool TasksList::Add(int _socket, TaskaBase* newTask) 
{
	try{
		std::unique_lock<std::mutex> lock(tasksAddMutex);	
//		Logger::log(Logger::LogLevel::log_DEBUG, "Enter ADD");
		
		const str_SGP2_ML_Message _msgN = newTask->getMessage();
		newTask->setTimeStart(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
		
		bool isExistTask =	isExist(_socket, &_msgN);
		if (! isExistTask) {
			
//			Logger::log(Logger::LogLevel::log_DEBUG, "Task @(Add list) socket %d (state %d) (cmd x%x, par-%d) not found", _socket, newTask->getState(),  _msgN.usCmd, _msgN.ucPar  );
	  		TaskList.push_back(newTask);
//	  		printf("\n Created task socket %d, all task = %d \n", _socket, getCount());
	  	}
		  
	    return isExistTask;	  
	} catch(...) { 
		Logger::log(Logger::LogLevel::log_ERROR, "Task ADD ");
		return false;
	}
	return false;				
}

// check exist task
bool TasksList::isExist(int _socket, const str_SGP2_ML_Message* _msg) {
//	std::lock_guard<std::mutex> lock(tasksIsMutex); // ������ � ������� ��������
	try{
		std::lock_guard<std::mutex> lock(tasksMutex);	
	    for (size_t i = 0; i < TaskList.size(); ++i) { 
//	        TaskaBase* task = TaskList[i]; 
            TaskaBase* task = TaskList.at(i);
            
	        if(task == nullptr) { return false; }
			const str_SGP2_ML_Message taskMessage = task->getMessage();
			int TaskSocket = task->getID(); 
	        if (
		           ( task->getID() == _socket            ) &&	
				   ( _msg->usCmd   == taskMessage.usCmd  ) &&
				   ( _msg->ucPar   == taskMessage.ucPar  )
			   ) {
			   	return true;
			}			 
	    }    
	    
	    return false;
	} catch(...) { 
		Logger::log(Logger::LogLevel::log_ERROR, "Task IS Exist ");	
	}
}


int TasksList::getIndex(int _socket, TaskaBase* _task) 
{
//	std::lock_guard<std::mutex> lock(tasksIndex); // ������ � ������� �������� 
	std::unique_lock<std::mutex> lock(tasksMutex);
	if(_task == nullptr) { return -1; }
	if( _socket <= 0 )   { return -1; }
	
	try {
		const str_SGP2_ML_Message taskMessage = _task->getMessage(); // Message _task
		
		for (size_t i = 0; i < TaskList.size(); ++i) { 
//	        TaskaBase* task = TaskList[i]; 
            TaskaBase* task = TaskList.at(i);
            
			const str_SGP2_ML_Message TaskMessageCurrent = task->getMessage();
	        if (
	        	   ( task->getState()         != ts_Success) &&
				   ( task->getID()            == _socket ) &&
				   ( TaskMessageCurrent.usCmd == taskMessage.usCmd   ) &&
				   ( TaskMessageCurrent.ucPar == taskMessage.ucPar   )
			   ) {
			   	
			   	return i;
			}			 
	    }    
	
		return -1;
	} catch(...) {
		Logger::log(Logger::LogLevel::log_ERROR, "Task GET INDEX ");	
	}
}

bool TasksList::AddToStack(int _socket, TaskaBase* newTask) {
//	std::lock_guard<std::mutex> lock(tasksMutex); // ������ � ������� ��������
// todo <?>

	if (Add(_socket, newTask)) {
		try {
			// Task is exist? 
			int Taskindex = getIndex(_socket, newTask); 
			// Task not added, found in the list
			if(Taskindex == -1) {
//				 Logger::log(Logger::LogLevel::log_DEBUG, "Task socket %d (state %d)  not found", _socket, newTask->getState()  );
				 return true; 
			} 
			// Task from the list
//		    TaskaBase* _task = reinterpret_cast<TaskaBase*>(getTask(Taskindex));	
//			TaskaBase* _task = dynamic_cast<TaskaBase*>(getTask(Taskindex));
			TaskaBase* _task = getTaskID(Taskindex);

			if(_task == nullptr) { 
				Logger::log(Logger::LogLevel::log_ERROR, "Get Task error");
				return false; 
			}	
			_task->prepareResponse();            
		} catch(...) { 
			Logger::log(Logger::LogLevel::log_ERROR, "Task ADD TO STACK");
			return false;
		}	
	} 

	return false;		
}

// �������� ���� ������ ����� - �� ������� 
int TasksList::ClearStack(int _socket) 
{
    try {
//        std::unique_lock<std::mutex> lock(tasksMutex); // ���������� ��������
        int DelCount = 0;

        // �������� �� ���������� �����
        if (_socket <= 0) {
            return DelCount;
        }

        // �������� ������ TaskList �� ����� ��������
        for (size_t i = 0; i < TaskList.size();) { 
//            TaskaBase* task = TaskList[i];
			  TaskaBase* task = TaskList.at(i);	 
            
            if (task == nullptr) { 
                // ���� task == nullptr, ������� ��� �� ������
                RemoveFromStack(i);  // �������� ������
                DelCount++;
                delete task;  // �������� �������
            } else {
                int TaskSocket = task->getID(); 
                if (TaskSocket <= 0) {
                    // ���� ������������� ������ <= 0, ����� ������� ��
                    RemoveFromStack(i);
                    DelCount++;
                    delete task;  // �������� �������
                } else if (TaskSocket == _socket) {
                    // ������� ������, ���� ����� ���������
                    RemoveFromStack(i);
                    DelCount++;
                    delete task;  // �������� �������
                } else {
                    // ����������� ������, ������ ���� ������ �� ���� �������
                    ++i;
                }
            }
        } 
        
        return DelCount;
    } catch (const std::exception& e) {
        // �������� ������
//        Logger::log(Logger::LogLevel::log_ERROR, std::string("ClearStack: ") + e.what());
		  Logger::log(Logger::LogLevel::log_ERROR, "ClearStack");	
    } catch (...) {
        // �������� ����������� ������
        Logger::log(Logger::LogLevel::log_ERROR, "ClearStack: Unknown error occurred");
    }
    
    return 0; // ���������� 0 � ������ ������
}


// for statistic --- count task
int TasksList::TaskSum(TaskState _state) 
{
	try {
		std::unique_lock<std::mutex> lock(tasksMutex);
		int res = 0;
		for (size_t i = 0; i < TaskList.size(); ++i) { 
//			TaskaBase* task = TaskList[i]; 
			TaskaBase* task = TaskList.at(i); 
			
			if(task == nullptr) { 
				Logger::log(Logger::LogLevel::log_ERROR, "task == nullptr");
				continue;
			}
			
			if(task->getState()==_state) {
				res++;
			}			
		}
		
		return res;
	} catch(...) {
		Logger::log(Logger::LogLevel::log_ERROR, "TaskSum");	
		return -1;	
	}
}


/*
	����� ���������� task 
	
	1. ��������� ���� �����, ��� �� ������� ���
	2. ���� ��� ���, ���������, ���� �� ���������� � ������ ����������
	3. ���� � ������� ��� ���������� nullptr.

*/
TaskaBase* TasksList::getTaskInstance(int _socket, const str_SGP2_ML_Message* _msg) 
{
	try{
		std::lock_guard<std::mutex> lock(tasksMutex);
	
//	    1) �������� ����� ���� ������ � ������, �������� ��� ��� ���������� � ��������� � ������
		for (size_t i = 0; i < TaskList.size(); ++i) { 	 
			TaskaBase* task = TaskList.at(i);
			
			if(task == nullptr) { // ���� ����� ������-�� null  
				Logger::log(Logger::LogLevel::log_ERROR, "TASK NULLPTR | Instans task #%d", i);
				continue;
			}
		
			const str_SGP2_ML_Message taskMessage = task->getMessage();
			int TaskSocket = task->getID(); 
			if (
			   ( task->getID() == _socket		  ) &&
//			   ( task->getState() <= ts_Ready      ) &&	/*ts_Success*/
			   ( _msg->usCmd == taskMessage.usCmd ) &&
			   ( _msg->ucPar == taskMessage.ucPar )
			) {	
				if( task->getState() >= ts_Success  ) { 
					task->task_reset();
				}
				
				
//				Logger::log(Logger::LogLevel::log_DEBUG, "GET |Existing| Task #%d cmd - 0x%x, par - %d  Exist!", i, taskMessage.usCmd, taskMessage.ucPar );	
				return task; 
//				return nullptr;
			}			 
		}
		
		
//	    2) ����� ������ ��� ���, ����� �������� ���������� ������, � ������� �� ���������, ��� �� �� ��������� �����
		for (size_t i = 0; i < TaskList.size(); ++i) { 	 
			TaskaBase* task = TaskList.at(i);
			
			if(task == nullptr) { // ���� ����� ������-�� null  
				Logger::log(Logger::LogLevel::log_ERROR, "TASK NULLPTR | Instans task #%d", i);	
				continue;
			}
		
			if(task->getState() == ts_free) { // ���� ������ ts_free ������ ��� ������������� ���������, ����� ������� ����
//				Logger::log(Logger::LogLevel::log_DEBUG, "GET |  FREE  | Instans task #%d", i);	
				task->task_reset();
				return task; 
			}
		}
	
		return nullptr;
		
    } catch (...) {
        Logger::log(Logger::LogLevel::log_ERROR, "Unknown exception occurred.");
    }
	
	return nullptr; // ���������� nullptr � ������ ������
}



