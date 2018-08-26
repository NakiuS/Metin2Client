#pragma once

#define SET_EXCEPTION(x) PyErr_SetString(PyExc_RuntimeError, #x)

bool PyTuple_GetString(PyObject* poArgs, int pos, char** ret);
bool PyTuple_GetInteger(PyObject* poArgs, int pos, unsigned char* ret);
bool PyTuple_GetInteger(PyObject* poArgs, int pos, int* ret);
bool PyTuple_GetInteger(PyObject* poArgs, int pos, WORD* ret);
bool PyTuple_GetByte(PyObject* poArgs, int pos, unsigned char* ret);
bool PyTuple_GetUnsignedInteger(PyObject* poArgs, int pos, unsigned int* ret);
bool PyTuple_GetLong(PyObject* poArgs, int pos, long* ret);
bool PyTuple_GetUnsignedLong(PyObject* poArgs, int pos, unsigned long* ret);
bool PyTuple_GetFloat(PyObject* poArgs, int pos, float* ret);
bool PyTuple_GetDouble(PyObject* poArgs, int pos, double* ret);
bool PyTuple_GetObject(PyObject* poArgs, int pos, PyObject** ret);
bool PyTuple_GetBoolean(PyObject* poArgs, int pos, bool* ret);

bool PyCallClassMemberFunc(PyObject* poClass, const char* c_szFunc, PyObject* poArgs);
bool PyCallClassMemberFunc(PyObject* poClass, const char* c_szFunc, PyObject* poArgs, bool* pisRet);
bool PyCallClassMemberFunc(PyObject* poClass, const char* c_szFunc, PyObject* poArgs, long * plRetValue);

bool PyCallClassMemberFunc_ByPyString(PyObject* poClass, PyObject* poFuncName, PyObject* poArgs);
bool PyCallClassMemberFunc(PyObject* poClass, PyObject* poFunc, PyObject* poArgs);

PyObject * Py_BuildException(const char * c_pszErr = NULL, ...);
PyObject * Py_BadArgument();
PyObject * Py_BuildNone();
PyObject * Py_BuildEmptyTuple();
