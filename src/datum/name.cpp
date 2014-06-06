#include <Python.h>

#include <QDebug>
#include <QRegExp>

#include "datum/name.h"
#include "datum/input.h"
#include "node/manager.h"

PyObject* NameDatum::kwlist_contains = NULL;

NameDatum::NameDatum(QString name, QString expr, QObject *parent)
    : EvalDatum(name, parent)
{
    setExpr(expr);
}

QString NameDatum::prepareExpr(QString s) const
{
    return "'" + s.trimmed() + "'";
}

bool NameDatum::validateExpr(QString e) const
{
    QRegExp regex("'[_a-zA-Z][_a-zA-Z0-9]*'");
    return regex.exactMatch(e);
}

bool NameDatum::isKeyword(PyObject* v)
{
    // Lazy initialization of keyword.kwlist.__contains__
    if (!kwlist_contains)
    {
        PyObject* keyword_module = PyImport_ImportModule("keyword");
        PyObject* kwlist = PyObject_GetAttrString(keyword_module, "kwlist");
        Py_DECREF(keyword_module);
        kwlist_contains = PyObject_GetAttrString(kwlist, "__contains__");
        Py_DECREF(kwlist);
    }

    PyObject* args = PyTuple_Pack(1, v);
    PyObject* in_kwlist = PyObject_Call(kwlist_contains, args, NULL);

    Py_DECREF(args);

    bool result = PyObject_IsTrue(in_kwlist);

    Py_DECREF(in_kwlist);

    return result;
}

bool NameDatum::validatePyObject(PyObject *v) const
{
    NameDatum* match = NodeManager::manager()->findMatchingName(v);
    return (match == NULL || match == this) && !isKeyword(v);
}
