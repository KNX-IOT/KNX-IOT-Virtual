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
/**
 * @file
 *
 * header file for the generated application.
 * header file contains functions to use the generated application with an external main.
 * e.g. if the c code is compiled without main then 
 * these functions can be used to call all generated code
 *
 */

#include "oc_api.h"
#include "oc_core_res.h"
#include "port/oc_clock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Callback invoked by the stack when a successfull post is done
 *
 * @param[in] url the url of the post
 *
 */
typedef void (*oc_post_cb_t)(char* url);

/**
 * @brief The post callback
 *
 */
typedef struct oc_post_struct_t
{
  oc_post_cb_t cb; /**< the post callback, e.g. when something has changed */
} oc_post_struct_t;

/**
 * @brief set the post callback (on appliation level)
 * 
 * @param cb the callback
 */
void app_set_post_cb(oc_post_cb_t cb);

/**
 * @brief initialize the stack
 * 
 * @return int 0 == success
 */
int app_initialize_stack();

/**
 * @brief retrieve the boolean variable of the url/data point
 * the caller needs to know if the resource/data point is conveying a boolean
 * "/p/1" of CH1_OnOff 
 * "/p/4" of CH1_InfoOnOff 
 * "/p/6" of CH2_OnOff 
 * "/p/9" of CH2_InfoOnOff 
 * "/p/11" of CH3_OnOff 
 * "/p/14" of CH3_InfoOnOff 
 * "/p/16" of CH4_OnOff 
 * "/p/19" of CH4_InfoOnOff 
 * 
 * @param url the url of the resource/data point
 * @return true value is true
 * @return false value is false or error.
 */
bool app_retrieve_bool_variable(char* url);

/**
 * @brief set the boolean variable that belongs to the url 
 * The caller needs to know if the resource/data point is conveying a boolean
 * "/p/1" of CH1_OnOff
 * "/p/6" of CH2_OnOff
 * "/p/11" of CH3_OnOff
 * "/p/16" of CH4_OnOff
 * 
 * @param url the url of the resource/data point
 * @param value the boolean value to be set
 */
void app_set_bool_variable(char* url, bool value);


/**
 * @brief retrieve the int variable of the url/data point
 * the caller needs to know if the resource/data point is conveying a integer
 * "/p/2" of CH1_RelDim
 * "/p/3" of CH1_AbsDimVal
 * "/p/5" of CH1_InfoAbsDimVal
 * "/p/7" of CH2_RelDim
 * "/p/8" of CH2_AbsDimVal
 * "/p/10" of CH2_InfoAbsDimVal
 * "/p/12" of CH3_RelDim
 * "/p/13" of CH3_AbsDimVal
 * "/p/15" of CH3_InfoAbsDimVal
 * "/p/17" of CH4_RelDim
 * "/p/18" of CH4_AbsDimVal
 * "/p/20" of CH4_InfoAbsDimVal
 * 
 * @param url the url of the resource/data point
 * @return true value is true
 * @return false value is false or error.
 */
int app_retrieve_int_variable(char* url);

/**
 * @brief set the integer variable that belongs to the url 
 * The caller needs to know if the resource/data point is conveying a integer
 * "/p/2" of CH1_RelDim
 * "/p/3" of CH1_AbsDimVal
 * "/p/7" of CH2_RelDim
 * "/p/8" of CH2_AbsDimVal
 * "/p/12" of CH3_RelDim
 * "/p/13" of CH3_AbsDimVal
 * "/p/17" of CH4_RelDim
 * "/p/18" of CH4_AbsDimVal
 * 
 * @param url the url of the resource/data point
 * @param value the boolean value to be set
 */
void app_set_integer_variable(char* url, int value);

/**
 * @brief retrieve the fault state of the url/data point
 * the caller needs to know if the resource/data point implements a fault situation
 * 
 * @param url the url of the resource/data point
 * @return true value is true
 * @return false value is false or error.
 */
bool app_retrieve_fault_variable(char* url);

/**
 * @brief sets the fault state of the url/data point 
 * the caller needs to know if the resource/data point implements a fault situation
 * 
 * @param url the url of the resource/data point
 * @param value the boolean fault value to be set
 */
void app_set_fault_variable(char* url, bool value);

/**
 * @brief function to report if the (oscore) security is turn on for this instance
 * 
 * @return true is secure
 * @return false is not secure
 */
bool app_is_secure();


#ifdef __cplusplus
}
#endif
