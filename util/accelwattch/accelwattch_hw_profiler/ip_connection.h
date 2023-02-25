/*
 * Copyright (C) 2012-2014, 2019 Matthias Bolte <matthias@tinkerforge.com>
 * Copyright (C) 2011 Olaf Lüke <olaf@tinkerforge.com>
 *
 * Redistribution and use in source and binary forms of this file,
 * with or without modification, are permitted. See the Creative
 * Commons Zero (CC0 1.0) License for more details.
 */

#ifndef IP_CONNECTION_H
#define IP_CONNECTION_H

/**
 * \defgroup IPConnection IP Connection
 */

#ifndef __STDC_LIMIT_MACROS
	#define __STDC_LIMIT_MACROS
#endif
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#if (!defined __cplusplus && defined __GNUC__) || (defined _MSC_VER && _MSC_VER >= 1600)
	#include <stdbool.h>
#endif

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#include <windows.h>
#else
	#include <pthread.h>
	#include <semaphore.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum {
	E_OK = 0,
	E_TIMEOUT = -1,
	E_NO_STREAM_SOCKET = -2,
	E_HOSTNAME_INVALID = -3,
	E_NO_CONNECT = -4,
	E_NO_THREAD = -5,
	E_NOT_ADDED = -6, // unused since v2.0
	E_ALREADY_CONNECTED = -7,
	E_NOT_CONNECTED = -8,
	E_INVALID_PARAMETER = -9, // error response from device
	E_NOT_SUPPORTED = -10, // error response from device
	E_UNKNOWN_ERROR_CODE = -11, // error response from device
	E_STREAM_OUT_OF_SYNC = -12,
	E_INVALID_UID = -13
};

#ifdef IPCON_EXPOSE_MILLISLEEP

void millisleep(uint32_t msec);

#endif // IPCON_EXPOSE_MILLISLEEP

#ifdef IPCON_EXPOSE_INTERNALS

typedef struct _Socket Socket;

typedef struct {
#ifdef _WIN32
	CRITICAL_SECTION handle;
#else
	pthread_mutex_t handle;
#endif
} Mutex;

void mutex_create(Mutex *mutex);

void mutex_destroy(Mutex *mutex);

void mutex_lock(Mutex *mutex);

void mutex_unlock(Mutex *mutex);

typedef struct {
#ifdef _WIN32
	HANDLE handle;
#else
	pthread_cond_t condition;
	pthread_mutex_t mutex;
	bool flag;
#endif
} Event;

typedef struct {
#ifdef _WIN32
	HANDLE handle;
#else
	sem_t object;
	sem_t *pointer;
#endif
} Semaphore;

typedef void (*ThreadFunction)(void *opaque);

typedef struct {
#ifdef _WIN32
	HANDLE handle;
	DWORD id;
#else
	pthread_t handle;
#endif
	ThreadFunction function;
	void *opaque;
} Thread;

typedef struct {
	Mutex mutex;
	int used;
	int allocated;
	uint32_t *keys;
	void **values;
} Table;

typedef struct _QueueItem {
	struct _QueueItem *next;
	int kind;
	void *data;
} QueueItem;

typedef struct {
	Mutex mutex;
	Semaphore semaphore;
	QueueItem *head;
	QueueItem *tail;
} Queue;

#if defined _MSC_VER || defined __BORLANDC__
	#pragma pack(push)
	#pragma pack(1)
	#define ATTRIBUTE_PACKED
#elif defined __GNUC__
	#ifdef _WIN32
		// workaround struct packing bug in GCC 4.7 on Windows
		// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=52991
		#define ATTRIBUTE_PACKED __attribute__((gcc_struct, packed))
	#else
		#define ATTRIBUTE_PACKED __attribute__((packed))
	#endif
#else
	#error unknown compiler, do not know how to enable struct packing
#endif

typedef struct {
	uint32_t uid; // always little endian
	uint8_t length;
	uint8_t function_id;
	uint8_t sequence_number_and_options;
	uint8_t error_code_and_future_use;
} ATTRIBUTE_PACKED PacketHeader;

typedef struct {
	PacketHeader header;
	uint8_t payload[64];
	uint8_t optional_data[8];
} ATTRIBUTE_PACKED Packet;

#if defined _MSC_VER || defined __BORLANDC__
	#pragma pack(pop)
#endif
#undef ATTRIBUTE_PACKED

#endif // IPCON_EXPOSE_INTERNALS

typedef struct _IPConnection IPConnection;
typedef struct _IPConnectionPrivate IPConnectionPrivate;
typedef struct _Device Device;
typedef struct _DevicePrivate DevicePrivate;

#ifdef IPCON_EXPOSE_INTERNALS

typedef struct _CallbackContext CallbackContext;
typedef struct _HighLevelCallback HighLevelCallback;

/**
 * \internal
 */
struct _HighLevelCallback {
	bool exists;
	void *data;
	size_t length;
};

#endif

typedef void (*EnumerateCallbackFunction)(const char *uid,
                                          const char *connected_uid,
     