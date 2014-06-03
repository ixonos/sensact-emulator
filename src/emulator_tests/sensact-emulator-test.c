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
#include <errno.h>
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
#define RUN_BLE_THREAD_ID 7

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

int rpm_max_last = 0;

void *shared_memory;
senshub_t * senshub;
engine_t *engine;
ble_t *ble;
/**
 *dummy to write RPM
 */

void emulator_set_direction(int direction) {
	engine->direction = direction;
}

void emulator_set_rpm(int rpm) {
  if (engine) {
  	engine->rpm = rpm;
  }
}

int emulator_get_rpm() {
	return engine->rpm;
}

int emulator_get_rpm_max(){
  return engine->rpm_max;
}

static char * test_engine_rand_value() {
  int l_rpm_max = emulator_get_rpm_max();
  emulator_set_rpm(l_rpm_max - (rand() % 50));
  return 0;
}

static char * test_engine_setting_its_value() {
 emulator_set_rpm(0);
 int l_rpm = emulator_get_rpm();
 int l_rpm_max = emulator_get_rpm_max();

 if (rpm_max_last != l_rpm_max) {
   rpm_max_last = l_rpm_max;
   if (l_rpm <= l_rpm_max){
     for (; l_rpm <= l_rpm_max ; l_rpm++) {
       emulator_set_rpm(l_rpm);
   }
  }
  else {
    for (l_rpm = l_rpm_max; l_rpm > l_rpm_max ; l_rpm--) {
       emulator_set_rpm(l_rpm);
    }
  }
 } 
 else {
   test_engine_rand_value();
 }
 return 0;
}


/**
 * sweeps the engine from 1 to max to 1 - 10 times
 */
static char * test_sweep_engine_fast() {
  printf("test_sweep_engine_fast\n");
	int rpm = 0;
	emulator_set_rpm(rpm);
	int count;
	for (count = 0; count < 10; count++) {
		for (rpm = 0; rpm < 8000; rpm++) {
			emulator_set_rpm(rpm);
			usleep(SWEEP_ENGINE_ZERO_TO_MAX_DELAY);
		}
		sleep(SWEEP_ENGINE_FAST_DELAY);
		mu_assert("error, engine->rpm != 7999", engine->rpm == 7999);
		for (rpm = 8000; rpm > 0; rpm--) {
			emulator_set_rpm(rpm);
			usleep(SWEEP_ENGINE_ZERO_TO_MAX_DELAY);
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
  printf("test_sweep_engine_zero_max\n");
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
  printf("test_sweep_engine_zero_max_zero\n");
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
	return 0;
}

static char * test_air_pressure() {
  senshub->presure = 800 - (rand() % 50);
	return 0;
}

static char * test_brightness_sweep() {
  senshub->light = 80 - (rand() % 10);
	return 0;
}

static char * test_humidity_sweep() {
  senshub->humidity = 80 - (rand() % 20);
	return 0;
}

static char * test_ambient_temperature_sweep() {
  senshub->ambtemp = 0 - (rand() % 5);
	return 0;
}

static char * test_temperature_sweep() {
  senshub->objtemp = 20 - (rand() % 5);
	return 0;
}


static char * test_ble_sweep() {
  ble->temp = 20 - (rand() % 5);
	return 0;
}



void * runorientation_test()
{
        while(1)
        {
            mu_run_test(test_orientation_sweep);
            sleep(1);
        }
}

void * runtemperature_test()
{
        while(1)
        {
           mu_run_test(test_temperature_sweep);
           sleep(1);
        }
}

void * runambienttemperature_test()
{
        while(1)
        {
           mu_run_test(test_ambient_temperature_sweep);
           usleep(500000);
        }
}

void * runhumidity_test()
{
        while(1)
        {
           mu_run_test(test_humidity_sweep);
           usleep(1000000);
        }
}

void * runbrightness_test()
{
        while(1)
        {
           mu_run_test(test_brightness_sweep);
           usleep(900000);
        }
}

void * runairpressure_test()
{
        while(1)
        {
           mu_run_test(test_air_pressure);
           usleep(900000);
        }
}


void * runbletemp_test() {
  while (1) {
    test_ble_sweep();
    usleep(800000);
  }
}

static char* runsenshub_tests() {

        pthread_create(&(threadId[RUN_ORIENTATION_THREAD_ID]), NULL, &runorientation_test, NULL);
        pthread_create(&(threadId[RUN_TEMPERATURE_THREAD_ID]), NULL, &runtemperature_test, NULL);
        pthread_create(&(threadId[RUN_AMBIENT_TEMPERATURE_THREAD_ID]), NULL, &runambienttemperature_test, NULL);
        pthread_create(&(threadId[RUN_HUMIDITY_THREAD_ID]), NULL, &runhumidity_test, NULL);
        pthread_create(&(threadId[RUN_BRIGHTNESS_THREAD_ID]), NULL, &runbrightness_test, NULL);
        pthread_create(&(threadId[RUN_AIRPRESSURE_THREAD_ID]), NULL, &runairpressure_test, NULL);
        pthread_create(&(threadId[RUN_BLE_THREAD_ID]), NULL, &runbletemp_test, NULL);

	return 0;
}


void * runengine_tests() {
 while(1) {
   mu_run_test(test_engine_setting_its_value);
   usleep(100000);
 }
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

  printf("setup() : shared_memory_senshub = %d\n",shared_memory_senshub);
	shmid = shmget((key_t) shared_memory_senshub, sizeof(senshub_t),
			0666 | IPC_CREAT);
  printf("setup() : shmid = %d\n",shmid);
	if (shmid == -1) {
		printf("shmget failed %s \n", strerror( errno));
	} else {
		shared_memory = shmat(shmid, (void *) 0, 0);
		senshub = (senshub_t *) shared_memory;
		retval = 1;
  } 

	shmid = shmget((key_t) shared_memory_engine, sizeof(engine_t),0666 | IPC_CREAT);
  printf("setup() : shmid = %d\n",shmid);
	if (shmid == -1) {
		printf("shmget failed %s \n", strerror( errno));
	} else {
    printf("setup:shmid\n");
		shared_memory = shmat(shmid, (void *) 0, 0);
    printf("setup:shmid:shared_memory 0x%x\n",shared_memory);
		engine = (engine_t *) shared_memory;
    printf("setup:shmid:engine 0x%x\n",engine);     
		retval = 1;
  }


	shmid = shmget((key_t) shared_memory_ble, sizeof(ble_t),0666 | IPC_CREAT);
  printf("setup() : shmid = %d\n",shmid);
	if (shmid == -1) {
		printf("shmget failed %s \n", strerror( errno));
	} else {
    printf("setup:shmid\n");
		shared_memory = shmat(shmid, (void *) 0, 0);
    printf("setup:shmid:shared_memory 0x%x\n",shared_memory);
		ble = (ble_t *) shared_memory;
    printf("setup:shmid:ble 0x%x\n",ble);     
		retval = 1;
  }



	return retval;
}

int main(int argc, char **argv) {

  time_t t;

  srand((unsigned) time(&t));
  printf("main()\n");
	int retval = setup();
  printf("main() : retval = %d\n",retval);
	if (retval) {
	   char *result = all_tests();
           while(emulatorRunning != 0)
           {
             sleep(1);
           }
	} else {
		printf("setup failed!");
	}
	return retval;
}
