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


//#define NO_MAIN
#include "knx_iot_virtual_pb.h"

#include "api/oc_knx_dev.h"
#include "api/oc_knx_fp.h"


#include <Python.h>
#include <signal.h>
#include <pthread.h>

static volatile int quit = 0;  /**< stop variable, used by handle_signal */
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;

PyObject *pModule;
PyObject *pSetBacklightFunc;
PyObject *pSetLedFunc;


// Action to take on left button press "p/1"
// This is exposed in the corresponding Python script
// as the knx.handle_left() function
static PyObject *
knx_handle_left(PyObject *self, PyObject *args)
{
  // don't care about args, so don't check them
  (void)self;
  (void)args;
  printf("Left from C! (/p/1) \n");
  char url[] = "/p/1";
  bool p1 = app_retrieve_bool_variable(url);
  if (p1 == true) {
    p1 = false;
  } else {
    p1 = true;
  }
  app_set_bool_variable(url, p1);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  Py_RETURN_NONE;
}

// Action to take on left button press
// This is exposed in the corresponding Python script
// as the knx.handle_left() function
static PyObject *
knx_handle_mid(PyObject *self, PyObject *args)
{
  // don't care about args, so don't check them
  (void)self;
  (void)args;
  printf("Mid from C!  (/p/2)\n");  
  char url[] = "/p/2";
  bool p1 = app_retrieve_bool_variable(url);
  if (p1 == true) {
    p1 = false;
  } else {
    p1 = true;
  }
  app_set_bool_variable(url, p1);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  Py_RETURN_NONE;
}

// Action to take on left button press
// This is exposed in the corresponding Python script
// as the knx.handle_left() function
static PyObject *
knx_handle_right(PyObject *self, PyObject *args)
{
  // don't care about args, so don't check them
  (void)self;
  (void)args;
  printf("Right from C! (/p/3)\n");
  char url[] = "/p/3";
  bool p1 = app_retrieve_bool_variable(url);
  if (p1 == true) {
    p1 = false;
  } else {
    p1 = true;
  }
  app_set_bool_variable(url, p1);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  Py_RETURN_NONE;
}

// Action to take on left button press
// This is exposed in the corresponding Python script
// as the knx.handle_left() function
static PyObject *
knx_handle_up(PyObject *self, PyObject *args)
{
  // don't care about args, so don't check them
  (void)self;
  (void)args;
  printf("UP from C! (/p/4)\n");
  char url[] = "/p/4";
  bool p1 = app_retrieve_bool_variable(url);
  if (p1 == true) {
    p1 = false;
  } else {
    p1 = true;
  }
  app_set_bool_variable(url, p1);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  Py_RETURN_NONE;
}


// Action to take on left button press
// This is exposed in the corresponding Python script
// as the knx.handle_left() function
static PyObject *
knx_handle_down(PyObject *self, PyObject *args)
{
  // don't care about args, so don't check them
  (void)self;
  (void)args;
  printf("Down from C! (/p/5)\n");
  char url[] = "/p/5";
  bool p1 = app_retrieve_bool_variable(url);
  if (p1 == true) {
    p1 = false;
  } else {
    p1 = true;
  }
  app_set_bool_variable(url, p1);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  Py_RETURN_NONE;
}

// Action to take on left button press
// This is exposed in the corresponding Python script
// as the knx.handle_left() function
static PyObject *
knx_handle_cancel(PyObject *self, PyObject *args)
{
  // don't care about args, so don't check them
  (void)self;
  (void)args;
  printf("Cancel from C! (/p/6)\n");
  char url[] = "/p/6";
  bool p1 = app_retrieve_bool_variable(url);
  if (p1 == true) {
    p1 = false;
  } else {
    p1 = true;
  }
  app_set_bool_variable(url, p1);
  oc_do_s_mode_with_scope(2, url, "w");
  oc_do_s_mode_with_scope(5, url, "w");
  Py_RETURN_NONE;
}


// Definition of the methods within the knx module.
// Extend this array if you need to add more Python->C functions
static PyMethodDef KnxMethods[] = {
  { "handle_left", knx_handle_left, METH_NOARGS,
    "Inform KNX of left button press" },
  { "handle_mid", knx_handle_mid, METH_NOARGS,
    "Inform KNX of mid button press" },
  { "handle_right", knx_handle_right, METH_NOARGS,
    "Inform KNX of right button press" },
  { "handle_up", knx_handle_up, METH_NOARGS,
    "Inform KNX of up button press" },
  { "handle_down", knx_handle_down, METH_NOARGS,
    "Inform KNX of down button press" },
  { "handle_cancel", knx_handle_cancel, METH_NOARGS,
    "Inform KNX of cancel button press" },
  { NULL, NULL, 0, NULL }
};

// Boilerplate to initialize the knx module
static PyModuleDef KnxModule = {
  PyModuleDef_HEAD_INIT, "knx", NULL, -1, KnxMethods, NULL, NULL, NULL, NULL
};

static PyObject *
PyInit_knx(void)
{
  return PyModule_Create(&KnxModule);
}

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


static void *
poll_python(void *data)
{
  PyErr_CheckSignals();
  if (PyRun_SimpleString("signal.sigtimedwait([], 0.001)") != 0) {
    printf("Python poll error!\n");
    PyErr_Print();
    quit = 1;
  }
}

/**
 * @brief handle Ctrl-C
 * @param signal the captured signal
 */
static void
handle_signal(int signal)
{
  (void)signal;
  //signal_event_loop();
  quit = 1;
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

  PyImport_AppendInittab("knx", PyInit_knx);

  Py_Initialize();
  PyObject *pName = PyUnicode_DecodeFSDefault("pi_hat");
  PyRun_SimpleString("import sys");
  PyRun_SimpleString("import os");
  PyRun_SimpleString("import signal");
  PyRun_SimpleString("sys.path.append(os.getcwd())");

  pModule = PyImport_Import(pName);
  Py_DECREF(pName);

  // initialize the PiHat - prints stuff to the LCD
  PyRun_SimpleString("import pi_hat");
  PyRun_SimpleString("pi_hat.init_client()");

  //app_set_post_cb(post_callback);

  app_initialize_stack();

  // Start the embedded Python interpreter and initialize the Python API
  // Necessary for controlling the backlight
  python_binding_init();
  // Set the backlight to the initial value
  // set_backlight(g_mystate);
  // set_backlight(true);

  /* Linux specific loop */
  while (quit != 1) {
    oc_main_poll();
    poll_python(NULL);
  }

  /* shut down the stack */
exit:
  oc_main_shutdown();
  return 0;
}
