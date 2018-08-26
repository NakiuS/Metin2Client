#include "StdAfx.h"
#include "PythonNetworkDatagram.h"
/*
PyObject * udpEnable(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkDatagram::Instance().Enable();
	return Py_BuildNone();
}

PyObject * udpDisable(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkDatagram::Instance().Disable();
	return Py_BuildNone();
}
*/
void initudp()
{
	/*
	static PyMethodDef s_methods[] =
	{
		{ "Enable",		udpEnable,					METH_VARARGS },
		{ "Disable",	udpDisable,					METH_VARARGS },
		
		{ NULL,						NULL,							NULL		 }
	};

	PyObject * poModule = Py_InitModule("udp", s_methods);
	*/
}
