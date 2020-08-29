#include <Python.h>
#include <string>
#include <iostream>
#include <vector>

using namespace std;


static PyObject * build(PyObject* self, PyObject* args )
{
  PyObject *list;

  if ( ! PyArg_ParseTuple(args, "O", &list)) 
    return NULL;

  PyObject *iter = PyObject_GetIter(list);

  if ( ! iter )
    return NULL;

  PyObject *next_item = NULL;
  PyObject * s = NULL;

  // this loop iterates every element in a list
  while ( 1 ) {
    next_item = PyIter_Next(iter);

    if ( ! next_item )
      break;

    if( PyUnicode_Check( next_item ) ) 
      s = PyUnicode_AsUTF8String( next_item );

    string str;
    if(s) {
      str = string( PyBytes_AsString(s) );
      Py_XDECREF(s);
    }

    // instead of this put the string into trie
    cout << str << endl;
    
  }

  return Py_BuildValue("i", 2);
}

static PyObject* version(PyObject * self )
{
  return Py_BuildValue("s", "Version 1.0");
}

static PyMethodDef myMethods[] = {
  {"build",build,METH_VARARGS,"Prints strings in the list."},
  {"version",(PyCFunction)version,METH_NOARGS,"returns the version."},
  {NULL,NULL,0,NULL}
};

static struct PyModuleDef trie = {
  PyModuleDef_HEAD_INIT,
  "trie",
  "trie Module",
  -1,
  myMethods
};

PyMODINIT_FUNC PyInit_trie(void)
{
  return PyModule_Create(&trie);
}
