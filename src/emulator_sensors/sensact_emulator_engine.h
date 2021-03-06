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

#ifndef SENS_EMULATOR_ENGINE_H_
#define SENS_EMULATOR_ENGINE_H_
#define shared_memory_engine 1234

/**
 *A little engine, with rpm, rpm_max and motor direction
 */


typedef struct {
	int rpm;
	const char * rpm_name;
	void (*setrpm)(int rpm);
	int (*getrpm)(void);

  int rpm_max;
	const char * rpm_max_name;
	void (*setrpm_max)(int rpm_max);
	int (*getrpm_max)(void);

	uint8_t direction;
	const char * direction_name;
	void (*setdirection)(uint8_t direction);
	uint8_t (*getdirection)(void);

} engine_t;

/**
 * call to create a engine
 */
engine_t *create_emulator_engine();
/**
 * destroy and detach memory
 */
void destroy_engine_emulator(void);
/**
 * setRpm_max
 */
void setrpm_max(int newrpm);
/*
 * getRpm_max
 *
 */
int getrpm_max(void);

/**
 * setRpm
 */
void setrpm(int newrpm);
/*
 * getRpm
 *
 */
int getrpm(void);
/*
 * setDirection of the engine
 */
void setdirection(uint8_t direction);
/*
 * getDirection of the engine
 */
uint8_t getdirection();
/*
 * Start the emulator
 */
void start();

#endif /* SENS_EMULATOR_ENGINE_H_ */
