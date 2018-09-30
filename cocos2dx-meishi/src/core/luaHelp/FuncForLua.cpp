
#include "FuncForLua.h"

FuncForLua::FuncForLua()
{

}

FuncForLua::~FuncForLua()
{

}

bool FuncForLua::ConvertToWidget(Node * node)
{
	Widget * widget = dynamic_cast<Widget *>(node);
	if (widget)
	{
		return true;
	}

	return false;
}
