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
#ifndef DOXYGEN
/* Force Doxygen to document static inline */
#define STATIC static
#endif

/**
 * @file
 * 
 * KNX virtual Dimming Actuator
 *
 * ## Application Design
 *
 * support functions:
 *
 * - app_init
 *   initializes the stack values.
 * - register_resources
 *   function that registers all endpoints,
 *   e.g. sets the GET/PUT/POST/DELETE
 *      handlers for each end point
 *
 * - main
 *   starts the stack, with the registered resources.
 *   can be compiled out with NO_MAIN
 *
 *  handlers for the implemented methods (get/post):
 *   - get_[path]
 *     function that is being called when a GET is called on [path]
 *     set the global variables in the output
 *   - post_[path]
 *     function that is being called when a POST is called on [path]
 *     if input data is of the correct type
 *       updates the global variables
 *
 * ## stack specific defines
 * - __linux__
 *   build for Linux
 * - WIN32
 *   build for Windows
 * - OC_OSCORE
 *   oscore is enabled as compile flag
 * ## File specific defines
 * - NO_MAIN
 *   compile out the function main()
 * - INCLUDE_EXTERNAL
 *   includes header file "external_header.h", so that other tools/dependencies
 can be included without changing this code
 */
#include "oc_api.h"
#include "oc_core_res.h"
#include "port/oc_clock.h"
#include <signal.h>
/* test purpose only; commandline reset */
#include "api/oc_knx_dev.h"

#ifdef INCLUDE_EXTERNAL
/* import external definitions from header file*/
/* this file should be externally supplied */
#include "external_header.h"
#endif
#include "knx_iot_virtual_dimming_actuator.h"

#ifdef __linux__
/** linux specific code */
#include <pthread.h>
#ifndef NO_MAIN
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
#endif /* NO_MAIN */
#endif

#include <stdio.h> /* defines FILENAME_MAX */

#define MY_NAME "KNX virtual Dimming Actuator" /**< The name of the application */

#ifdef WIN32
/** windows specific code */
#include <windows.h>
STATIC CONDITION_VARIABLE cv; /**< event loop variable */
STATIC CRITICAL_SECTION cs;   /**< event loop variable */
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define btoa(x) ((x) ? "true" : "false")
volatile int quit = 0;  /**< stop variable, used by handle_signal */
bool g_reset = false;   /**< reset variable, set by commandline arguments */

volatile bool g_CH1_OnOff;   /**< global variable for CH1_OnOff */
volatile int g_CH1_RelDim;   /**< global variable for CH1_RelDim */
volatile int g_CH1_AbsDimVal;   /**< global variable for CH1_AbsDimVal */
volatile bool g_CH1_InfoOnOff;   /**< global variable for CH1_InfoOnOff */
volatile int g_CH1_InfoAbsDimVal;   /**< global variable for CH1_InfoAbsDimVal */
volatile bool g_CH2_OnOff;   /**< global variable for CH2_OnOff */
volatile int g_CH2_RelDim;   /**< global variable for CH2_RelDim */
volatile int g_CH2_AbsDimVal;   /**< global variable for CH2_AbsDimVal */
volatile bool g_CH2_InfoOnOff;   /**< global variable for CH2_InfoOnOff */
volatile int g_CH2_InfoAbsDimVal;   /**< global variable for CH2_InfoAbsDimVal */
volatile bool g_CH3_OnOff;   /**< global variable for CH3_OnOff */
volatile int g_CH3_RelDim;   /**< global variable for CH3_RelDim */
volatile int g_CH3_AbsDimVal;   /**< global variable for CH3_AbsDimVal */
volatile bool g_CH3_InfoOnOff;   /**< global variable for CH3_InfoOnOff */
volatile int g_CH3_InfoAbsDimVal;   /**< global variable for CH3_InfoAbsDimVal */
volatile bool g_CH4_OnOff;   /**< global variable for CH4_OnOff */
volatile int g_CH4_RelDim;   /**< global variable for CH4_RelDim */
volatile int g_CH4_AbsDimVal;   /**< global variable for CH4_AbsDimVal */
volatile bool g_CH4_InfoOnOff;   /**< global variable for CH4_InfoOnOff */
volatile int g_CH4_InfoAbsDimVal;   /**< global variable for CH4_InfoAbsDimVal */
 
volatile bool g_fault_CH1_OnOff;   /**< global variable for fault CH1_OnOff */ 
volatile bool g_fault_CH1_RelDim;   /**< global variable for fault CH1_RelDim */ 
volatile bool g_fault_CH1_AbsDimVal;   /**< global variable for fault CH1_AbsDimVal */ 
volatile bool g_fault_CH2_OnOff;   /**< global variable for fault CH2_OnOff */ 
volatile bool g_fault_CH2_RelDim;   /**< global variable for fault CH2_RelDim */ 
volatile bool g_fault_CH2_AbsDimVal;   /**< global variable for fault CH2_AbsDimVal */ 
volatile bool g_fault_CH3_OnOff;   /**< global variable for fault CH3_OnOff */ 
volatile bool g_fault_CH3_RelDim;   /**< global variable for fault CH3_RelDim */ 
volatile bool g_fault_CH3_AbsDimVal;   /**< global variable for fault CH3_AbsDimVal */ 
volatile bool g_fault_CH4_OnOff;   /**< global variable for fault CH4_OnOff */ 
volatile bool g_fault_CH4_RelDim;   /**< global variable for fault CH4_RelDim */ 
volatile bool g_fault_CH4_AbsDimVal;   /**< global variable for fault CH4_AbsDimVal */

void app_set_bool_variable(char* url, bool value) 
{
  if ( strcmp(url, "/p/1") == 0) { 
    g_CH1_OnOff = value;   /**< global variable for CH1_OnOff */
    return;
  } 
  if ( strcmp(url, "/p/4") == 0) { 
    g_CH1_InfoOnOff = value;   /**< global variable for CH1_InfoOnOff */
    return;
  } 
  if ( strcmp(url, "/p/6") == 0) { 
    g_CH2_OnOff = value;   /**< global variable for CH2_OnOff */
    return;
  } 
  if ( strcmp(url, "/p/9") == 0) { 
    g_CH2_InfoOnOff = value;   /**< global variable for CH2_InfoOnOff */
    return;
  } 
  if ( strcmp(url, "/p/11") == 0) { 
    g_CH3_OnOff = value;   /**< global variable for CH3_OnOff */
    return;
  } 
  if ( strcmp(url, "/p/14") == 0) { 
    g_CH3_InfoOnOff = value;   /**< global variable for CH3_InfoOnOff */
    return;
  } 
  if ( strcmp(url, "/p/16") == 0) { 
    g_CH4_OnOff = value;   /**< global variable for CH4_OnOff */
    return;
  } 
  if ( strcmp(url, "/p/19") == 0) { 
    g_CH4_InfoOnOff = value;   /**< global variable for CH4_InfoOnOff */
    return;
  } 
}

bool app_retrieve_bool_variable(char* url) 
{
  if ( strcmp(url, "/p/1") == 0) { 
    return g_CH1_OnOff;   /**< global variable for CH1_OnOff */
  }
  if ( strcmp(url, "/p/4") == 0) { 
    return g_CH1_InfoOnOff;   /**< global variable for CH1_InfoOnOff */
  }
  if ( strcmp(url, "/p/6") == 0) { 
    return g_CH2_OnOff;   /**< global variable for CH2_OnOff */
  }
  if ( strcmp(url, "/p/9") == 0) { 
    return g_CH2_InfoOnOff;   /**< global variable for CH2_InfoOnOff */
  }
  if ( strcmp(url, "/p/11") == 0) { 
    return g_CH3_OnOff;   /**< global variable for CH3_OnOff */
  }
  if ( strcmp(url, "/p/14") == 0) { 
    return g_CH3_InfoOnOff;   /**< global variable for CH3_InfoOnOff */
  }
  if ( strcmp(url, "/p/16") == 0) { 
    return g_CH4_OnOff;   /**< global variable for CH4_OnOff */
  }
  if ( strcmp(url, "/p/19") == 0) { 
    return g_CH4_InfoOnOff;   /**< global variable for CH4_InfoOnOff */
  }
  return false;
}


int app_retrieve_int_variable(char* url)
{
  if ( strcmp(url, "/p/2") == 0) { 
    return g_CH1_RelDim;   /**< global variable for CH1_RelDim */
  }
  if ( strcmp(url, "/p/3") == 0) { 
    return g_CH1_AbsDimVal;   /**< global variable for CH1_AbsDimVal */
  }
  if ( strcmp(url, "/p/5") == 0) { 
    return g_CH1_InfoAbsDimVal;   /**< global variable for CH1_InfoAbsDimVal */
  }
  if ( strcmp(url, "/p/7") == 0) { 
    return g_CH2_RelDim;   /**< global variable for CH2_RelDim */
  }
  if ( strcmp(url, "/p/8") == 0) { 
    return g_CH2_AbsDimVal;   /**< global variable for CH2_AbsDimVal */
  }
  if ( strcmp(url, "/p/10") == 0) { 
    return g_CH2_InfoAbsDimVal;   /**< global variable for CH2_InfoAbsDimVal */
  }
  if ( strcmp(url, "/p/12") == 0) { 
    return g_CH3_RelDim;   /**< global variable for CH3_RelDim */
  }
  if ( strcmp(url, "/p/13") == 0) { 
    return g_CH3_AbsDimVal;   /**< global variable for CH3_AbsDimVal */
  }
  if ( strcmp(url, "/p/15") == 0) { 
    return g_CH3_InfoAbsDimVal;   /**< global variable for CH3_InfoAbsDimVal */
  }
  if ( strcmp(url, "/p/17") == 0) { 
    return g_CH4_RelDim;   /**< global variable for CH4_RelDim */
  }
  if ( strcmp(url, "/p/18") == 0) { 
    return g_CH4_AbsDimVal;   /**< global variable for CH4_AbsDimVal */
  }
  if ( strcmp(url, "/p/20") == 0) { 
    return g_CH4_InfoAbsDimVal;   /**< global variable for CH4_InfoAbsDimVal */
  }
  return -1;
}

void app_set_integer_variable(char* url, int value)
{
  if ( strcmp(url, "/p/2") == 0) { 
    g_CH1_RelDim = value;   /**< global variable for CH1_RelDim */
    return;
  } 
  if ( strcmp(url, "/p/3") == 0) { 
    g_CH1_AbsDimVal = value;   /**< global variable for CH1_AbsDimVal */
    return;
  } 
  if ( strcmp(url, "/p/5") == 0) { 
    g_CH1_InfoAbsDimVal = value;   /**< global variable for CH1_InfoAbsDimVal */
    return;
  } 
  if ( strcmp(url, "/p/7") == 0) { 
    g_CH2_RelDim = value;   /**< global variable for CH2_RelDim */
    return;
  } 
  if ( strcmp(url, "/p/8") == 0) { 
    g_CH2_AbsDimVal = value;   /**< global variable for CH2_AbsDimVal */
    return;
  } 
  if ( strcmp(url, "/p/10") == 0) { 
    g_CH2_InfoAbsDimVal = value;   /**< global variable for CH2_InfoAbsDimVal */
    return;
  } 
  if ( strcmp(url, "/p/12") == 0) { 
    g_CH3_RelDim = value;   /**< global variable for CH3_RelDim */
    return;
  } 
  if ( strcmp(url, "/p/13") == 0) { 
    g_CH3_AbsDimVal = value;   /**< global variable for CH3_AbsDimVal */
    return;
  } 
  if ( strcmp(url, "/p/15") == 0) { 
    g_CH3_InfoAbsDimVal = value;   /**< global variable for CH3_InfoAbsDimVal */
    return;
  } 
  if ( strcmp(url, "/p/17") == 0) { 
    g_CH4_RelDim = value;   /**< global variable for CH4_RelDim */
    return;
  } 
  if ( strcmp(url, "/p/18") == 0) { 
    g_CH4_AbsDimVal = value;   /**< global variable for CH4_AbsDimVal */
    return;
  } 
  if ( strcmp(url, "/p/20") == 0) { 
    g_CH4_InfoAbsDimVal = value;   /**< global variable for CH4_InfoAbsDimVal */
    return;
  } 
}

void app_set_fault_variable(char* url, bool value)
{
 
  if ( strcmp(url, "/p/1") == 0) { 
    g_fault_CH1_OnOff = value;   /**< global variable for CH1_OnOff */
  } 
  if ( strcmp(url, "/p/2") == 0) { 
    g_fault_CH1_RelDim = value;   /**< global variable for CH1_RelDim */
  } 
  if ( strcmp(url, "/p/3") == 0) { 
    g_fault_CH1_AbsDimVal = value;   /**< global variable for CH1_AbsDimVal */
  } 
  if ( strcmp(url, "/p/6") == 0) { 
    g_fault_CH2_OnOff = value;   /**< global variable for CH2_OnOff */
  } 
  if ( strcmp(url, "/p/7") == 0) { 
    g_fault_CH2_RelDim = value;   /**< global variable for CH2_RelDim */
  } 
  if ( strcmp(url, "/p/8") == 0) { 
    g_fault_CH2_AbsDimVal = value;   /**< global variable for CH2_AbsDimVal */
  } 
  if ( strcmp(url, "/p/11") == 0) { 
    g_fault_CH3_OnOff = value;   /**< global variable for CH3_OnOff */
  } 
  if ( strcmp(url, "/p/12") == 0) { 
    g_fault_CH3_RelDim = value;   /**< global variable for CH3_RelDim */
  } 
  if ( strcmp(url, "/p/13") == 0) { 
    g_fault_CH3_AbsDimVal = value;   /**< global variable for CH3_AbsDimVal */
  } 
  if ( strcmp(url, "/p/16") == 0) { 
    g_fault_CH4_OnOff = value;   /**< global variable for CH4_OnOff */
  } 
  if ( strcmp(url, "/p/17") == 0) { 
    g_fault_CH4_RelDim = value;   /**< global variable for CH4_RelDim */
  } 
  if ( strcmp(url, "/p/18") == 0) { 
    g_fault_CH4_AbsDimVal = value;   /**< global variable for CH4_AbsDimVal */
  }
}

bool app_retrieve_fault_variable(char* url)
{
 
  if ( strcmp(url, "/p/1") == 0) { 
    return g_fault_CH1_OnOff;   /**< global variable for CH1_OnOff */
  } 
  if ( strcmp(url, "/p/2") == 0) { 
    return g_fault_CH1_RelDim;   /**< global variable for CH1_RelDim */
  } 
  if ( strcmp(url, "/p/3") == 0) { 
    return g_fault_CH1_AbsDimVal;   /**< global variable for CH1_AbsDimVal */
  } 
  if ( strcmp(url, "/p/6") == 0) { 
    return g_fault_CH2_OnOff;   /**< global variable for CH2_OnOff */
  } 
  if ( strcmp(url, "/p/7") == 0) { 
    return g_fault_CH2_RelDim;   /**< global variable for CH2_RelDim */
  } 
  if ( strcmp(url, "/p/8") == 0) { 
    return g_fault_CH2_AbsDimVal;   /**< global variable for CH2_AbsDimVal */
  } 
  if ( strcmp(url, "/p/11") == 0) { 
    return g_fault_CH3_OnOff;   /**< global variable for CH3_OnOff */
  } 
  if ( strcmp(url, "/p/12") == 0) { 
    return g_fault_CH3_RelDim;   /**< global variable for CH3_RelDim */
  } 
  if ( strcmp(url, "/p/13") == 0) { 
    return g_fault_CH3_AbsDimVal;   /**< global variable for CH3_AbsDimVal */
  } 
  if ( strcmp(url, "/p/16") == 0) { 
    return g_fault_CH4_OnOff;   /**< global variable for CH4_OnOff */
  } 
  if ( strcmp(url, "/p/17") == 0) { 
    return g_fault_CH4_RelDim;   /**< global variable for CH4_RelDim */
  } 
  if ( strcmp(url, "/p/18") == 0) { 
    return g_fault_CH4_AbsDimVal;   /**< global variable for CH4_AbsDimVal */
  }
  return false;
}

bool app_is_secure()
{
#ifdef OC_OSCORE
  return true;
#else
  return false;
#endif /* OC_OSCORE */
}

static oc_post_struct_t app_post = { NULL };

void
app_set_post_cb(oc_post_cb_t cb)
{
  app_post.cb = cb;
}

oc_post_struct_t *
oc_get_post_cb(void)
{
  return &app_post;
}

void do_post_cb(char* url) 
{
  oc_post_struct_t *my_cb = oc_get_post_cb();
  if (my_cb && my_cb->cb) {
    my_cb->cb(url);
  }
}

#ifdef __cplusplus
extern "C" {
#endif
int app_initialize_stack();
void signal_event_loop(void);
void register_resources(void);
int app_init(void);
#ifdef __cplusplus
}
#endif

/**
 * @brief s-mode response callback
 * will be called when a response is received on an s-mode read request
 *
 * @param url the url
 * @param rep the full response
 * @param rep_value the parsed value of the response
 */
STATIC void
oc_add_s_mode_response_cb(char *url, oc_rep_t *rep, oc_rep_t *rep_value)
{
  (void)rep;
  (void)rep_value;

  PRINT("oc_add_s_mode_response_cb %s\n", url);
}

/**
 * @brief function to set up the device.
 *
 * sets the:
 * - manufacturer  : cascoda
 * - serial number : 0006000
 * - base path
 * - knx spec version
 * - hardware version
 * - firmware version
 * - hardware type
 * - device model
 *
 */
STATIC int
app_init(void)
{
  int ret = oc_init_platform("cascoda", NULL, NULL);

  /* set the application name, version, base url, device serial number */
  ret |= oc_add_device(MY_NAME, "1.0.0", "//", "0006000", NULL, NULL);

  oc_device_info_t *device = oc_core_get_device_info(0);
  PRINT("Serial Number: %s\n", oc_string(device->serialnumber));

  /* set the hardware version 0.1.2 */
  oc_core_set_device_hwv(0, 0, 1, 2);

  /* set the firmware version 3.4.5 */
  oc_core_set_device_fwv(0, 3, 4, 5);

  /* set the hardware type*/
  oc_core_set_device_hwt(0, "Linux/windows");

  /* set the model */
  oc_core_set_device_model(0, "KNX virtual - Dimming Actuator");

  oc_set_s_mode_response_cb(oc_add_s_mode_response_cb);

  return ret;
}

/**
 * @brief GET method for "CH1_OnOff" resource at "/p/1".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH1_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH1_OnOff /p/1 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH1_OnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH1_OnOff\n");
}
 
/**
 * @brief POST method for "CH1_OnOff" resource at "/p/1".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH1_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH1_OnOff:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_CH1_OnOff received : %d\n", rep->value.boolean);
      g_CH1_OnOff = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/1");
      PRINT("-- End post_CH1_OnOff\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH1_OnOff\n");
}
/**
 * @brief GET method for "CH1_RelDim" resource at "/p/2".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH1_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH1_RelDim /p/2 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH1_RelDim);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH1_RelDim\n");
}
 
/**
 * @brief POST method for "CH1_RelDim" resource at "/p/2".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH1_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH1_RelDim:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH1_RelDim received : %d\n", rep->value.integer);
      g_CH1_RelDim = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/2");
      PRINT("-- End post_CH1_RelDim\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH1_RelDim\n");
}
/**
 * @brief GET method for "CH1_AbsDimVal" resource at "/p/3".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH1_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH1_AbsDimVal /p/3 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH1_AbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH1_AbsDimVal\n");
}
 
/**
 * @brief POST method for "CH1_AbsDimVal" resource at "/p/3".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH1_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH1_AbsDimVal:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH1_AbsDimVal received : %d\n", rep->value.integer);
      g_CH1_AbsDimVal = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/3");
      PRINT("-- End post_CH1_AbsDimVal\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH1_AbsDimVal\n");
}
/**
 * @brief GET method for "CH1_InfoOnOff" resource at "/p/4".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH1_InfoOnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH1_InfoOnOff /p/4 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH1_InfoOnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH1_InfoOnOff\n");
}

/**
 * @brief GET method for "CH1_InfoAbsDimVal" resource at "/p/5".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH1_InfoAbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH1_InfoAbsDimVal /p/5 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH1_InfoAbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH1_InfoAbsDimVal\n");
}

/**
 * @brief GET method for "CH2_OnOff" resource at "/p/6".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH2_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH2_OnOff /p/6 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH2_OnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH2_OnOff\n");
}
 
/**
 * @brief POST method for "CH2_OnOff" resource at "/p/6".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH2_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH2_OnOff:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_CH2_OnOff received : %d\n", rep->value.boolean);
      g_CH2_OnOff = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/6");
      PRINT("-- End post_CH2_OnOff\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH2_OnOff\n");
}
/**
 * @brief GET method for "CH2_RelDim" resource at "/p/7".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH2_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH2_RelDim /p/7 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH2_RelDim);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH2_RelDim\n");
}
 
/**
 * @brief POST method for "CH2_RelDim" resource at "/p/7".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH2_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH2_RelDim:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH2_RelDim received : %d\n", rep->value.integer);
      g_CH2_RelDim = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/7");
      PRINT("-- End post_CH2_RelDim\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH2_RelDim\n");
}
/**
 * @brief GET method for "CH2_AbsDimVal" resource at "/p/8".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH2_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH2_AbsDimVal /p/8 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH2_AbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH2_AbsDimVal\n");
}
 
/**
 * @brief POST method for "CH2_AbsDimVal" resource at "/p/8".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH2_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH2_AbsDimVal:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH2_AbsDimVal received : %d\n", rep->value.integer);
      g_CH2_AbsDimVal = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/8");
      PRINT("-- End post_CH2_AbsDimVal\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH2_AbsDimVal\n");
}
/**
 * @brief GET method for "CH2_InfoOnOff" resource at "/p/9".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH2_InfoOnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH2_InfoOnOff /p/9 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH2_InfoOnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH2_InfoOnOff\n");
}

/**
 * @brief GET method for "CH2_InfoAbsDimVal" resource at "/p/10".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH2_InfoAbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH2_InfoAbsDimVal /p/10 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH2_InfoAbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH2_InfoAbsDimVal\n");
}

/**
 * @brief GET method for "CH3_OnOff" resource at "/p/11".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH3_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH3_OnOff /p/11 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH3_OnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH3_OnOff\n");
}
 
/**
 * @brief POST method for "CH3_OnOff" resource at "/p/11".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH3_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH3_OnOff:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_CH3_OnOff received : %d\n", rep->value.boolean);
      g_CH3_OnOff = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/11");
      PRINT("-- End post_CH3_OnOff\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH3_OnOff\n");
}
/**
 * @brief GET method for "CH3_RelDim" resource at "/p/12".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH3_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH3_RelDim /p/12 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH3_RelDim);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH3_RelDim\n");
}
 
/**
 * @brief POST method for "CH3_RelDim" resource at "/p/12".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH3_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH3_RelDim:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH3_RelDim received : %d\n", rep->value.integer);
      g_CH3_RelDim = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/12");
      PRINT("-- End post_CH3_RelDim\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH3_RelDim\n");
}
/**
 * @brief GET method for "CH3_AbsDimVal" resource at "/p/13".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH3_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH3_AbsDimVal /p/13 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH3_AbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH3_AbsDimVal\n");
}
 
/**
 * @brief POST method for "CH3_AbsDimVal" resource at "/p/13".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH3_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH3_AbsDimVal:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH3_AbsDimVal received : %d\n", rep->value.integer);
      g_CH3_AbsDimVal = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/13");
      PRINT("-- End post_CH3_AbsDimVal\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH3_AbsDimVal\n");
}
/**
 * @brief GET method for "CH3_InfoOnOff" resource at "/p/14".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH3_InfoOnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH3_InfoOnOff /p/14 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH3_InfoOnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH3_InfoOnOff\n");
}

/**
 * @brief GET method for "CH3_InfoAbsDimVal" resource at "/p/15".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH3_InfoAbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH3_InfoAbsDimVal /p/15 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH3_InfoAbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH3_InfoAbsDimVal\n");
}

/**
 * @brief GET method for "CH4_OnOff" resource at "/p/16".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH4_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH4_OnOff /p/16 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH4_OnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH4_OnOff\n");
}
 
/**
 * @brief POST method for "CH4_OnOff" resource at "/p/16".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH4_OnOff(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH4_OnOff:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_CH4_OnOff received : %d\n", rep->value.boolean);
      g_CH4_OnOff = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/16");
      PRINT("-- End post_CH4_OnOff\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH4_OnOff\n");
}
/**
 * @brief GET method for "CH4_RelDim" resource at "/p/17".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH4_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH4_RelDim /p/17 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH4_RelDim);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH4_RelDim\n");
}
 
/**
 * @brief POST method for "CH4_RelDim" resource at "/p/17".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH4_RelDim(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH4_RelDim:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH4_RelDim received : %d\n", rep->value.integer);
      g_CH4_RelDim = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/17");
      PRINT("-- End post_CH4_RelDim\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH4_RelDim\n");
}
/**
 * @brief GET method for "CH4_AbsDimVal" resource at "/p/18".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH4_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH4_AbsDimVal /p/18 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH4_AbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH4_AbsDimVal\n");
}
 
/**
 * @brief POST method for "CH4_AbsDimVal" resource at "/p/18".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_CH4_AbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_CH4_AbsDimVal:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_INT)) {
      PRINT("  post_CH4_AbsDimVal received : %d\n", rep->value.integer);
      g_CH4_AbsDimVal = rep->value.integer;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      do_post_cb("/p/18");
      PRINT("-- End post_CH4_AbsDimVal\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_CH4_AbsDimVal\n");
}
/**
 * @brief GET method for "CH4_InfoOnOff" resource at "/p/19".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH4_InfoOnOff(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH4_InfoOnOff /p/19 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_CH4_InfoOnOff);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH4_InfoOnOff\n");
}

/**
 * @brief GET method for "CH4_InfoAbsDimVal" resource at "/p/20".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
get_CH4_InfoAbsDimVal(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_CH4_InfoAbsDimVal /p/20 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }
  
  oc_rep_begin_root_object();
  oc_rep_i_set_int(root, 1, g_CH4_InfoAbsDimVal);
  oc_rep_end_root_object();
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_CH4_InfoAbsDimVal\n");
}



/**
 * @brief register all the data point resources to the stack
 * this function registers all data point level resources:
 * - each resource path is bind to a specific function for the supported methods
 *  (GET, POST)
 * - each resource is
 *   - secure
 *   - observable
 *   - discoverable through well-known/core
 *   - used interfaces as: dpa.xxx.yyy 
 *      - xxx : function block number
 *      - yyy : data point function number
 */
STATIC void
register_resources(void)
{
  PRINT("Register Resource 'CH1_OnOff' with local path \"/p/1\"\n");
  oc_resource_t *res_CH1_OnOff =
    oc_new_resource("CH1_OnOff", "/p/1", 2, 0);
  oc_resource_bind_resource_type(res_CH1_OnOff, "urn:knx:dpa.418.61");
  oc_resource_bind_resource_type(res_CH1_OnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH1_OnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH1_OnOff, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH1_OnOff, 1); /* instance 1 */ 
  oc_resource_set_discoverable(res_CH1_OnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH1_OnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH1_OnOff, true);
  oc_resource_set_request_handler(res_CH1_OnOff, OC_GET, get_CH1_OnOff, NULL);
  oc_resource_set_request_handler(res_CH1_OnOff, OC_POST, post_CH1_OnOff, NULL); 
  oc_add_resource(res_CH1_OnOff);
  PRINT("Register Resource 'CH1_RelDim' with local path \"/p/2\"\n");
  oc_resource_t *res_CH1_RelDim =
    oc_new_resource("CH1_RelDim", "/p/2", 2, 0);
  oc_resource_bind_resource_type(res_CH1_RelDim, "urn:knx:dpa.418.62");
  oc_resource_bind_resource_type(res_CH1_RelDim, "DPT_Switch");
  oc_resource_bind_content_type(res_CH1_RelDim, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH1_RelDim, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH1_RelDim, 1); /* instance 1 */ 
  oc_resource_set_discoverable(res_CH1_RelDim, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH1_RelDim, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH1_RelDim, true);
  oc_resource_set_request_handler(res_CH1_RelDim, OC_GET, get_CH1_RelDim, NULL);
  oc_resource_set_request_handler(res_CH1_RelDim, OC_POST, post_CH1_RelDim, NULL); 
  oc_add_resource(res_CH1_RelDim);
  PRINT("Register Resource 'CH1_AbsDimVal' with local path \"/p/3\"\n");
  oc_resource_t *res_CH1_AbsDimVal =
    oc_new_resource("CH1_AbsDimVal", "/p/3", 2, 0);
  oc_resource_bind_resource_type(res_CH1_AbsDimVal, "urn:knx:dpa.418.63");
  oc_resource_bind_resource_type(res_CH1_AbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH1_AbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH1_AbsDimVal, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH1_AbsDimVal, 1); /* instance 1 */ 
  oc_resource_set_discoverable(res_CH1_AbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH1_AbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH1_AbsDimVal, true);
  oc_resource_set_request_handler(res_CH1_AbsDimVal, OC_GET, get_CH1_AbsDimVal, NULL);
  oc_resource_set_request_handler(res_CH1_AbsDimVal, OC_POST, post_CH1_AbsDimVal, NULL); 
  oc_add_resource(res_CH1_AbsDimVal);
  PRINT("Register Resource 'CH1_InfoOnOff' with local path \"/p/4\"\n");
  oc_resource_t *res_CH1_InfoOnOff =
    oc_new_resource("CH1_InfoOnOff", "/p/4", 2, 0);
  oc_resource_bind_resource_type(res_CH1_InfoOnOff, "urn:knx:dpa.418.51");
  oc_resource_bind_resource_type(res_CH1_InfoOnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH1_InfoOnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH1_InfoOnOff, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH1_InfoOnOff, 1); /* instance 1 */ 
  oc_resource_set_discoverable(res_CH1_InfoOnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH1_InfoOnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH1_InfoOnOff, true);
  oc_resource_set_request_handler(res_CH1_InfoOnOff, OC_GET, get_CH1_InfoOnOff, NULL);
  oc_add_resource(res_CH1_InfoOnOff);
  PRINT("Register Resource 'CH1_InfoAbsDimVal' with local path \"/p/5\"\n");
  oc_resource_t *res_CH1_InfoAbsDimVal =
    oc_new_resource("CH1_InfoAbsDimVal", "/p/5", 2, 0);
  oc_resource_bind_resource_type(res_CH1_InfoAbsDimVal, "urn:knx:dpa.418.52");
  oc_resource_bind_resource_type(res_CH1_InfoAbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH1_InfoAbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH1_InfoAbsDimVal, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH1_InfoAbsDimVal, 1); /* instance 1 */ 
  oc_resource_set_discoverable(res_CH1_InfoAbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH1_InfoAbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH1_InfoAbsDimVal, true);
  oc_resource_set_request_handler(res_CH1_InfoAbsDimVal, OC_GET, get_CH1_InfoAbsDimVal, NULL);
  oc_add_resource(res_CH1_InfoAbsDimVal);
  PRINT("Register Resource 'CH2_OnOff' with local path \"/p/6\"\n");
  oc_resource_t *res_CH2_OnOff =
    oc_new_resource("CH2_OnOff", "/p/6", 2, 0);
  oc_resource_bind_resource_type(res_CH2_OnOff, "urn:knx:dpa.418.61");
  oc_resource_bind_resource_type(res_CH2_OnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH2_OnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH2_OnOff, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH2_OnOff, 2); /* instance 2 */ 
  oc_resource_set_discoverable(res_CH2_OnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH2_OnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH2_OnOff, true);
  oc_resource_set_request_handler(res_CH2_OnOff, OC_GET, get_CH2_OnOff, NULL);
  oc_resource_set_request_handler(res_CH2_OnOff, OC_POST, post_CH2_OnOff, NULL); 
  oc_add_resource(res_CH2_OnOff);
  PRINT("Register Resource 'CH2_RelDim' with local path \"/p/7\"\n");
  oc_resource_t *res_CH2_RelDim =
    oc_new_resource("CH2_RelDim", "/p/7", 2, 0);
  oc_resource_bind_resource_type(res_CH2_RelDim, "urn:knx:dpa.418.62");
  oc_resource_bind_resource_type(res_CH2_RelDim, "DPT_Switch");
  oc_resource_bind_content_type(res_CH2_RelDim, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH2_RelDim, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH2_RelDim, 2); /* instance 2 */ 
  oc_resource_set_discoverable(res_CH2_RelDim, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH2_RelDim, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH2_RelDim, true);
  oc_resource_set_request_handler(res_CH2_RelDim, OC_GET, get_CH2_RelDim, NULL);
  oc_resource_set_request_handler(res_CH2_RelDim, OC_POST, post_CH2_RelDim, NULL); 
  oc_add_resource(res_CH2_RelDim);
  PRINT("Register Resource 'CH2_AbsDimVal' with local path \"/p/8\"\n");
  oc_resource_t *res_CH2_AbsDimVal =
    oc_new_resource("CH2_AbsDimVal", "/p/8", 2, 0);
  oc_resource_bind_resource_type(res_CH2_AbsDimVal, "urn:knx:dpa.418.63");
  oc_resource_bind_resource_type(res_CH2_AbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH2_AbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH2_AbsDimVal, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH2_AbsDimVal, 2); /* instance 2 */ 
  oc_resource_set_discoverable(res_CH2_AbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH2_AbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH2_AbsDimVal, true);
  oc_resource_set_request_handler(res_CH2_AbsDimVal, OC_GET, get_CH2_AbsDimVal, NULL);
  oc_resource_set_request_handler(res_CH2_AbsDimVal, OC_POST, post_CH2_AbsDimVal, NULL); 
  oc_add_resource(res_CH2_AbsDimVal);
  PRINT("Register Resource 'CH2_InfoOnOff' with local path \"/p/9\"\n");
  oc_resource_t *res_CH2_InfoOnOff =
    oc_new_resource("CH2_InfoOnOff", "/p/9", 2, 0);
  oc_resource_bind_resource_type(res_CH2_InfoOnOff, "urn:knx:dpa.418.51");
  oc_resource_bind_resource_type(res_CH2_InfoOnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH2_InfoOnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH2_InfoOnOff, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH2_InfoOnOff, 2); /* instance 2 */ 
  oc_resource_set_discoverable(res_CH2_InfoOnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH2_InfoOnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH2_InfoOnOff, true);
  oc_resource_set_request_handler(res_CH2_InfoOnOff, OC_GET, get_CH2_InfoOnOff, NULL);
  oc_add_resource(res_CH2_InfoOnOff);
  PRINT("Register Resource 'CH2_InfoAbsDimVal' with local path \"/p/10\"\n");
  oc_resource_t *res_CH2_InfoAbsDimVal =
    oc_new_resource("CH2_InfoAbsDimVal", "/p/10", 2, 0);
  oc_resource_bind_resource_type(res_CH2_InfoAbsDimVal, "urn:knx:dpa.418.52");
  oc_resource_bind_resource_type(res_CH2_InfoAbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH2_InfoAbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH2_InfoAbsDimVal, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH2_InfoAbsDimVal, 2); /* instance 2 */ 
  oc_resource_set_discoverable(res_CH2_InfoAbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH2_InfoAbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH2_InfoAbsDimVal, true);
  oc_resource_set_request_handler(res_CH2_InfoAbsDimVal, OC_GET, get_CH2_InfoAbsDimVal, NULL);
  oc_add_resource(res_CH2_InfoAbsDimVal);
  PRINT("Register Resource 'CH3_OnOff' with local path \"/p/11\"\n");
  oc_resource_t *res_CH3_OnOff =
    oc_new_resource("CH3_OnOff", "/p/11", 2, 0);
  oc_resource_bind_resource_type(res_CH3_OnOff, "urn:knx:dpa.418.61");
  oc_resource_bind_resource_type(res_CH3_OnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH3_OnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH3_OnOff, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH3_OnOff, 3); /* instance 3 */ 
  oc_resource_set_discoverable(res_CH3_OnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH3_OnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH3_OnOff, true);
  oc_resource_set_request_handler(res_CH3_OnOff, OC_GET, get_CH3_OnOff, NULL);
  oc_resource_set_request_handler(res_CH3_OnOff, OC_POST, post_CH3_OnOff, NULL); 
  oc_add_resource(res_CH3_OnOff);
  PRINT("Register Resource 'CH3_RelDim' with local path \"/p/12\"\n");
  oc_resource_t *res_CH3_RelDim =
    oc_new_resource("CH3_RelDim", "/p/12", 2, 0);
  oc_resource_bind_resource_type(res_CH3_RelDim, "urn:knx:dpa.418.62");
  oc_resource_bind_resource_type(res_CH3_RelDim, "DPT_Switch");
  oc_resource_bind_content_type(res_CH3_RelDim, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH3_RelDim, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH3_RelDim, 3); /* instance 3 */ 
  oc_resource_set_discoverable(res_CH3_RelDim, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH3_RelDim, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH3_RelDim, true);
  oc_resource_set_request_handler(res_CH3_RelDim, OC_GET, get_CH3_RelDim, NULL);
  oc_resource_set_request_handler(res_CH3_RelDim, OC_POST, post_CH3_RelDim, NULL); 
  oc_add_resource(res_CH3_RelDim);
  PRINT("Register Resource 'CH3_AbsDimVal' with local path \"/p/13\"\n");
  oc_resource_t *res_CH3_AbsDimVal =
    oc_new_resource("CH3_AbsDimVal", "/p/13", 2, 0);
  oc_resource_bind_resource_type(res_CH3_AbsDimVal, "urn:knx:dpa.418.63");
  oc_resource_bind_resource_type(res_CH3_AbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH3_AbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH3_AbsDimVal, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH3_AbsDimVal, 3); /* instance 3 */ 
  oc_resource_set_discoverable(res_CH3_AbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH3_AbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH3_AbsDimVal, true);
  oc_resource_set_request_handler(res_CH3_AbsDimVal, OC_GET, get_CH3_AbsDimVal, NULL);
  oc_resource_set_request_handler(res_CH3_AbsDimVal, OC_POST, post_CH3_AbsDimVal, NULL); 
  oc_add_resource(res_CH3_AbsDimVal);
  PRINT("Register Resource 'CH3_InfoOnOff' with local path \"/p/14\"\n");
  oc_resource_t *res_CH3_InfoOnOff =
    oc_new_resource("CH3_InfoOnOff", "/p/14", 2, 0);
  oc_resource_bind_resource_type(res_CH3_InfoOnOff, "urn:knx:dpa.418.51");
  oc_resource_bind_resource_type(res_CH3_InfoOnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH3_InfoOnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH3_InfoOnOff, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH3_InfoOnOff, 3); /* instance 3 */ 
  oc_resource_set_discoverable(res_CH3_InfoOnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH3_InfoOnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH3_InfoOnOff, true);
  oc_resource_set_request_handler(res_CH3_InfoOnOff, OC_GET, get_CH3_InfoOnOff, NULL);
  oc_add_resource(res_CH3_InfoOnOff);
  PRINT("Register Resource 'CH3_InfoAbsDimVal' with local path \"/p/15\"\n");
  oc_resource_t *res_CH3_InfoAbsDimVal =
    oc_new_resource("CH3_InfoAbsDimVal", "/p/15", 2, 0);
  oc_resource_bind_resource_type(res_CH3_InfoAbsDimVal, "urn:knx:dpa.418.52");
  oc_resource_bind_resource_type(res_CH3_InfoAbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH3_InfoAbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH3_InfoAbsDimVal, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH3_InfoAbsDimVal, 3); /* instance 3 */ 
  oc_resource_set_discoverable(res_CH3_InfoAbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH3_InfoAbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH3_InfoAbsDimVal, true);
  oc_resource_set_request_handler(res_CH3_InfoAbsDimVal, OC_GET, get_CH3_InfoAbsDimVal, NULL);
  oc_add_resource(res_CH3_InfoAbsDimVal);
  PRINT("Register Resource 'CH4_OnOff' with local path \"/p/16\"\n");
  oc_resource_t *res_CH4_OnOff =
    oc_new_resource("CH4_OnOff", "/p/16", 2, 0);
  oc_resource_bind_resource_type(res_CH4_OnOff, "urn:knx:dpa.418.61");
  oc_resource_bind_resource_type(res_CH4_OnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH4_OnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH4_OnOff, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH4_OnOff, 4); /* instance 4 */ 
  oc_resource_set_discoverable(res_CH4_OnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH4_OnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH4_OnOff, true);
  oc_resource_set_request_handler(res_CH4_OnOff, OC_GET, get_CH4_OnOff, NULL);
  oc_resource_set_request_handler(res_CH4_OnOff, OC_POST, post_CH4_OnOff, NULL); 
  oc_add_resource(res_CH4_OnOff);
  PRINT("Register Resource 'CH4_RelDim' with local path \"/p/17\"\n");
  oc_resource_t *res_CH4_RelDim =
    oc_new_resource("CH4_RelDim", "/p/17", 2, 0);
  oc_resource_bind_resource_type(res_CH4_RelDim, "urn:knx:dpa.418.62");
  oc_resource_bind_resource_type(res_CH4_RelDim, "DPT_Switch");
  oc_resource_bind_content_type(res_CH4_RelDim, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH4_RelDim, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH4_RelDim, 4); /* instance 4 */ 
  oc_resource_set_discoverable(res_CH4_RelDim, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH4_RelDim, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH4_RelDim, true);
  oc_resource_set_request_handler(res_CH4_RelDim, OC_GET, get_CH4_RelDim, NULL);
  oc_resource_set_request_handler(res_CH4_RelDim, OC_POST, post_CH4_RelDim, NULL); 
  oc_add_resource(res_CH4_RelDim);
  PRINT("Register Resource 'CH4_AbsDimVal' with local path \"/p/18\"\n");
  oc_resource_t *res_CH4_AbsDimVal =
    oc_new_resource("CH4_AbsDimVal", "/p/18", 2, 0);
  oc_resource_bind_resource_type(res_CH4_AbsDimVal, "urn:knx:dpa.418.63");
  oc_resource_bind_resource_type(res_CH4_AbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH4_AbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH4_AbsDimVal, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_CH4_AbsDimVal, 4); /* instance 4 */ 
  oc_resource_set_discoverable(res_CH4_AbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH4_AbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH4_AbsDimVal, true);
  oc_resource_set_request_handler(res_CH4_AbsDimVal, OC_GET, get_CH4_AbsDimVal, NULL);
  oc_resource_set_request_handler(res_CH4_AbsDimVal, OC_POST, post_CH4_AbsDimVal, NULL); 
  oc_add_resource(res_CH4_AbsDimVal);
  PRINT("Register Resource 'CH4_InfoOnOff' with local path \"/p/19\"\n");
  oc_resource_t *res_CH4_InfoOnOff =
    oc_new_resource("CH4_InfoOnOff", "/p/19", 2, 0);
  oc_resource_bind_resource_type(res_CH4_InfoOnOff, "urn:knx:dpa.418.51");
  oc_resource_bind_resource_type(res_CH4_InfoOnOff, "DPT_Switch");
  oc_resource_bind_content_type(res_CH4_InfoOnOff, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH4_InfoOnOff, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH4_InfoOnOff, 4); /* instance 4 */ 
  oc_resource_set_discoverable(res_CH4_InfoOnOff, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH4_InfoOnOff, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH4_InfoOnOff, true);
  oc_resource_set_request_handler(res_CH4_InfoOnOff, OC_GET, get_CH4_InfoOnOff, NULL);
  oc_add_resource(res_CH4_InfoOnOff);
  PRINT("Register Resource 'CH4_InfoAbsDimVal' with local path \"/p/20\"\n");
  oc_resource_t *res_CH4_InfoAbsDimVal =
    oc_new_resource("CH4_InfoAbsDimVal", "/p/20", 2, 0);
  oc_resource_bind_resource_type(res_CH4_InfoAbsDimVal, "urn:knx:dpa.418.52");
  oc_resource_bind_resource_type(res_CH4_InfoAbsDimVal, "DPT_Switch");
  oc_resource_bind_content_type(res_CH4_InfoAbsDimVal, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_CH4_InfoAbsDimVal, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_CH4_InfoAbsDimVal, 4); /* instance 4 */ 
  oc_resource_set_discoverable(res_CH4_InfoAbsDimVal, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_CH4_InfoAbsDimVal, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_CH4_InfoAbsDimVal, true);
  oc_resource_set_request_handler(res_CH4_InfoAbsDimVal, OC_GET, get_CH4_InfoAbsDimVal, NULL);
  oc_add_resource(res_CH4_InfoAbsDimVal);
}

/**
 * @brief initiate preset for device
 * current implementation: device reset as command line argument
 * @param device_index the device identifier of the list of devices
 * @param data the supplied data.
 */
STATIC void
factory_presets_cb(size_t device_index, void *data)
{
  (void)device_index;
  (void)data;

  if (g_reset) {
    PRINT("factory_presets_cb: resetting device\n");
    oc_knx_device_storage_reset(device_index, 2);
  }
}

/**
 * @brief application reset
 *
 * @param device_index the device identifier of the list of devices
 * @param reset_value the knx reset value
 * @param data the supplied data.
 */
STATIC void
reset_cb(size_t device_index, int reset_value, void *data)
{
  (void)device_index;

  PRINT("reset_cb %d\n", reset_value);
}

/**
 * @brief restart the device (application depended)
 *
 * @param device_index the device identifier of the list of devices
 * @param data the supplied data.
 */
STATIC void
restart_cb(size_t device_index, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("-----restart_cb -------\n");
  exit(0);
}

/**
 * @brief set the host name on the device (application depended)
 *
 * @param device_index the device identifier of the list of devices
 * @param host_name the host name to be set on the device
 * @param data the supplied data.
 */
STATIC void
hostname_cb(size_t device_index, oc_string_t host_name, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("-----host name ------- %s\n", oc_string(host_name));
}

/**
 * @brief software update callback
 *
 * @param device_index the device index
 * @param offset the offset of the image
 * @param payload the image data
 * @param len the length of the image data
 * @param data the user data
 */
STATIC void
swu_cb(size_t device_index, size_t offset, uint8_t *payload, size_t len,
       void *data)
{
  (void)device_index;
  char *fname = (char *)data;
  PRINT(" swu_cb %s block=%d size=%d \n", fname, (int)offset, (int)len);

  FILE *fp = fopen(fname, "rw");
  fseek(fp, offset, SEEK_SET);
  size_t written = fwrite(payload, len, 1, fp);
  if (written != len) {
    PRINT(" swu_cb returned %d != %d (expected)\n", (int)written, (int)len);
  }
  fclose(fp);
}

/**
 * @brief initializes the global variables
 * registers and starts the handler
 */
STATIC void
initialize_variables(void)
{
  /* initialize global variables for resources */
  /* if wanted read them from persistent storage */

  g_CH1_OnOff = true;   /**< global variable for CH1_OnOff */ 
  g_CH1_RelDim = 0;   /**< global variable for CH1_RelDim */ 
  g_CH1_AbsDimVal = 0;   /**< global variable for CH1_AbsDimVal */ 
  g_CH1_InfoOnOff = true;   /**< global variable for CH1_InfoOnOff */ 
  g_CH1_InfoAbsDimVal = 0;   /**< global variable for CH1_InfoAbsDimVal */ 
  g_CH2_OnOff = true;   /**< global variable for CH2_OnOff */ 
  g_CH2_RelDim = 0;   /**< global variable for CH2_RelDim */ 
  g_CH2_AbsDimVal = 0;   /**< global variable for CH2_AbsDimVal */ 
  g_CH2_InfoOnOff = true;   /**< global variable for CH2_InfoOnOff */ 
  g_CH2_InfoAbsDimVal = 0;   /**< global variable for CH2_InfoAbsDimVal */ 
  g_CH3_OnOff = true;   /**< global variable for CH3_OnOff */ 
  g_CH3_RelDim = 0;   /**< global variable for CH3_RelDim */ 
  g_CH3_AbsDimVal = 0;   /**< global variable for CH3_AbsDimVal */ 
  g_CH3_InfoOnOff = true;   /**< global variable for CH3_InfoOnOff */ 
  g_CH3_InfoAbsDimVal = 0;   /**< global variable for CH3_InfoAbsDimVal */ 
  g_CH4_OnOff = true;   /**< global variable for CH4_OnOff */ 
  g_CH4_RelDim = 0;   /**< global variable for CH4_RelDim */ 
  g_CH4_AbsDimVal = 0;   /**< global variable for CH4_AbsDimVal */ 
  g_CH4_InfoOnOff = true;   /**< global variable for CH4_InfoOnOff */ 
  g_CH4_InfoAbsDimVal = 0;   /**< global variable for CH4_InfoAbsDimVal */ 
}

int app_initialize_stack()
{
  int init;
  char *fname = "my_software_image";

  PRINT("KNX-IOT Server name : \"%s\"\n", MY_NAME);

  /* show the current working folder */
  char buff[FILENAME_MAX];
  char *retbuf = NULL;
  retbuf = GetCurrentDir(buff, FILENAME_MAX);
  if (retbuf != NULL) {
    PRINT("Current working dir: %s\n", buff);
  }

  /*
   The storage folder depends on the build system
   the folder is created in the makefile, with $target as name with _cred as
   post fix.
  */
  PRINT("\tstorage at 'knx_iot_virtual_dimming_actuator' \n");
  oc_storage_config("./knx_iot_virtual_dimming_actuator");


  /*initialize the variables */
  initialize_variables();

  /* initializes the handlers structure */
  STATIC oc_handler_t handler = { .init = app_init,
                                  .signal_event_loop = signal_event_loop,
                                  .register_resources = register_resources,
                                  .requests_entry = NULL };

  /* set the application callbacks */
  oc_set_hostname_cb(hostname_cb, NULL);
  oc_set_reset_cb(reset_cb, NULL);
  oc_set_restart_cb(restart_cb, NULL);
  oc_set_factory_presets_cb(factory_presets_cb, NULL);
  oc_set_swu_cb(swu_cb, (void *)fname);

  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0) {
    PRINT("oc_main_init failed %d, exiting.\n", init);
    return init;
  }

#ifdef OC_OSCORE
  PRINT("OSCORE - Enabled\n");
#else
  PRINT("OSCORE - Disabled\n");
#endif /* OC_OSCORE */

  oc_device_info_t *device = oc_core_get_device_info(0);
  PRINT("serial number: %s\n", oc_string(device->serialnumber));
  oc_endpoint_t *my_ep = oc_connectivity_get_endpoints(0);
  if (my_ep != NULL) {
    PRINTipaddr(*my_ep);
    PRINT("\n");
  }
  PRINT("Server \"%s\" running, waiting on incoming "
        "connections.\n",
        MY_NAME);
  return 0;
}

#ifdef WIN32
/**
 * @brief signal the event loop (windows version)
 * wakes up the main function to handle the next callback
 */
STATIC void
signal_event_loop(void)
{

#ifndef NO_MAIN
  WakeConditionVariable(&cv);
#endif /* NO_MAIN */
}
#endif /* WIN32 */

#ifndef NO_MAIN

#ifdef __linux__
/**
 * @brief signal the event loop (Linux)
 * wakes up the main function to handle the next callback
 */
STATIC void
signal_event_loop(void)
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
}
#endif /* __linux__ */

/**
 * @brief handle Ctrl-C
 * @param signal the captured signal
 */
STATIC void
handle_signal(int signal)
{
  (void)signal;
  signal_event_loop();
  quit = 1;
}

/**
 * @brief print usage and quits
 *
 */
STATIC void
print_usage()
{
  PRINT("Usage:\n");
  PRINT("no arguments : starts the server\n");
  PRINT("-help  : this message\n");
  PRINT("reset  : does an full reset of the device\n");
  exit(0);
}
/**
 * @brief main application.
 * initializes the global variables
 * registers and starts the handler
 * handles (in a loop) the next event.
 * shuts down the stack
 */
int
main(int argc, char *argv[])
{
  oc_clock_time_t next_event;
  bool do_send_s_mode = false;

#ifdef WIN32
  /* windows specific */
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);
  /* install Ctrl-C */
  signal(SIGINT, handle_signal);
#endif
#ifdef __linux__
  /* Linux specific */
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  /* install Ctrl-C */
  sigaction(SIGINT, &sa, NULL);
#endif

  for (int i = 0; i < argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  if (argc > 1) {
    if (strcmp(argv[1], "reset") == 0) {
      PRINT(" internal reset\n");
      g_reset = true;
    }
    if (strcmp(argv[1], "-help") == 0) {
      print_usage();
    }
  }

  /* do all initialization */
  app_initialize_stack();

#ifdef WIN32
  /* windows specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    if (next_event == 0) {
      SleepConditionVariableCS(&cv, &cs, INFINITE);
    } else {
      oc_clock_time_t now = oc_clock_time();
      if (now < next_event) {
        SleepConditionVariableCS(
          &cv, &cs, (DWORD)((next_event - now) * 1000 / OC_CLOCK_SECOND));
      }
    }
  }
#endif

#ifdef __linux__
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
#endif

  /* shut down the stack */

  oc_main_shutdown();
  return 0;
}
#endif /* NO_MAIN */
