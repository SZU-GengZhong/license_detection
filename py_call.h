#define PY_SSIZE_T_CLEAN
#include <Python.h>

char* svm(char* data);
char* ch_svm(char* data);
void py_init();
void py_finalize();