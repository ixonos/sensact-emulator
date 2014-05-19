/*
 * Copyright (c) 2013-2014, Ixonos Denmark ApS
 * Copyright (c) 2013-2014, Michael Møller
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "sensact-emulator-test.h"
#include "../emulator_sensors/sensact_emulator_ble.h"
#include "../emulator_sensors/sensact_emulator_engine.h"
#include "../emulator_sensors/sensact_emulator_senshub.h"

#define MAX_NUMBER_OF_THREDS			10
#define RUN_ENGINE_THREAD_ID   	 	 	 0
#define RUN_ORIENTATION_THREAD_ID	 	 1
#define RUN_TEMPERATURE_THREAD_ID	 	 2
#define RUN_AMBIENT_TEMPERATURE_THREAD_ID	 3
#define RUN_HUMIDITY_THREAD_ID			 4
#define RUN_BRIGHTNESS_THREAD_ID		 5
#define RUN_AIRPRESSURE_THREAD_ID                6

#define AMBIENT_TEMPERATURE_CHANGE_DELAY	400000
#define TEMPERATURE_CHANGE_DELAY		800000
#define HUMIDITY_CHANGE_DELAY			600000
#define BRIGHTNESS_CHANGE_DELAY			900000
#define AIR_PRESSURE_CHANGE_DELAY		1000

#define  SWEEP_ENGINE_FAST_DELAY		1
#define  SWEEP_ENGINE_ZERO_TO_MAX_DELAY		1000
#define  SWEEP_ENGINE_ZERO_TO_MAX_ZERO_DELAY	1000
#define  SWEEP_ENGINE_REVERSE_DELAY		2

#define  ORIENTATION_SWEEP_DELAY		8000
pthread_t threadId[MAX_NUMBER_OF_THREDS];

int emulatorRunning = 1;

int tests_run = 0;

void *shared_memory;
senshub_t * senshub;
engine_t *engine;
/**
 *dummy to write RPM
 */

void emulator_set_direction(int direction) {
	engine->direction = direction;
}

void emulator_set_rpm(int rpm) {
	engine->rpm = rpm;
}

/**
 * sweeps the engine from 1 to max to 1 - 10 times
 */
static char * test_sweep_engine_fast() {
	int rpm = 0;
	emulator_set_rpm(rpm);
	int count;
	for (count = 0; count < 10; count++) {
		for (rpm = 0; rpm < 8000; rpm++) {
			emulator_set_rpm(rpm);
		}
		sleep(SWEEP_ENGINE_FAST_DELAY);
		mu_assert("error, engine->rpm != 7999", engine->rpm == 7999);
		for (rpm = 8000; rpm > 0; rpm--) {
			emulator_set_rpm(rpm);
		}
		sleep(SWEEP_ENGINE_FAST_DELAY);
		mu_assert("error, engine->rpm != 1", engine->rpm == 1);
	}

	return 0;
}
/**
 * sweeps the engine from 1 to 8000 - 10 times
 */
static char * test_sweep_engine_zero_max() {

	int rpm = 0;
	emulator_set_rpm(rpm);
	int count;
	for (count = 0; count < 10; count++) {
		for (rpm = 0; rpm < 8000; rpm++) {
			usleep(SWEEP_ENGINE_ZERO_TO_MAX_DELAY);
			emulator_set_rpm(rpm);
		}
		mu_assert("error, engine->rpm != 7999", engine->rpm == 7999);
	}

	return 0;
}
/**
 * sweeps the engine from 1 to 8000 to 1 - 10 times
 */
static char * test_sweep_engine_zero_max_zero() {

	int rpm = 0;
	emulator_set_rpm(rpm);
	int count;
	for (count = 0; count < 10; count++) {
		for (rpm = 0; rpm < 8000; rpm++) {
			usleep(SWEEP_ENGINE_ZERO_TO_MAX_ZERO_DELAY);
			emulator_set_rpm(rpm);
		}
		mu_assert("error, engine->rpm != 7999", engine->rpm == 7999);
		for (rpm = 8000; rpm > 0; rpm--) {
			usleep(SWEEP_ENGINE_ZERO_TO_MAX_ZERO_DELAY);
			emulator_set_rpm(rpm);
		}
		mu_assert("error, engine->rpm != 1", engine->rpm == 1);
	}

	return 0;
}
/**
 * sets the rpm in jumps of 1000 and reverses the engine
 */

static char * test_sweep_engine_and_reverse() {

	int rpm = 0;

	int count;
	for (count = 0; count < 8; count++) {
		emulator_set_rpm(count * 1000);
		emulator_set_direction(count % 2 > 0);
		sleep(SWEEP_ENGINE_REVERSE_DELAY);
	}
	emulator_set_rpm(1000);
	emulator_set_direction(1);
	//setdirection(1);

	return 0;
}
/**
 * send some values after the the senshub orientation part
 */
static char * test_orientation_sweep() {

	int i, count = 0;
	printf("Starting %s\n", __PRETTY_FUNCTION__);

	for (count = 0; count < 10; count++) {
		for (i = 0; i < 1000; i++) {
			senshub->pitch = i;
			senshub->yaw = i;
			senshub->roll = 0;
			usleep(ORIENTATION_SWEEP_DELAY);
		}
		sleep(5);
	}
	for (count = 0; count < 10; count++) {
		for (i = 0; i < 1000; i++) {
			senshub->yaw = i;
			usleep(ORIENTATION_SWEEP_DELAY/5);
		}
	}
	sleep(5);

	for (count = 0; count < 10; count++) {
		for (i = 0; i < 1000; i++) {
			senshub->roll = i;
			usleep(ORIENTATION_SWEEP_DELAY/10);
		}
	}
	printf("Ending %s\n", __PRETTY_FUNCTION__);
	return 0;
}

static char * test_air_pressure() {
	printf("Starting %s\n", __PRETTY_FUNCTION__);
	int factor = 100;
	int pressure = 700 * factor;
	int i;
	int j;
	for (j = 0; j < 10; j++) {
		for (i = pressure; i < 1200 * factor; i++) {
			senshub->presure = i;
			usleep(AIR_PRESSURE_CHANGE_DELAY);
		}
		sleep(1);
		for (i = 1200 * factor; i > pressure; i--) {
			senshub->presure = i;
			usleep(AIR_PRESSURE_CHANGE_DELAY);
		}
		sleep(1);
	}
	printf("Ending %s\n", __PRETTY_FUNCTION__);
	return 0;
}

static char * test_brightness_sweep() {
	int brightness = 0;
	int i;
	int j;

	for (j = 0; j < 10; j++) {
		for (i = brightness; i < 100; i++) {
			senshub->light = i;
			usleep(BRIGHTNESS_CHANGE_DELAY);
		}
		sleep(1);
		for (i = 100; i > 0; i--) {
			senshub->light = i;
			usleep(BRIGHTNESS_CHANGE_DELAY);
		}
		sleep(1);
	}
	printf("Ending %s\n", __PRETTY_FUNCTION__);
	return 0;
}

static char * test_humidity_sweep() {
	int humidity = 0;
	int i;
	int j = 0;
	for (j = 0; j < 10; j++) {
		for (i = humidity; i < 100; i++) {
			senshub->humidity = i;
			usleep(HUMIDITY_CHANGE_DELAY);
		}
		sleep(1);
		for (i = 100; i > 0; i--) {
			senshub->humidity = i;
			usleep(HUMIDITY_CHANGE_DELAY);
		}
		sleep(1);
	}
	printf("Ending %s\n", __PRETTY_FUNCTION__);
	return 0;
}

static char * test_ambient_temperature_sweep() {
	printf("Starting %s\n", __PRETTY_FUNCTION__);
	int temp = -20;
	int i;
	int j = 0;
	for (j = 0; j < 10; j++) {
		for (i = temp; i < 50; i++) {
			senshub->ambtemp = i;
			usleep(AMBIENT_TEMPERATURE_CHANGE_DELAY);
		}
		sleep(1);
		for (i = 50; i > temp; i--) {
			senshub->ambtemp = i;
			usleep(AMBIENT_TEMPERATURE_CHANGE_DELAY);

		}
		sleep(1);
	}
	printf("Ending %s\n", __PRETTY_FUNCTION__);
	return 0;
}

static char * test_temperature_sweep() {
	printf("Starting %s\n", __PRETTY_FUNCTION__);
	int temp = -20;
	int i;
	int j = 0;
	for (j = 0; j < 10; j++) {
		for (i = temp; i < 50; i++) {
			senshub->objtemp = i;
			usleep(TEMPERATURE_CHANGE_DELAY);
		}
		sleep(1);
		for (i = 50; i > temp; i--) {
			senshub->objtemp = i;
			usleep(TEMPERATURE_CHANGE_DELAY);
		}
		sleep(1);
	}
	printf("Ending %s\n", __PRETTY_FUNCTION__);
	return 0;
}

void * runorientation_test()
{
        while(1)
        {
            mu_run_test(test_orientation_sweep);
        }
}

void * runtemperature_test()
{
        while(1)
        {
           mu_run_test(test_temperature_sweep);
        }
}

void * runambienttemperature_test()
{
        while(1)
        {
           mu_run_test(test_ambient_temperature_sweep);
        }
}

void * runhumidity_test()
{
        while(1)
        {
           mu_run_test(test_humidity_sweep);
        }
}

void * runbrightness_test()
{
        while(1)
        {
           mu_run_test(test_brightness_sweep);
        }
}

void * runairpressure_test()
{
        while(1)
        {
           mu_run_test(test_air_pressure);
        }
}

static char* runsenshub_tests() {

        pthread_create(&(threadId[RUN_ORIENTATION_THREAD_ID]), NULL, &runorientation_test, NULL);
        pthread_create(&(threadId[RUN_TEMPERATURE_THREAD_ID]), NULL, &runtemperature_test, NULL);
        pthread_create(&(threadId[RUN_AMBIENT_TEMPERATURE_THREAD_ID]), NULL, &runambienttemperature_test, NULL);
        pthread_create(&(threadId[RUN_HUMIDITY_THREAD_ID]), NULL, &runhumidity_test, NULL);
        pthread_create(&(threadId[RUN_BRIGHTNESS_THREAD_ID]), NULL, &runbrightness_test, NULL);
        pthread_create(&(threadId[RUN_AIRPRESSURE_THREAD_ID]), NULL, &runairpressure_test, NULL);

	return 0;
}

void * runengine_tests() {

	mu_run_test(test_sweep_engine_fast);
	mu_run_test(test_sweep_engine_zero_max);
	mu_run_test(test_sweep_engine_zero_max_zero);
	mu_run_test(test_sweep_engine_and_reverse);

	return 0;
}

static char * all_tests() {
	char * retval = 0;
	pthread_create(&(threadId[RUN_ENGINE_THREAD_ID]), NULL, &runengine_tests, NULL);
	retval = runsenshub_tests();
	return retval;
}
/*
 * setup called before all tests
 *
 */
int setup() {
	int retval = 0;
	int shmid = 0;

	shmid = shmget((key_t) shared_memory_engine, sizeof(engine_t),
			0666 | IPC_CREAT);
	if (shmid != -1) {
		shared_memory = shmat(shmid, (void *) 0, 0);
		engine = (engine_t *) shared_memory;
		retval = 1;
	}

	shmid = shmget((key_t) shared_memory_senshub, sizeof(senshub_t),
			0666 | IPC_CREAT);
	if (shmid != -1) {
		shared_memory = shmat(shmid, (void *) 0, 0);
		senshub = (senshub_t *) shared_memory;
		retval = 1;
	}
	return retval;
}

int main(int argc, char **argv) {
	int retval = setup();
	if (retval) {
	   char *result = all_tests();
           while(emulatorRunning != 0)
           {
           }
	} else {
		printf("setup failed!");
	}
	return retval;
}
