#pragma once
#include <string>
#include <memory>

class ShortCutHandlerImpl;
class ShortCutHandler
{
public:
	ShortCutHandler();
	ShortCutHandler(const ShortCutHandler&) = delete;
	ShortCutHandler& operator=(const ShortCutHandler&) = delete;
	void Init();
	void CallFunc(const std::string &funcName);
	bool Contains(const std::string& funcName);
private:
	friend class ShortCutHandlerImpl;
	std::unique_ptr<ShortCutHandlerImpl> impl_;
	std::unordered_map<std::string, void (ShortCutHandlerImpl::*)()> funcMap_;
};

