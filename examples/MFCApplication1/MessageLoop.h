#pragma once
#include "resource.h"

class MainThread : public nbase::FrameworkThread
{
public:
	typedef std::function<void()> Func;
	MainThread() : nbase::FrameworkThread("MainThread") 
	{
	}
	virtual ~MainThread()
	{
	}
	void setFunc(const Func& func) 
	{
		f_ = func;
	}
protected:
	virtual void SpecificInit();
	Func f_;
private:
	virtual void Init() override;
	virtual void Cleanup() override;
};
