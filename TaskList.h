#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <vector>
#include <stdio.h>

#include "SGP2/SGP2.h"
#include "thClient.h"
#include "cmd_Task.h"

#include "Task_Base.h"
#include "Task_Connect.h"


/*
	������ �����  std::vector<TaskaBase*> TaskList; 
	��������� ������� TaskaBase*
	
	������� ����� ������ �������������� � TaskExecutor
*/

// Tasks List  
class TasksList
{
private:
	std::mutex tasksMutex; // ������� ��� ������ TaskList
	std::mutex tasksAddMutex; // ������� ��� ������ TaskList
	std::mutex tasksClear; // ������� ��� ������ TaskList
    std::vector<TaskaBase*> TaskList; 
    bool isExist(int _socket, const str_SGP2_ML_Message* _msg); 
    std::mutex mtxTaskFreeList;
    //----------------------------------------------------
public:	
//-----
	bool Add(int _socket, TaskaBase* newTask);
	 int  getIndex(int _socket, TaskaBase* _task);
//-----
    int  getCount();	
	int  ClearStack(int _socket);
	bool AddToStack(int _socket, TaskaBase* newTask);
	void RemoveFromStack(int _index);
	TaskaBase* getTaskID(int _index);
	
	
	// ���� � ������ ����������, ������ ������, ���� ��� nullptr 
    //	TaskaBase* getTaskMSG(const str_SGP2_ML_Message* _msg);
	
	int TaskSum(TaskState _state); 
	
	 // -------- Task-List-Used --------------------------
	 
	 void TaskFreeListAdd(TaskaBase* _item) { 
	 	std::unique_lock<std::mutex> lock(mtxTaskFreeList);
//	 	TaskFreeList.push_back(_item);
	 }
	 
	TaskaBase* getTaskInstance(int _socket, const str_SGP2_ML_Message* _msg ); 
 //----------------------------------------------------
};

/*

 ����� ��� ����������� � ������ �������, ������� ���� ������������ ������ �����
 ��� ������������� ���������� ������������ ���� �����


*/

class TaskListAttachBase
{
	protected:
		TasksList* FTasksList;
	public:
		void       setTasksList(TasksList* _taskslist) { FTasksList = _taskslist;	}
		TasksList* getTasksList()                      { return FTasksList;	        }
};


#endif
