#include "StdAfx.h"
#include "PythonApplication.h"

PyObject * sndPlaySound(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.PlaySound2D(szFileName);
	return Py_BuildNone();
}

PyObject * sndPlaySound3D(PyObject * poSelf, PyObject * poArgs)
{
	float fx;
	if (!PyTuple_GetFloat(poArgs, 0, &fx))
		return Py_BuildException();
	float fy;
	if (!PyTuple_GetFloat(poArgs, 1, &fy))
		return Py_BuildException();
	float fz;
	if (!PyTuple_GetFloat(poArgs, 2, &fz))
		return Py_BuildException();
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 3, &szFileName))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.PlaySound3D(fx, fy, fz, szFileName);
	return Py_BuildNone();
}

PyObject * sndPlayMusic(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.PlayMusic(szFileName);
	return Py_BuildNone();
}

PyObject * sndFadeInMusic(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.FadeInMusic(szFileName);
	return Py_BuildNone();
}

PyObject * sndFadeOutMusic(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.FadeOutMusic(szFileName);
	return Py_BuildNone();
}

PyObject * sndFadeOutAllMusic(PyObject * poSelf, PyObject * poArgs)
{
	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.FadeOutAllMusic();
	return Py_BuildNone();
}

PyObject * sndFadeLimitOutMusic(PyObject * poSelf, PyObject * poArgs)
{
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
		return Py_BuildException();

	float fLimitVolume;
	if (!PyTuple_GetFloat(poArgs, 1, &fLimitVolume))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.FadeLimitOutMusic(szFileName, fLimitVolume);
	return Py_BuildNone();
}

PyObject * sndStopAllSound(PyObject * poSelf, PyObject * poArgs)
{
	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.StopAllSound3D();
	return Py_BuildNone();
}

PyObject * sndSetMusicVolume(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();	
	rkSndMgr.SetMusicVolume(fVolume);
	return Py_BuildNone();
}

PyObject * sndSetSoundVolumef(PyObject * poSelf, PyObject * poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();	
	rkSndMgr.SetSoundVolumeRatio(fVolume);
	return Py_BuildNone();
}


PyObject * sndSetSoundVolume(PyObject * poSelf, PyObject * poArgs)
{
	int iVolume;
	if (!PyTuple_GetInteger(poArgs, 0, &iVolume))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.SetSoundVolumeGrade(iVolume);
	return Py_BuildNone();
}

PyObject * sndSetSoundScale(PyObject * poSelf, PyObject * poArgs)
{
	float fScale;
	if (!PyTuple_GetFloat(poArgs, 0, &fScale))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.SetSoundScale(fScale);
	return Py_BuildNone();
}

PyObject * sndSetAmbienceSoundScale(PyObject * poSelf, PyObject * poArgs)
{
	float fScale;
	if (!PyTuple_GetFloat(poArgs, 0, &fScale))
		return Py_BuildException();

	CSoundManager& rkSndMgr=CSoundManager::Instance();
	rkSndMgr.SetAmbienceSoundScale(fScale);
	return Py_BuildNone();
}

void initsnd()
{
	static PyMethodDef s_methods[] = 
	{
		{ "PlaySound",				sndPlaySound,				METH_VARARGS },
		{ "PlaySound3D",			sndPlaySound3D,				METH_VARARGS },
		{ "PlayMusic",				sndPlayMusic,				METH_VARARGS },
		{ "FadeInMusic",			sndFadeInMusic,				METH_VARARGS },
		{ "FadeOutMusic",			sndFadeOutMusic,			METH_VARARGS },
		{ "FadeOutAllMusic",		sndFadeOutAllMusic,			METH_VARARGS },
		{ "FadeLimitOutMusic",		sndFadeLimitOutMusic,		METH_VARARGS },
		{ "StopAllSound",			sndStopAllSound,			METH_VARARGS },

		{ "SetMusicVolumef",		sndSetMusicVolume,			METH_VARARGS },
		{ "SetMusicVolume",			sndSetMusicVolume,			METH_VARARGS },
		{ "SetSoundVolumef",		sndSetSoundVolumef,			METH_VARARGS },
		{ "SetSoundVolume",			sndSetSoundVolume,			METH_VARARGS },
		{ "SetSoundScale",			sndSetSoundScale,			METH_VARARGS },
		{ "SetAmbienceSoundScale",	sndSetAmbienceSoundScale,	METH_VARARGS },
		{ NULL,						NULL,						NULL },
	};

	Py_InitModule("snd", s_methods);
}