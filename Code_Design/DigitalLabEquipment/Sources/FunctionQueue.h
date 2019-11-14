/*
 * FunctionQueue.h
 *
 *  Created on: 14 Nov 2019
 *      Author: podonoghue
 */

#ifndef SOURCES_FUNCTIONQUEUE_H_
#define SOURCES_FUNCTIONQUEUE_H_

#include "queue.h"

using FunctionPtr   = void (*)();
using FunctionQueue = USBDM::Queue<FunctionPtr, 10>;

#endif /* SOURCES_FUNCTIONQUEUE_H_ */
