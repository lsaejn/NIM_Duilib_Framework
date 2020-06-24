#pragma once

std::string GetRegValue(int nKeyType, const std::string& strUrl, const std::string& strKey);
void SetRegValue(int nKeyType, const std::string& strUrl, const std::string& strKey, const std::string& value);