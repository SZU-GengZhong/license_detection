//#include "StdAfx.h"
#include <iostream>
#include "py_call.h"

using namespace std;
int flag = 0;
char* svm(char* data)
{
   
    
    PyObject* pmodule = PyImport_ImportModule("svm_v2");
    PyObject* pFunc = PyObject_GetAttrString(pmodule, "test");
    PyObject* args = PyTuple_New(1);
    //char* data = new char[32 * 40];
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 40; j++) {
            if(data[i * 40 + j]==0)
                data[i * 40 + j] = 1;
            else
                data[i * 40 + j] = 127;
        }
    }
    /*
    unsigned char* data2 = new unsigned char[20 * 32];
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 32; j++) {
            data2[i * 32 + j] = 254;
        }
    }
    
    //PyObject* t = PyTuple_New(6);
   // PyTuple_SetItem(t, 1, Py_BuildValue("s#", data2, 20 * 32));
    //PyTuple_SetItem(args, 0, t);
    //args =  Py_BuildValue("(s)", data1);
   // PyEval_CallObject(pFunc, NULL);
   */
    PyTuple_SetItem(args, 0, Py_BuildValue("s#", data, 32 * 40));
    PyObject* pyResult = PyEval_CallObject(pFunc, args);
    char* nResult;
    nResult = PyBytes_AsString(pyResult);
    //PyArg_Parse(pyResult, "s", &nResult);
    return nResult;
    
   
}
char* ch_svm(char* data)
{


    PyObject* pmodule = PyImport_ImportModule("ch_svm");
    PyObject* pFunc = PyObject_GetAttrString(pmodule, "test");
    PyObject* args = PyTuple_New(1);
    //char* data = new char[32 * 40];
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 20; j++) {
            if (data[i * 20 + j] == 0)
                data[i * 20 + j] = 1;
            else
                data[i * 20 + j] = 127;
        }
    }

    PyTuple_SetItem(args, 0, Py_BuildValue("s#", data, 20 * 32));
    PyObject* pyResult = PyEval_CallObject(pFunc, args);
    char* nResult;
    nResult = PyBytes_AsString(pyResult);
    return nResult;


}
void py_init() {
    if (flag)
        return;
    Py_Initialize();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append('D:\\design\\detection')");
}
void py_finalize() {
    Py_Finalize();
}