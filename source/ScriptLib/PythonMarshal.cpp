/* Write Python objects to files and read them back.
   This is intended for writing and reading compiled Python code only;
   a true persistent storage facility would be much harder, since
   it would have to take circular links and sharing into account. */

#include "Stdafx.h"
#include <Python-2.7/longintrepr.h>

/* High water mark to determine when the marshalled object is dangerously deep
 * and risks coring the interpreter.  When the object stack gets this deep,
 * raise an exception instead of continuing.
 */
#define MAX_MARSHAL_STACK_DEPTH 5000

#define TYPE_NULL		'0'
#define TYPE_NONE		'N'
#define TYPE_STOPITER	'S'
#define TYPE_ELLIPSIS   '.'
#define TYPE_INT		'i'
#define TYPE_INT64		'I'
#define TYPE_FLOAT		'f'
#define TYPE_COMPLEX	'x'
#define TYPE_LONG		'l'
#define TYPE_STRING		's'
#define TYPE_TUPLE		'('
#define TYPE_LIST		'['
#define TYPE_DICT		'{'
#define TYPE_CODE		'c'
#define TYPE_UNICODE	'u'
#define TYPE_UNKNOWN	'?'

typedef struct
{
	FILE *		fp;
	int			error;
	int			depth;
	PyObject *	str;
	char *		ptr;
	char *		end;
} WFILE;

typedef WFILE RFILE; /* Same struct with different invariants */

#define rs_byte(p) (((p)->ptr != (p)->end) ? (unsigned char)*(p)->ptr++ : EOF)

#define r_byte(p) ((p)->fp ? getc((p)->fp) : rs_byte(p))

static int r_string(char *s, int n, RFILE *p)
{
	if (p->fp != NULL)
		return fread(s, 1, n, p->fp);
	if (p->end - p->ptr < n)
		n = p->end - p->ptr;
	memcpy(s, p->ptr, n);
	p->ptr += n;
	return n;
}

static int r_short(RFILE *p)
{
	register short x;
	x = (short) r_byte(p);
	x |= (short) r_byte(p) << 8;
	/* Sign-extension, in case short greater than 16 bits */
	x |= -(x & 0x8000);
	return x;
}

static long r_long(RFILE *p)
{
	register long x;
	register FILE *fp = p->fp;
	if (fp) {
		x = getc(fp);
		x |= (long)getc(fp) << 8;
		x |= (long)getc(fp) << 16;
		x |= (long)getc(fp) << 24;
	}
	else {
		x = rs_byte(p);
		x |= (long)rs_byte(p) << 8;
		x |= (long)rs_byte(p) << 16;
		x |= (long)rs_byte(p) << 24;
	}
#if SIZEOF_LONG > 4
	/* Sign extension for 64-bit machines */
	x |= -(x & 0x80000000L);
#endif
	return x;
}

/* r_long64 deals with the TYPE_INT64 code.  On a machine with
   sizeof(long) > 4, it returns a Python int object, else a Python long
   object.  Note that w_long64 writes out TYPE_INT if 32 bits is enough,
   so there's no inefficiency here in returning a PyLong on 32-bit boxes
   for everything written via TYPE_INT64 (i.e., if an int is written via
   TYPE_INT64, it *needs* more than 32 bits).
*/
static PyObject * r_long64(RFILE *p)
{
	long lo4 = r_long(p);
	long hi4 = r_long(p);
#if SIZEOF_LONG > 4
	long x = (hi4 << 32) | (lo4 & 0xFFFFFFFFL);
	return PyInt_FromLong(x);
#else
	unsigned char buf[8];
	int one = 1;
	int is_little_endian = (int)*(char*)&one;
	if (is_little_endian) {
		memcpy(buf, &lo4, 4);
		memcpy(buf+4, &hi4, 4);
	}
	else {
		memcpy(buf, &hi4, 4);
		memcpy(buf+4, &lo4, 4);
	}
	return _PyLong_FromByteArray(buf, 8, is_little_endian, 1);
#endif
}

static PyObject * r_object(RFILE *p)
{
	PyObject *v, *v2;
	long i, n;
	int type = r_byte(p);

	switch (type) {

	case EOF:
		PyErr_SetString(PyExc_EOFError,
				"EOF read where object expected");
		return NULL;

	case TYPE_NULL:
		return NULL;

	case TYPE_NONE:
		Py_INCREF(Py_None);
		return Py_None;

	case TYPE_STOPITER:
		Py_INCREF(PyExc_StopIteration);
		return PyExc_StopIteration;

	case TYPE_ELLIPSIS:
		Py_INCREF(Py_Ellipsis);
		return Py_Ellipsis;

	case TYPE_INT:
		return PyInt_FromLong(r_long(p));

	case TYPE_INT64:
		return r_long64(p);

	case TYPE_LONG:
		{
			int size;
			PyLongObject* ob;
			n = r_long(p);
			size = n<0 ? -n : n;
			ob = _PyLong_New(size);
			if (ob == NULL)
				return NULL;
			ob->ob_size = n;
			for (i = 0; i < size; i++)
				ob->ob_digit[i] = (short) r_short(p);
			return (PyObject *) ob;
		}

	case TYPE_FLOAT:
		{
			char buf[256];
			double dx;
			n = r_byte(p);
			if (r_string(buf, (int)n, p) != n) {
				PyErr_SetString(PyExc_EOFError,
					"EOF read where object expected");
				return NULL;
			}
			buf[n] = '\0';
			PyFPE_START_PROTECT("atof", return 0)
			dx = atof(buf);
			PyFPE_END_PROTECT(dx)
			return PyFloat_FromDouble(dx);
		}

#ifndef WITHOUT_COMPLEX
	case TYPE_COMPLEX:
		{
			char buf[256];
			Py_complex c;
			n = r_byte(p);
			if (r_string(buf, (int)n, p) != n) {
				PyErr_SetString(PyExc_EOFError,
					"EOF read where object expected");
				return NULL;
			}
			buf[n] = '\0';
			PyFPE_START_PROTECT("atof", return 0)
			c.real = atof(buf);
			PyFPE_END_PROTECT(c)
			n = r_byte(p);
			if (r_string(buf, (int)n, p) != n) {
				PyErr_SetString(PyExc_EOFError,
					"EOF read where object expected");
				return NULL;
			}
			buf[n] = '\0';
			PyFPE_START_PROTECT("atof", return 0)
			c.imag = atof(buf);
			PyFPE_END_PROTECT(c)
			return PyComplex_FromCComplex(c);
		}
#endif

	case TYPE_STRING:
		n = r_long(p);
		if (n < 0) {
			PyErr_SetString(PyExc_ValueError, "bad marshal data");
			return NULL;
		}
		v = PyString_FromStringAndSize((char *)NULL, n);
		if (v != NULL) {
			if (r_string(PyString_AS_STRING(v), (int)n, p) != n) {
				Py_DECREF(v);
				v = NULL;
				PyErr_SetString(PyExc_EOFError,
					"EOF read where object expected");
			}
		}
		return v;

#ifdef Py_USING_UNICODE
	case TYPE_UNICODE:
	    {
		char *buffer;

		n = r_long(p);
		if (n < 0) {
			PyErr_SetString(PyExc_ValueError, "bad marshal data");
			return NULL;
		}
		buffer = PyMem_NEW(char, n);
		if (buffer == NULL)
			return PyErr_NoMemory();
		if (r_string(buffer, (int)n, p) != n) {
			PyMem_DEL(buffer);
			PyErr_SetString(PyExc_EOFError,
				"EOF read where object expected");
			return NULL;
		}
		v = PyUnicode_DecodeUTF8(buffer, n, NULL);
		PyMem_DEL(buffer);
		return v;
	    }
#endif

	case TYPE_TUPLE:
		n = r_long(p);
		if (n < 0) {
			PyErr_SetString(PyExc_ValueError, "bad marshal data");
			return NULL;
		}
		v = PyTuple_New((int)n);
		if (v == NULL)
			return v;
		for (i = 0; i < n; i++) {
			v2 = r_object(p);
			if ( v2 == NULL ) {
				Py_DECREF(v);
				v = NULL;
				break;
			}
			PyTuple_SET_ITEM(v, (int)i, v2);
		}
		return v;

	case TYPE_LIST:
		n = r_long(p);
		if (n < 0) {
			PyErr_SetString(PyExc_ValueError, "bad marshal data");
			return NULL;
		}
		v = PyList_New((int)n);
		if (v == NULL)
			return v;
		for (i = 0; i < n; i++) {
			v2 = r_object(p);
			if ( v2 == NULL ) {
				Py_DECREF(v);
				v = NULL;
				break;
			}
			PyList_SetItem(v, (int)i, v2);
		}
		return v;

	case TYPE_DICT:
		v = PyDict_New();
		if (v == NULL)
			return NULL;
		for (;;) {
			PyObject *key, *val;
			key = r_object(p);
			if (key == NULL)
				break; /* XXX Assume TYPE_NULL, not an error */
			val = r_object(p);
			if (val != NULL)
				PyDict_SetItem(v, key, val);
			Py_DECREF(key);
			Py_XDECREF(val);
		}
		return v;

	case TYPE_CODE:
		if (PyEval_GetRestricted()) {
			PyErr_SetString(PyExc_RuntimeError,
				"cannot unmarshal code objects in "
				"restricted execution mode");
			return NULL;
		}
		else {
			int argcount = r_short(p);
			int nlocals = r_short(p);
			int stacksize = r_short(p);
			int flags = r_short(p);
			PyObject *code = NULL;
			PyObject *consts = NULL;
			PyObject *names = NULL;
			PyObject *varnames = NULL;
			PyObject *freevars = NULL;
			PyObject *cellvars = NULL;
			PyObject *filename = NULL;
			PyObject *name = NULL;
			int firstlineno = 0;
			PyObject *lnotab = NULL;

			code = r_object(p);
			if (code) consts = r_object(p);
			if (consts) names = r_object(p);
			if (names) varnames = r_object(p);
			if (varnames) freevars = r_object(p);
			if (freevars) cellvars = r_object(p);
			if (cellvars) filename = r_object(p);
			if (filename) name = r_object(p);
			if (name) {
				firstlineno = r_short(p);
				lnotab = r_object(p);
			}

			if (!PyErr_Occurred()) {
				v = (PyObject *) PyCode_New(
					argcount, nlocals, stacksize, flags,
					code, consts, names, varnames,
					freevars, cellvars, filename, name,
					firstlineno, lnotab);
			}
			else
				v = NULL;
			Py_XDECREF(code);
			Py_XDECREF(consts);
			Py_XDECREF(names);
			Py_XDECREF(varnames);
			Py_XDECREF(freevars);
			Py_XDECREF(cellvars);
			Py_XDECREF(filename);
			Py_XDECREF(name);
			Py_XDECREF(lnotab);

		}
		return v;

	default:
		/* Bogus data got written, which isn't ideal.
		   This will let you keep working and recover. */
		PyErr_SetString(PyExc_ValueError, "bad marshal data");
		return NULL;
	}
}

int _PyMarshal_ReadShortFromFile(FILE *fp)
{
	RFILE rf;
	rf.fp = fp;
	return r_short(&rf);
}

long _PyMarshal_ReadLongFromFile(FILE *fp)
{
	RFILE rf;
	rf.fp = fp;
	return r_long(&rf);
}

/* Return size of file in bytes; < 0 if unknown. */
static off_t getfilesize(FILE *fp)
{
	struct stat st;
	if (fstat(fileno(fp), &st) != 0)
		return -1;
	else
		return st.st_size;
}

/* If we can get the size of the file up-front, and it's reasonably small,
 * read it in one gulp and delegate to ...FromString() instead.  Much quicker
 * than reading a byte at a time from file; speeds .pyc imports.
 * CAUTION:  since this may read the entire remainder of the file, don't
 * call it unless you know you're done with the file.
 */
PyObject *_PyMarshal_ReadLastObjectFromFile(FILE *fp)
{
	/* 75% of 2.1's .pyc files can exploit SMALL_FILE_LIMIT.
	 * REASONABLE_FILE_LIMIT is by defn something big enough for Tkinter.pyc.
	 */
#define SMALL_FILE_LIMIT (1L << 14)
#define REASONABLE_FILE_LIMIT (1L << 18)

	off_t filesize;

	if (PyErr_Occurred())
	{
		fprintf(stderr, "XXX rd_object called with exception set\n");
		return NULL;
	}

	filesize = getfilesize(fp);
	if (filesize > 0) {
		char buf[SMALL_FILE_LIMIT];
		char* pBuf = NULL;
		if (filesize <= SMALL_FILE_LIMIT)
			pBuf = buf;
		else if (filesize <= REASONABLE_FILE_LIMIT)
			pBuf = (char *)PyMem_MALLOC(filesize);
		if (pBuf != NULL) {
			PyObject* v;
			size_t n = fread(pBuf, 1, filesize, fp);
			v = PyMarshal_ReadObjectFromString(pBuf, n);
			if (pBuf != buf)
				PyMem_FREE(pBuf);
			return v;
		}

	}

	/* We don't have fstat, or we do but the file is larger than
	 * REASONABLE_FILE_LIMIT or malloc failed -- read a byte at a time.
	 */
	return _PyMarshal_ReadObjectFromFile(fp);
#undef SMALL_FILE_LIMIT
#undef REASONABLE_FILE_LIMIT
}

PyObject * _PyMarshal_ReadObjectFromFile(FILE *fp)
{
	RFILE rf;
	if (PyErr_Occurred()) {
		fprintf(stderr, "XXX rd_object called with exception set\n");
		return NULL;
	}
	rf.fp = fp;
	return r_object(&rf);
}

PyObject * _PyMarshal_ReadObjectFromString(char *str, int len)
{
	RFILE rf;
	if (PyErr_Occurred()) {
		fprintf(stderr, "XXX rds_object called with exception set\n");
		return NULL;
	}
	rf.fp = NULL;
	rf.str = NULL;
	rf.ptr = str;
	rf.end = str + len;
	return r_object(&rf);
}
