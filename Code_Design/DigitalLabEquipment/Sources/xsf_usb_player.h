/*
 * xsf_usb_player.h
 *
 *  Created on: 10 Jan 2020
 *      Author: podonoghue
 */

#ifndef SOURCES_XSF_USB_PLAYER_H_
#define SOURCES_XSF_USB_PLAYER_H_

/**
 * Poll USB interface for activity
 *
 * If active the return may be very much delayed e.g. programming of a device.
 */
void pollUsb();

#endif /* SOURCES_XSF_USB_PLAYER_H_ */
