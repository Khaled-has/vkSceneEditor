#ifndef TITLEBAR_H
#define TITLEBAR_H

#include "EditorLabel.h"

class TitleBar : public EditorLabel
{
public:
	TitleBar(AppContext* pAppContext) 
		: m_pAppContext(pAppContext)
	{}

	virtual void OnUpdate() override;

private:
	AppContext* m_pAppContext;
	
	void Bar();
	void BarButtons();
	void WindowButtons();
};

#endif