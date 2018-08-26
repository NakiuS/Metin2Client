#include "StdAfx.h"
#include <Python-2.7/frameobject.h>
#include "../eterPack/EterPackManager.h"

#include "PythonLauncher.h"

CPythonLauncher::CPythonLauncher()
{
	Py_Initialize();
}

CPythonLauncher::~CPythonLauncher()
{
	Clear();
}

void CPythonLauncher::Clear()
{
	Py_Finalize();
}

std::string g_stTraceBuffer[512];
int	g_nCurTraceN = 0;

void Traceback()
{
	std::string str;

	for (int i = 0; i < g_nCurTraceN; ++i)
	{
		str.append(g_stTraceBuffer[i]);
		str.append("\n");
	}
	
	PyObject * exc;
	PyObject * v;
	PyObject * tb;
	const char * errStr;

	PyErr_Fetch(&exc, &v, &tb);

	if (PyString_Check(v))
	{
		errStr = PyString_AS_STRING(v);
		str.append("Error: ");
		str.append(errStr);

		Tracef("%s\n", errStr);
	}
	Py_DECREF(exc);
	Py_DECREF(v);
	Py_DECREF(tb);
	LogBoxf("Traceback:\n\n%s\n", str.c_str());
}

int TraceFunc(PyObject * obj, PyFrameObject * f, int what, PyObject *arg)
{
	const char * funcname;
	char szTraceBuffer[128];

	switch (what)
	{
		case PyTrace_CALL:
			if (g_nCurTraceN >= 512)
				return 0;

			if (Py_OptimizeFlag)
				f->f_lineno = PyCode_Addr2Line(f->f_code, f->f_lasti);

			funcname = PyString_AsString(f->f_code->co_name);

			_snprintf(szTraceBuffer, sizeof(szTraceBuffer), "Call: File \"%s\", line %d, in %s", 
					  PyString_AsString(f->f_code->co_filename), 
					  f->f_lineno,
					  funcname);

			g_stTraceBuffer[g_nCurTraceN++]=szTraceBuffer;			
			break;

		case PyTrace_RETURN:
			if (g_nCurTraceN > 0)
				--g_nCurTraceN;
			break;

		case PyTrace_EXCEPTION:
			if (g_nCurTraceN >= 512)
				return 0;
			
			PyObject * exc_type, * exc_value, * exc_traceback;

			PyTuple_GetObject(arg, 0, &exc_type);
			PyTuple_GetObject(arg, 1, &exc_value);
			PyTuple_GetObject(arg, 2, &exc_traceback);

			int len;
			const char * exc_str;
			PyObject_AsCharBuffer(exc_type, &exc_str, &len);
			
			_snprintf(szTraceBuffer, sizeof(szTraceBuffer), "Exception: File \"%s\", line %d, in %s", 
					  PyString_AS_STRING(f->f_code->co_filename), 
					  f->f_lineno,
					  PyString_AS_STRING(f->f_code->co_name));

			g_stTraceBuffer[g_nCurTraceN++]=szTraceBuffer;
			
			break;
	}
	return 0;
}

void CPythonLauncher::SetTraceFunc(int (*pFunc)(PyObject * obj, PyFrameObject * f, int what, PyObject *arg))
{
	PyEval_SetTrace(pFunc, NULL);
}

bool CPythonLauncher::Create(const char* c_szProgramName)
{
	NANOBEGIN
	Py_SetProgramName((char*)c_szProgramName);
#ifdef _DEBUG
	PyEval_SetTrace(TraceFunc, NULL);
#endif
	m_poModule = PyImport_AddModule((char *) "__main__");

	if (!m_poModule)
		return false;
	
	m_poDic = PyModule_GetDict(m_poModule);

    PyObject * builtins = PyImport_ImportModule("__builtin__");
	PyModule_AddIntConstant(builtins, "TRUE", 1);
	PyModule_AddIntConstant(builtins, "FALSE", 0);
    PyDict_SetItemString(m_poDic, "__builtins__", builtins);
	Py_DECREF(builtins);

	if (!RunLine("import __main__"))
		return false;
	
	if (!RunLine("import sys"))
		return false;

	NANOEND
	return true;
}

bool CPythonLauncher::RunCompiledFile(const char* c_szFileName)
{
	NANOBEGIN
	FILE * fp = fopen(c_szFileName, "rb");

	if (!fp)
		return false;

	PyCodeObject *co;
	PyObject *v;
	long magic;
	long PyImport_GetMagicNumber(void);

	magic = _PyMarshal_ReadLongFromFile(fp);

	if (magic != PyImport_GetMagicNumber())
	{
		PyErr_SetString(PyExc_RuntimeError, "Bad magic number in .pyc file");
		fclose(fp);
		return false;
	}

	_PyMarshal_ReadLongFromFile(fp);
	v = _PyMarshal_ReadLastObjectFromFile(fp);

	fclose(fp);

	if (!v || !PyCode_Check(v))
	{
		Py_XDECREF(v);
		PyErr_SetString(PyExc_RuntimeError, "Bad code object in .pyc file");
		return false;
	}

	co = (PyCodeObject *) v;
	v = PyEval_EvalCode(co, m_poDic, m_poDic);
/*	if (v && flags)
		flags->cf_flags |= (co->co_flags & PyCF_MASK);*/
	Py_DECREF(co);
	if (!v)
	{
		Traceback();
		return false;
	}

	Py_DECREF(v);
	if (Py_FlushLine()) 
		PyErr_Clear();

	NANOEND
	return true;
}


bool CPythonLauncher::RunMemoryTextFile(const char* c_szFileName, UINT uFileSize, const VOID* c_pvFileData)
{
	NANOBEGIN
	const CHAR* c_pcFileData=(const CHAR*)c_pvFileData;

	std::string stConvFileData;
	stConvFileData.reserve(uFileSize);
	stConvFileData+="exec(compile('''";

	// ConvertPythonTextFormat
	{
		for (UINT i=0; i<uFileSize; ++i)
		{
			if (c_pcFileData[i]!=13)
				stConvFileData+=c_pcFileData[i];
		}
	}

	stConvFileData+= "''', ";
	stConvFileData+= "'";
	stConvFileData+= c_szFileName;
	stConvFileData+= "', ";
	stConvFileData+= "'exec'))";

	const CHAR* c_pcConvFileData=stConvFileData.c_str();
	NANOEND
	return RunLine(c_pcConvFileData);
}

bool CPythonLauncher::RunFile(const char* c_szFileName)
{
	char* acBufData=NULL;
	DWORD dwBufSize=0;
	
	{
		CMappedFile file;
		const VOID* pvData;
		CEterPackManager::Instance().Get(file, c_szFileName, &pvData);
		
		dwBufSize=file.Size();
		if (dwBufSize==0)
			return false;
		
		acBufData=new char[dwBufSize];
		memcpy(acBufData, pvData, dwBufSize);	
	}

	bool ret=false;
	
	ret=RunMemoryTextFile(c_szFileName, dwBufSize, acBufData);

	delete [] acBufData;
	
	return ret;
}

bool CPythonLauncher::RunLine(const char* c_szSrc)
{
	PyObject * v = PyRun_String((char *) c_szSrc, Py_file_input, m_poDic, m_poDic);

	if (!v)
	{
		Traceback();
		return false;
	}

	Py_DECREF(v);
	return true;
}

const char* CPythonLauncher::GetError()
{
	PyObject* exc;
	PyObject* v;
	PyObject* tb;

	PyErr_Fetch(&exc, &v, &tb);        

	if (PyString_Check(v))
		return PyString_AS_STRING(v);
	
	return "";
}
