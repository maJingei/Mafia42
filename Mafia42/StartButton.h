#pragma once
#include "Button.h"

class StartButton : public Button
{
	using Super = Button;
public:
	StartButton();
	virtual ~StartButton();

	virtual void Update() override;

	void OnClicked();
};

