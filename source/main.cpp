/*-------------------------------------------------------------------+
 |     Copyright (c) 2006-2010 Hartmut Ring, University Siegen       |
 |     http://www.mathgui.de                                         |
 |                                                                   |
 |     This file is part of the mathGUIde package.                   |
 |                                                                   |
 | mathGUIde is free software: you can redistribute it and/or modify |
 | it under the terms of the GNU General Public License as published |
 | by the Free Software Foundation, either version 3 of the License, |
 | or (at your option) any later version.                            |
 |                                                                   |
 | You should have received a copy of the GNU General Public License |
 | along with this program. If not, see http://www.gnu.org/licenses/.|
 +-------------------------------------------------------------------*/

#include "mainwindow.h"

#include <QApplication>
#include <QPainterPath>
#include <QPainter>
#include <QMap>
#include <QTranslator>
#include <QLocale>
#include <QDesktopWidget>
#include <QXmlStreamReader>

//#define PY_DEBUG

#ifdef PY_DEBUG
 #include <qdebug.h>
 #include <QMessageBox>
#endif

int main(int argc, char** argv) {
    QApplication app(argc, argv);

#ifdef PY_DEBUG
    QMessageBox::information(0, "Debug-Info", "1");
#endif

    QTranslator translator(0);
    QTranslator qTranslator(0);
    QSettings settings("HartmutRing", "mathGUIde");
    QString localeId = QLocale::system().name().left(2); // "en", "de", ...

    QString userLanguageId = settings.value("/lang", "--").toString();
    if (userLanguageId == "--") { // first start of mathGUIde
        userLanguageId = settings.value("/lang", localeId).toString();
        settings.setValue("/lang", userLanguageId);
    }

#ifdef PY_DEBUG
    QMessageBox::information(0, "Debug-Info", "2");
#endif

    // application data path (for mathguide.py, presentations etc.)
    QString dataPath = QCoreApplication::applicationDirPath(); // default: application path
    QFile f(QString("%1/settings.xml").arg(dataPath));
    if (f.exists()) {                                  // if file "settings.xml" found in application path:
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QTextStream in(&f);
        in.setCodec("UTF-8");
        QString xml = in.readAll();
        f.close();
        // EXAMPLE: <settings datapath="C:\UserDataFolder\Hartmut Ring\mathGUIde"/>
        // without using Qt-XML in order to avoid installing QXml4.dll
        QRegExp re("<\\s*settings\\s+datapath\\s*=\\s*(\"([^\"]+)\"|\'([^\']+)\')");
        re.indexIn(xml);
        QString path = re.cap(1);
        path = path.mid(1, path.length()-2).trimmed();
        if (path.startsWith('.'))                      // relative path
            dataPath += QString("/%1").arg(path);
        else                                           // absolute path
            dataPath = path.trimmed();
#ifdef PY_DEBUG
        QMessageBox::information(0, "path", path);
#endif
    }

    userLanguageId = settings.value("/lang", "en").toString();
    if (userLanguageId == "de") {
        qTranslator.load(dataPath + "/de/qt_de");
        app.installTranslator(&qTranslator);
        translator.load(dataPath + "/de/mathguide_de");
        app.installTranslator(&translator);
    }

    app.setWindowIcon(QIcon(":/img/mathguide.ico"));
    MainWindow mainWin(dataPath);
    mainWin.restoreGeometry(settings.value("geometry/mainWnd").toByteArray());
    mainWin.show();
    return app.exec();
}
