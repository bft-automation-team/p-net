/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2018 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef SAMPLEAPP_COMMON_H
#define SAMPLEAPP_COMMON_H

#include "osal.h"
#include "pnal.h"
#include <pnet_api.h>

#ifdef __cplusplus
extern "C" {
#endif

#define APP_TICK_INTERVAL_US 1000 /* 1 ms */

/* Thread configuration for targets where sample
 * event loop is run in a separate thread (not main).
 * This applies for linux sample app implementation.
 */
#define APP_MAIN_THREAD_PRIORITY  15
#define APP_MAIN_THREAD_STACKSIZE 8192 /* bytes */

#define APP_TICKS_READ_FILES 125     // 10
#define APP_TICKS_UPDATE_DATA  125     // 100

typedef struct app_args
{
   char path_storage_directory[PNET_MAX_DIRECTORYPATH_SIZE]; /** Terminated */
   char station_name[PNET_STATION_NAME_MAX_SIZE]; /** Terminated string */
   char eth_interfaces
      [PNET_INTERFACE_NAME_MAX_SIZE * (PNET_MAX_PHYSICAL_PORTS + 1) +
       PNET_MAX_PHYSICAL_PORTS]; /** Terminated string */
   int verbosity;
   int show;
   bool factory_reset;
   bool remove_files;
   char path_inputs_from_plexus[PNET_MAX_FILE_FULLPATH_SIZE]; /** Terminated string */
   char path_outputs_for_plexus[PNET_MAX_FILE_FULLPATH_SIZE]; /** Terminated string */
   char path_heartbeat[PNET_MAX_FILE_FULLPATH_SIZE]; /** Terminated string */
} app_args_t;

typedef enum
{
   RUN_IN_SEPARATE_THREAD,
   RUN_IN_MAIN_THREAD
} app_run_in_separate_task_t;

typedef struct app_data_t app_data_t;

void app_pnet_cfg_init_default (pnet_cfg_t * pnet_cfg);

/**
 * Initialize application
 *
 * Initialize P-Net stack and application.
 * The pnet_cfg argument shall have been initialized using
 * app_pnet_cfg_init_default() before this function is
 * called.
 * @param pnet_cfg               In:    P-Net start configuration
 * @return Application handle, NULL on error
 */
app_data_t * app_init (pnet_cfg_t * pnet_cfg);

/**
 * Start application
 *
 * Application must have been initialized using app_init() before
 * app_start() is called.
 * If task_config parameters is set to RUN_IN_SEPARATE_THREAD a
 * thread execution the app_loop_forever() function is started.
 * If task_config is set to RUN_IN_MAIN_THREAD no such thread is
 * started and the caller must call the app_loop_forever() after
 * calling this function.
 * RUN_IN_MAIN_THREAD is intended for rt-kernel targets.
 * RUN_IN_SEPARATE_THREAD is intended for linux targets.
 * @param app                 In:    Application handle
 * @param task_config         In:    Defines if stack and application
 *                                   is run in main or separate task.
 *
 * @return 0 on success, -1 on error
 */
int app_start (app_data_t * app, app_run_in_separate_task_t task_config);

/**
 * Application task definition. Handles events
 * in eternal loop.
 * @param arg                 In: Application handle
 * return Should not
 */
void app_loop_forever (void * arg);

/**
 * Get P-Net instance from application
 *
 * @param app                 In:    Application handle
 *
 * @return P-Net instance
 */
pnet_t * app_get_pnet_instance (app_data_t * app);

/**
 * Set output state
 * Hardware specific. Implemented in sample app main file for
 * each supported platform.
 *
 * @param output_state        In:    Output state
 */
void app_set_output_state (uint16_t * output_state);

/**
 * Read button state
 *
 * Hardware specific. Implemented in sample app main file for
 * each supported platform.
 *
 * @param id               In:    Button number, starting from 0.
 * @return  true if button is pressed, false if not
 */
bool app_get_button (uint16_t id);

/**
 * Read inputs state
 *
 * Hardware specific. Implemented in sample app main file for
 * each supported platform.
 *
 * @param read_values   Memory buffer to store read values
 */
void app_get_inputs (uint16_t * read_values);

/**
 * Writes current timestamp on a file.
 */
void update_heartbeat ();

#ifdef __cplusplus
}
#endif

#endif /* SAMPLEAPP_COMMON_H */
