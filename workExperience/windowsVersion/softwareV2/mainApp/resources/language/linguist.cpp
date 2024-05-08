#include "Linguist.h"
#include <qcoreapplication.h>
#include <iostream>

linguist* linguist::lang = nullptr;

linguist::linguist()
{
    translator = new QTranslator;
}

linguist* linguist::getLinguistInstance()
{
    if (nullptr == lang)
    {
        lang = new linguist;
    }
    return lang;
}

void linguist::changeLanguage(Language lan)
{

    if (lan == this->current_language || lan == Language::undefined) {
        return;
    }
    bool ret{};
    switch (lan)
    {

    case zh_:
        if (nullptr != translator)
        {
            qApp->removeTranslator(translator);
        }
        ret = translator->load(":linguist/Translation_zh.qm");
        if (ret)
        {
            qApp->installTranslator(translator);
        }
        break;
    case en_:
        if (nullptr != translator)
        {
            qApp->removeTranslator(translator);
        }
        ret = translator->load(":linguist/Translation_en.qm");
        if (ret)
        {
            qApp->installTranslator(translator);
        }
        break;
    default:
        break;
    }
    if (ret)
    {
        this->current_language = lan;
    }
}
