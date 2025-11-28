#pragma once

class GuiCmd
{
public:
	GuiCmd();
	virtual ~GuiCmd();
	virtual void Undo() = 0;
	virtual void Redo() = 0;
	virtual void CleanUp() = 0;

private:

};
