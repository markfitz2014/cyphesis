// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#include "Py_ObserverClient.h"
#include "Py_CreatorClient.h"

#include "ObserverClient.h"

#include "rulesets/Py_Operation.h"
#include "rulesets/Py_Oplist.h"

#include "common/debug.h"

using Atlas::Message::Element;
using Atlas::Message::MapType;

static const bool debug_flag = false;

static PyObject * ObserverClient_setup(PyObserverClient * self, PyObject * args)
{
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.set");
        return nullptr;
    }
#endif // NDEBUG
    char * username = nullptr;
    char * password = nullptr;
    char * avatar = nullptr;
    char empty[] = "";
    if (!PyArg_ParseTuple(args, "|sss", &username, &password, &avatar)) {
        return nullptr;
    }
    if (username == nullptr) {
        username = &empty[0];
        if (password == nullptr) {
            password = &empty[0];
        }
    } else if (password == nullptr) {
        PyErr_SetString(PyExc_TypeError, "function takes 0 or 2, or 3 arguments (1 given)");
        return nullptr;
    }
    int res;
    if (avatar == nullptr) {
        res = self->m_client->setup(username, password);
    } else {
        res = self->m_client->setup(username, password, avatar);
    }
    if (res != 0) {
        PyErr_SetString(PyExc_RuntimeError, "client setup failed");
        return nullptr;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * ObserverClient_teardown(PyObserverClient * self)
{
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.set");
        return nullptr;
    }
#endif // NDEBUG
    self->m_client->teardown();
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * ObserverClient_create_avatar(PyObserverClient * self,
                                               PyObject * arg)
{
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.set");
        return nullptr;
    }
#endif // NDEBUG
    if (!PyUnicode_Check(arg)) {
        PyErr_SetString(PyExc_TypeError, "id must be a string");
        return nullptr;
    }
    char * avatar = PyUnicode_AsUTF8(arg);
    CreatorClient * c = self->m_client->createCharacter(avatar);
    if (c == 0) {
        PyErr_SetString(PyExc_RuntimeError, "avatar creation failed");
        return nullptr;
    }
    PyCreatorClient * pcc = newPyCreatorClient();
    pcc->m_mind.a = c;
    return (PyObject*)pcc;
}

static PyObject * ObserverClient_run(PyObserverClient * self)
{
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.set");
        return nullptr;
    }
#endif // NDEBUG
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * ObserverClient_send(PyObserverClient * self, PyOperation * op)
{
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.send");
        return nullptr;
    }
#endif // NDEBUG
    if (!PyOperation_Check(op)) {
        PyErr_SetString(PyExc_TypeError, "Can only send Atlas operation");
        return nullptr;
    }
    self->m_client->send(op->operation);
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject * ObserverClient_send_wait(PyObserverClient * self,
                                           PyOperation * op)
{
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.send");
        return nullptr;
    }
#endif // NDEBUG
    if (!PyOperation_Check(op)) {
        PyErr_SetString(PyExc_TypeError, "Can only send Atlas operation");
        return nullptr;
    }
    OpVector res;
    self->m_client->sendAndWaitReply(op->operation, res);
    if (res.empty()) {
        Py_INCREF(Py_None);
        return Py_None;
    } else if (res.size() == 1) {
        PyOperation * ret = newPyOperation();
        if (ret != nullptr) {
            ret->operation = res[0];
        }
        return (PyObject*)ret;
    } else {
        PyOplist * ret = newPyOplist();
        if (ret != nullptr) {
            ret->ops = new OpVector(res);
        }
        return (PyObject*)ret;
    }
}

static PyObject * ObserverClient_wait(PyObserverClient * self)
{
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.set");
        return nullptr;
    }
#endif // NDEBUG
    int ret = self->m_client->wait();
    if (ret != 0) {
        PyErr_SetString(PyExc_RuntimeError, "Timeout waiting for reply");
        return nullptr;
    }
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef ObserverClient_methods[] = {
        {"setup",          (PyCFunction)ObserverClient_setup,     METH_VARARGS},
        {"teardown",       (PyCFunction)ObserverClient_teardown,  METH_NOARGS},
        {"create_avatar",  (PyCFunction)ObserverClient_create_avatar, METH_O},
        {"run",            (PyCFunction)ObserverClient_run,       METH_NOARGS},
        {"send",           (PyCFunction)ObserverClient_send,      METH_O},
        {"send_wait",      (PyCFunction)ObserverClient_send_wait, METH_O},
        {"wait",           (PyCFunction)ObserverClient_wait,      METH_NOARGS},
        {nullptr,          nullptr}           /* sentinel */
};

static void ObserverClient_dealloc(PyObserverClient *self)
{
    delete self->m_client;
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject * ObserverClient_getattro(PyObserverClient *self,
                                          PyObject * oname)
{
    // Fairly major re-write of this to use operator[] of ObserverClient base class
#ifndef NDEBUG
    if (self->m_client == nullptr) {
        PyErr_SetString(PyExc_AssertionError, "nullptr ObserverClient in ObserverClient.getattr");
        return nullptr;
    }
#endif // NDEBUG
    char * name = PyUnicode_AsUTF8(oname);
    if (strcmp(name, "character") == 0) {
        if (self->m_client->character() == 0) {
            Py_INCREF(Py_None);
            return Py_None;
        }
        PyCreatorClient * pcc = newPyCreatorClient();
        pcc->m_mind.a = self->m_client->character();
        return (PyObject*)pcc;
    }
    if (strcmp(name, "id") == 0) {
        return PyUnicode_FromString(self->m_client->id().c_str());
    }
    return PyObject_GenericGetAttr((PyObject *)self, oname);
}

static int ObserverClient_setattro(PyObserverClient *self,
                                   PyObject * oname, PyObject *v)
{
    char * name = PyUnicode_AsUTF8(oname);
    if (strcmp(name, "server") == 0) {
        if (!PyUnicode_Check(v)) {
            PyErr_SetString(PyExc_TypeError, "server val must be string");
            return -1;
        }
        self->m_client->setServer(PyUnicode_AsUTF8(v));
        return 0;
    }
    return PyObject_GenericSetAttr((PyObject *)self, oname, v);
}

static PyObject* ObserverClient_compare(PyObject *a, PyObject *b, int op)
{
    auto self = (PyObserverClient*)a;
    if (PyObserverClient_Check(b)) {
        auto other = (PyObserverClient*)b;
        if (op == Py_EQ) {
            return self->m_client == other->m_client ? Py_True : Py_False;
        }
    }

    return Py_NotImplemented;
}

static int ObserverClient_init(PyObserverClient * self,
                              PyObject * args, PyObject * kwds)
{
    if (!PyArg_ParseTuple(args, "")) {
        return -1;
    }
    self->m_client = new ObserverClient();
    return 0;
}

PyTypeObject PyObserverClient_Type = {
        PyVarObject_HEAD_INIT(&PyType_Type, 0)
        "server.ObserverClient",                /*tp_name*/
        sizeof(PyObserverClient),               /*tp_basicsize*/
        0,                                      /*tp_itemsize*/
        /* methods */
        (destructor)ObserverClient_dealloc,     /*tp_dealloc*/
        0,                                      /*tp_print*/
        0,                                      /*tp_getattr*/
        0,                                      /*tp_setattr*/
        0,                                      /*tp_compare*/
        0,                                      /*tp_repr*/
        0,                                      /*tp_as_number*/
        0,                                      /*tp_as_sequence*/
        0,                                      /*tp_as_mapping*/
        0,                                      /*tp_hash*/
        0,                                      // tp_call
        0,                                      // tp_str
        (getattrofunc)ObserverClient_getattro,  // tp_getattro
        (setattrofunc)ObserverClient_setattro,  // tp_setattro
        0,                                      // tp_as_buffer
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flags
        "ObserverClient objects",               // tp_doc
        0,                                      // tp_travers
        0,                                      // tp_clear
        (richcmpfunc)ObserverClient_compare,    // tp_richcompare
        0,                                      // tp_weaklistoffset
        0,                                      // tp_iter
        0,                                      // tp_iternext
        ObserverClient_methods,                 // tp_methods
        0,                                      // tp_members
        0,                                      // tp_getset
        0,                                      // tp_base
        0,                                      // tp_dict
        0,                                      // tp_descr_get
        0,                                      // tp_descr_set
        0,                                      // tp_dictoffset
        (initproc)ObserverClient_init,          // tp_init
        0,                                      // tp_alloc
        0,                                      // tp_new
};

PyObserverClient * newPyObserverClient()
{
    return (PyObserverClient *)PyObserverClient_Type.tp_new(&PyObserverClient_Type, 0, 0);
}
