#include <QtWidgets>

#pragma execution_character_set("utf-8")  // 解决界面显示中文乱码

#ifndef MMS_DEBUG
	#define MMS_INFO(TITLE, TEXT)  QMessageBox::information(this, TITLE, TEXT)
	#define MMS_QU(TITLE, TEXT)  QMessageBox::question(this, TITLE, TEXT)
	#define MMS_WARN(TITLE, TEXT)  QMessageBox::warning(this, TITLE, TEXT)
    #define MMS_CRI(TITLE, TEXT)  QMessageBox::critical(this, TITLE, TEXT)
#else
	#define MMS_INFO(TITLE, TEXT)  
	#define MMS_QU(TITLE, TEXT)  
	#define MMS_WARN(TITLE, TEXT)  
    #define MMS_CRI(TITLE, TEXT)  
#endif