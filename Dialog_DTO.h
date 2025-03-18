#ifndef DIALOG_DTO_H
#define DIALOG_DTO_H

class QeustItem { 
public:
	std::string fileName 	= "";
	bool isValid 		 	= false;
	int  size			 	= 0;
	std::string FirstQuest	= "";
};

// 
class TDialogDTO {
public:
	std::string  QuestText 	 = "";
	unsigned int AnswerCount = 0; 
	std::string  AnswerText[DIALOG_MAX_ANSWERS_COUNT];	
	unsigned int NextActID[DIALOG_MAX_ANSWERS_COUNT]; 	
};




#endif
