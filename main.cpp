#include <QApplication>
#include <QLocale>
#include <QStandardPaths>
#include <QTextStream>
#include <QTranslator>

#include "mainthread.h"
#include "parser.h"
#include "printer.h"
#include "settings.h"
#include "currencies.h"
#include "exceptions/fileexception.h"

#include <QDebug> //TODO

void loadLocale(QApplication *app, QTranslator *translator){
    QString locale;
    if(Settings::getLanguage().isEmpty()){
        locale = QLocale::system().name();
    }
    else{
        locale = Settings::getLanguage();
    }
    QStringList paths;
    paths << QLibraryInfo::location(QLibraryInfo::TranslationsPath);
    paths.push_front(QString()); //Null String will be interpreted as current dir by translator->load()
    auto path = paths.begin();
    for(; path!=paths.end() && translator->isEmpty(); path++){
        translator->load("monex."+locale,*path);
    }
    //If you fail lo load desired locale, try en_US
    for(path = paths.begin(); translator->isEmpty() && path!=paths.end(); path++){
        translator->load("monex.en_US", *path);
    }
    app->installTranslator(translator);
}

int main(int argc, char **argv){
    QApplication app(argc, argv);
    QTranslator translator;
    Money::initCurrencies();
    try{
        Settings::load();
    }
    catch(SettingsParsingException spe){
        loadLocale(&app, &translator);
        Printer::printSettingsParsingError(spe);
        return 1;
    }
    loadLocale(&app, &translator);

    Parser *parser = new Parser(argc, argv);

    MainProgram *mainProgram = new MainProgram(parser);
    Currencies *currencies = new Currencies();
    QObject::connect(currencies, &Currencies::initingFinished, mainProgram, &MainProgram::run);

    currencies->initTable();
    //TODO
    if(Currencies::wereCurrenciesDownloaded()){
        return app.exec();
    }
    else{
        return mainProgram->getExitCode();
    }
}
