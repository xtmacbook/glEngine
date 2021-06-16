#ifndef _Render_h_
#define _Render_h_

#include "decl.h"
#include "smartPointer.h"
#include "BaseObject.h"
#include "thread.h"

class  IRender :public base::BaseObject
{

};

class  Render :public IRender,public base::Thread
{
public:
	Render();

	~Render();

	virtual void	render();

	virtual void    run();


};

typedef base::SmartPointer<Render>  Render_SP;
#endif
