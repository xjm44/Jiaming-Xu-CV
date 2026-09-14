#pragma once
#include <QObject>
#include <qtranslator.h>

enum Language
{
    zh_, // chi
    en_, // eng
    undefined
};

// for multi languages
class linguist :public QObject
{
    Q_OBJECT
private:
    linguist();

public:

    Language current_language = Language::undefined;
    
    void changeLanguage(Language lan); // switch language
    static  linguist* getLinguistInstance(); // new pointer

private:
    static linguist* lang;
    QTranslator* translator;

};

