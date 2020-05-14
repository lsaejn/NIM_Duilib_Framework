#pragma once
#include <string>

class VersionNo;
class IVersionCmpStrategy
{
public:
	virtual int Compare(const VersionNo& a, const VersionNo& b) = 0;
};

class VersionNo
{
public:
	int Compare(const VersionNo& a, const VersionNo& b) const { return a.strategy_->Compare(*this, b); }
	bool	operator==(const VersionNo& versionNo)const { return Compare(*this, versionNo) == 0; }
	bool	operator!=(const VersionNo& versionNo)const { return Compare(*this, versionNo) != 0; }
	bool	operator< (const VersionNo& versionNo)const { return Compare(*this, versionNo) < 0; }
	bool	operator<=(const VersionNo& versionNo)const { return Compare(*this, versionNo) <= 0; }
	bool	operator> (const VersionNo& versionNo)const { return Compare(*this, versionNo) > 0; }
	bool	operator>=(const VersionNo& versionNo)const { return Compare(*this, versionNo) >= 0; }
private:
	std::string	version_;
	IVersionCmpStrategy* strategy_;
};

class AscendingOrder: public IVersionCmpStrategy
{
public:
	virtual int Compare(const VersionNo& a, const VersionNo& b)
	{

	}
};

class AlwaysLessWhenNotEuqual : public IVersionCmpStrategy
{
public:
	virtual int Compare(const VersionNo& a, const VersionNo& b)
	{

	}
};





