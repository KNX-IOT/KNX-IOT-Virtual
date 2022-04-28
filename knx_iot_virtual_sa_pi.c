/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright (c) 2022 Cascoda Ltd
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/


#define NO_MAIN
#include "knx_iot_virtual_sa.h"
#include "api/oc_knx_dev.h"
#include "api/oc_knx_fp.h"



volatile int quit = 0; /**< stop variable, used by handle_signal */

// Python objects used for initialization
PyObject *pModule;

// Python function for turning the backlight of the LCD on or off.
// Takes one boolean argument matching the desired state of the backlight
PyObject *pSetBacklightFunc;
PyObject *pSetLedFunc;
void

void
python_binding_init(void)
{
  Py_Initialize();

  // Import the Python module that talks to the Displayotron HAT
  PyObject *pName = PyUnicode_DecodeFSDefault("pi_hat");

  // Add current directory to Python path, so that we may import the module
  // located in the same folder as the executable
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("import os");
  PyRun_SimpleString("sys.path.append(os.getcwd())");

  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  if (pModule) {
    // Import the test function, to check the import is successful
    // & that we are importing the right script
    PyObject *pPrintInPythonFunc =
      PyObject_GetAttrString(pModule, "print_in_python");

    if (pPrintInPythonFunc && PyCallable_Check(pPrintInPythonFunc)) {
      // Ensure that the Python embedding is successful
      PyObject *pValue = PyObject_CallObject(pPrintInPythonFunc, NULL);
    }
    // Do not need the test function anymore, so we decrement the reference
    // counter so that the memory for this object can be garbage-collected
    Py_DECREF(pPrintInPythonFunc);

    // Import the rest of the Python API
    pSetBacklightFunc = PyObject_GetAttrString(pModule, "set_backlight");
    pSetLedFunc = PyObject_GetAttrString(pModule, "set_led");

    // Initialize the state of the LCD
    PyRun_SimpleString("import pi_hat");
    PyRun_SimpleString("pi_hat.init()");

  } else {
    PyErr_Print();
    fprintf(stderr, "Failed to load pi_hat\n");
    fprintf(stderr, "Please ensure that pi_hat.py is in the directory "
                    "you are running this executable from!\n");
    exit(1);
  }
}

void
set_backlight(bool value)
{
  if (pSetBacklightFunc && PyCallable_Check(pSetBacklightFunc)) {
    // When called from C, Python functions expect a single tuple argument
    // containing the arguments defined in the API
    PyObject *pArgs = PyTuple_New(1);
    PyObject *pArgTheZeroth = PyBool_FromLong(value);
    PyTuple_SetItem(pArgs, 0, pArgTheZeroth);

    PyObject *pReturn = PyObject_Call(pSetBacklightFunc, pArgs, NULL);
    PyErr_Print();

    // cleanup - must free every temporary Python object
    Py_DECREF(pArgs);
    Py_DECREF(pArgTheZeroth);
    Py_DECREF(pReturn);
  } else {
    PyErr_Print();
    fprintf(stderr, "set_backlight was not called successfully!");
  }
}


void
set_led(int led_nr, bool value)
{
  if (pSetLedFunc && PyCallable_Check(pSetLedFunc)) {
    // When called from C, Python functions expect a single tuple argument
    // containing the arguments defined in the API
    PyObject *pArgs = PyTuple_New(2);
    PyObject *pArgTheZeroth = PyLong_FromLong(led_nr);
    PyObject *pArgTheFirst = PyBool_FromLong(value);
    PyTuple_SetItem(pArgs, 0, pArgTheZeroth);
    PyTuple_SetItem(pArgs, 1, pArgTheFirst);

    PyObject *pReturn = PyObject_Call(pSetLedFunc, pArgs, NULL);
    PyErr_Print();

    // cleanup - must free every temporary Python object
    Py_DECREF(pArgs);
    Py_DECREF(pArgTheZeroth);
    Py_DECREF(pArgTheFirst);
    Py_DECREF(pReturn);
  } else {
    PyErr_Print();
    fprintf(stderr, "set_led was not called successfully!");
  }
}

void post_callback(char* url){
  bool my_bool = app_retrieve_bool_variable(url);
  if (strcmp(url, "p/1") == 0) set_backlight(1, my_bool);
  if (strcmp(url, "p/2") == 0) set_backlight(2, my_bool);
  if (strcmp(url, "p/3") == 0) set_backlight(3, my_bool);
  if (strcmp(url, "p/4") == 0) set_backlight(4, my_bool);
}

/**
 * main application.
 * initializes the global variables
 * registers and starts the handler
 * handles (in a loop) the next event.
 * shuts down the stack
 */
int
main(void)
{
  int init;
  oc_clock_time_t next_event;

  /* Linux specific */
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  /* install Ctrl-C */
  sigaction(SIGINT, &sa, NULL);


  app_set_post_cb(post_callback);

  app_initialize_stack();

  // Start the embedded Python interpreter and initialize the Python API
  // Necessary for controlling the backlight
  python_binding_init();
  // Set the backlight to the initial value
  // set_backlight(g_mystate);

  set_backlight(false);

  /* Linux specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    pthread_mutex_lock(&mutex);
    if (next_event == 0) {
      pthread_cond_wait(&cv, &mutex);
    } else {
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    pthread_mutex_unlock(&mutex);
  }

  /* shut down the stack */

  oc_main_shutdown();
  return 0;
}
