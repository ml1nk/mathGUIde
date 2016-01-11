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

#include "py-inter.h"

#include <Python.h>

#include <QFile>
#include <QRegExp>
#include <QApplication>
#include <QStringList>
#include <QTextStream>
#ifdef TEMP_FILE_IN_PROG_DIR
#else
#include <QDir>
#endif

static int RunPythonString(const QString& s) {
    //return PyRun_SimpleString(s.toLatin1());
    PyCompilerFlags flags;
    flags.cf_flags = PyCF_SOURCE_IS_UTF8;
    return PyRun_SimpleStringFlags(s.toUtf8(), &flags);
}


PythonInterpreter::PythonInterpreter(QString dataPath)
:   _dataPath(dataPath)
{
    _initFiles();
    Py_Initialize();
    wchar_t* argv[] = {(wchar_t*)L""};
    PySys_SetArgv(1, argv);

    QString patch = QString(
        "import sys\n"
        "sys.path.insert(0,r'%1')\n" // look in mathGUIde directory first
        "from code import compile_command\n"
        "from mathguide import *\n"
        "import codecs\n"
        "import mathguide, re\n"
        "class _StdErrCatcher:\n"  // reassign sys.stderr
        "  def write(self, s):\n"
        "    f = codecs.open(r'%2','a+', 'utf8')\n"
        "    f.write(s)\n"
        "    f.close()\n"
        "sys.stderr=_StdErrCatcher()\n"

        "class _StdOutCatcher:\n"  // reassign sys.stdout
        "  def write(self, s):\n"
        "    f = codecs.open(r'%3','a+', 'utf8')\n"
        "    f.write(s)\n"
        "    #f.write(s)\n"
        "    f.close()\n"
        "sys.stdout=_StdOutCatcher()\n").arg(_dataPath)
                                        .arg(_errFile)
                                        .arg(_outFile);
    int result = RunPythonString(patch);
    isOk = (result == 0);

    // Python code for plotting and tables
    QStringList extensions;
    extensions << "plot" << "table";
    foreach (QString ext, extensions) {
        QFile f(QString("%1/%2.py").arg(_dataPath).arg(ext));
        f.open(QIODevice::ReadOnly | QIODevice::Text);
        QString input(f.readAll());
        f.close();
        RunPythonString(input);
    }
}

PythonInterpreter::~PythonInterpreter() {
    Py_Finalize();
}

void PythonInterpreter::_initFiles() {
#ifdef TEMP_FILE_IN_PROG_DIR
    _errFile = _dataPath + "/~mathGUIde.err";
    _outFile = _dataPath + "/~mathGUIde.out";
#else
    _errFile = QDir::tempPath() + "/~mathGUIde.err";
    _outFile = QDir::tempPath() + "/~mathGUIde.out";
#endif
    QFile f(_dataPath + "/mathguide.py");
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&f);
        stream.setCodec("UTF-8");
        _source = stream.readAll();
    }
    f.close();
}

void PythonInterpreter::getMethod(QString sourceLine, QString& cls, QString& method) {
    QString input = QString(
        "try:\n"
        "  print(mathguide._x_classMethInfo('%1', globals(), locals()))\n"
        "except:\n"
        "  print('')\n").arg(sourceLine);
    QString output, error;
    runString(input, output, error);
    QStringList l = output.simplified().split(".");
    cls = l[0];
    method = l[1];
}

QString PythonInterpreter::source(QString method, QString cls/*="global"*/) {
    if (cls == "global") {
        int pos = _source.indexOf(QString("\ndef %1(").arg(method));
        if (pos < 0) return "error";
        int end = _source.indexOf(QRegExp("\\n\\S"), pos+3); // new line, non whitespace
        if (end < 0)
             return "error";
        QString s = _source.mid(pos+1, end-pos-1);
        pos = s.indexOf(QRegExp("\\n\\s*\"\"\""));
        if (pos >= 0) {
            int q = s.indexOf("\"\"\"", pos) + 3;
            end = s.indexOf(QRegExp("\"\"\"\\s*\\n"), q);
            if (end >= 0) {
                s = s.left(pos) + s.mid(end+3);
            }
        }
        return s;
    }
    else {
        int pos = _source.indexOf(QRegExp(QString("\\nclass %1[: (]").arg(cls)));
        if (pos < 0) return cls + " error 0";
        pos = _source.indexOf(QString("\n    def %1(").arg(method), pos);
        if (pos < 0) return cls + " error 1";
        int end = _source.indexOf(QRegExp("\\n    \\S"), pos+7); // new line, 4 spaces, non whitespace
        if (end < 0)
             return "error 2";
        QString s = _source.mid(pos+1, end-pos-1);
        pos = s.indexOf(QRegExp("\\n\\s*\"\"\""));
        if (pos >= 0) {
            int q = s.indexOf("\"\"\"", pos) + 3;
            end = s.indexOf(QRegExp("\"\"\"\\s*\\n"), q);
            if (end >= 0) {
                s = s.left(pos) + s.mid(end+3);
            }
        }
        return s;
    }
}


static QString pythonOutput(QString path) {
    QFile file(path);
    QString text;
    int size = file.size();
    if (size > 0) {
        file.open(QIODevice::ReadOnly);
        QByteArray s = file.read(size);
        file.close();
        QFile::remove(path); // TODO l. 172
        text = QString::fromUtf8(s, size);
    }
    return text;
}

bool PythonInterpreter::runString(QString input, QString& output, QString& error) {
    //QFile::remove(_errFile); // TODO l.165
    QString pyInput = "eval(compile_command('"
        + input.replace(QRegExp("'"), "\\'") + "\\n'))";
    pyInput.replace(QRegExp("\n"), "\\n");
    int result = RunPythonString(pyInput);
    output += pythonOutput(_outFile);
    error  += pythonOutput(_errFile);
    return result == 0;
}

QString PythonInterpreter::getContextInfo(const QString& line) {
    QString input = QString(
        "try:\n"
        "  print(mathguide._contextInfo('%1', globals(), locals()))\n"
        "except:\n"
        "  print('')\n").arg(line);
    QString output, error;
    runString(input, output, error);
    return output;//.simplified();
}

/***
QString PythonInterpreter::editError(QString error, QString errorPrefix) {
    error.replace("\r\n", "#").replace("\r", "#").replace("\n", "#")  // new line --> "#"
         .replace("<", "[").replace(">", "]")                         // HTML!
         //.replace(QRegExp("\\s+"), " ")                               // reduce multiple witespace
         //.replace(" #", "#").replace("# ", "#")                       // strip line begins and ends
         //.replace("Traceback (most recent call last):#File \"[string]\", line 1, in [module]#", "")
         .replace(QRegExp("\\s*Traceback \\(most recent call last\\):\\s*#\\s*File \"\\[string\\]\", line 1, in \\[module\\]\\s*#\\s*"), "");
         //.replace(QRegExp("\\s*Traceback \\(most recent call last\\):"), "§1§")
         //.replace(QRegExp("\\s*File \"\\[string\\]\", line 1, in \\[module\\]\\s*"), "§2§");
    if (error.endsWith('#'))
        error.remove(error.length()-1, 1);      // remove empty last line
    //error.replace("#", "<br/>" + errorPrefix);  // line breaks for HTML
    error.replace("#", "<br/>");  // line breaks for HTML
    int pos = 0;
    while (pos >= 0) {
        QString match = "File \"[input]\", line ";
        pos = error.indexOf(match, pos);
        if (pos >= 0) {
            pos += match.length();
            int len = 0;
            QString numStr = "";
            while (pos+len < error.length() && error[pos+len].isDigit()) {
                numStr += error[pos+len];
                len += 1;
            }
            error.remove(pos, len);
            error.insert(pos, QString("%1").arg(numStr.toInt()-1));
        }
    }
    error.replace("[", "&lt;").replace("]", "&gt;");
    return errorPrefix + error;
}***/

QString PythonInterpreter::editError(QString error, QString errorPrefix) {
    error.replace("\r\n", "#").replace("\r", "#").replace("\n", "#")  // new line --> "#"
         .replace("<", "[").replace(">", "]")                         // HTML!
         .replace(QRegExp("\\s*Traceback \\(most recent call last\\):\\s*#\\s*File \"\\[string\\]\", line 1, in \\[module\\]\\s*#\\s*"), "");
    if (error.endsWith('#'))
        error.remove(error.length()-1, 1);      // remove empty last line
    error.replace("#", "<br/>");  // line breaks for HTML
    int pos = 0;
    while (pos >= 0) {
        QString match = "File \"[input]\", line ";
        pos = error.indexOf(match, pos);
        if (pos >= 0) {
            pos += match.length();
            int len = 0;
            QString numStr = "";
            while (pos+len < error.length() && error[pos+len].isDigit()) {
                numStr += error[pos+len];
                len += 1;
            }
            error.remove(pos, len);
            error.insert(pos, QString("%1").arg(numStr.toInt()-1));
        }
    }
    error.replace("[", "&lt;").replace("]", "&gt;");
    return errorPrefix + error;
}
