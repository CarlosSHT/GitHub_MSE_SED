/** \file crono.h
 *  Mar 2022
 *  Maestría en SIstemas Embebidos 
 */

#ifndef CRONO_H
#define CRONO_H

typedef void (*extFunIRQ)(void);

/* Prototipos */
// Timers
void CRONO_timerInit(void* arg);
void CRONO_timerStart(uint64_t);
void CRONO_timerStop(void);

// Delays/Sleeps
void CRONO_delayMs(int);
void CRONO_sleepMs(uint64_t);

// SNTP
void CRONO_sntpInit(void);
int64_t CRONO_getTime(char *, int);

#endif  /* CRONO_H_ */
