#ifndef _LANGUAGE_H
#define _LANGUAGE_H

#include <string>

enum class Lang {
    ENG,  /* English */
    CRO   /* Croatian */
};

class Language {
public:
    Language();

    inline void changeLanguage(Lang lang)
    {
        (lang == Lang::ENG) ? _language = Lang::ENG : _language = Lang::CRO;
    }

    const std::string getText(const std::string& text);

    /* Getters */
    inline Lang language() const { return _language; }

private:
    Lang _language = Lang::CRO;
};

#endif