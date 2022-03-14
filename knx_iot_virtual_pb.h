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
 * 
 * @param url the url of the resource/data point
 * @return true value is true
 * @return false value is false or error.
 */
bool app_retrieve_bool_variable(char* url);

/**
 * @brief set the boolean varialbe that belongs to the url 
 * The caller needs to know if the resource/data point is conveying a boolean 
 * 
 * @param url the url of the resource/data point
 * @param value the boolean value to be set
 */
void app_set_bool_variable(char* url, bool value);

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


#ifdef __cplusplus
}
#endif
