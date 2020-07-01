#pragma once
#include <string>

namespace Alime
{
    class LineInfo
    {
        constexpr LineInfo() noexcept
            :lineNo_(0),
            filename_("")
        {
        }

        constexpr LineInfo(const int lineno, const char* filename, bool withPath)
            : lineNo_(lineno),
            filename_(filename)
        {
            if (!withPath)
            {
                const char* slash = strrchr(filename, '/');
                if (slash)
                    filename_ = slash + 1;
            }
        }

        std::string to_string() const
        {
            std::string ret;
            this->to_string(ret);
            return ret;
        }

        void to_string(std::string& out) const
        {
            out += filename_;
            Strings::append(out, '(', m_line_number, ')');
        }

    private:
        int lineNo_;
        const char* filename_;
    };
}

