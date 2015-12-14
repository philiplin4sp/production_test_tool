#ifndef TOKENISER
#define TOKENISER

#include <deque>
#include <string>

static inline std::deque<std::string> tokenise(const std::string& toparse, const std::string& delimiters, const bool allowEmptyTokens = false)
{
    std::deque<std::string> tokens;
    std::string::size_type token_start(0), token_end(0);

    while (std::string::npos != token_end && token_start < toparse.size())
    {
        token_end = toparse.find_first_of(delimiters, token_start);
        if (token_end > token_start || allowEmptyTokens)
            tokens.push_back(toparse.substr(token_start, std::string::npos == token_end ? token_end : token_end - token_start));
        token_start = token_end + 1;
    }
	if(allowEmptyTokens && token_start == toparse.size()) // allow terminal delimiters
		tokens.push_back(toparse.substr(token_start));
    return tokens;
}

static inline std::deque<std::wstring> tokenise(const std::wstring& toparse, const std::wstring& delimiters, const bool allowEmptyTokens = false)
{
    std::deque<std::wstring> tokens;
    std::wstring::size_type token_start(0), token_end(0);

    while (std::wstring::npos != token_end && token_start < toparse.size())
    {
        token_end = toparse.find_first_of(delimiters, token_start);
        if (token_end > token_start || allowEmptyTokens)
            tokens.push_back(toparse.substr(token_start, std::wstring::npos == token_end ? token_end : token_end - token_start));
        token_start = token_end + 1;
    }
	if(allowEmptyTokens && token_start == toparse.size()) // allow terminal delimiters
		tokens.push_back(toparse.substr(token_start));
    return tokens;
}

#endif
