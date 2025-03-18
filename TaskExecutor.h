#ifndef TASKLISTENER_H
#define TASKLISTENER_H

#include "TaskList.h"
#include "Logger.h"

/*
	����� ����������� ����������� ���������
	��� ���������� ��������, � ��������� �������� �������
	 - ������ �� ����������
	 - ����� ������� 
	 - ���������� ������ �������
	 �� ����������, ������ �� ���������, � ��������� � ������: FREE
	 ������ �� ��������: FREE, ���������������� �� �����  

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
