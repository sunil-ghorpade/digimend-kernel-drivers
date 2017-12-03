/*
 *  HID driver for ViewSonic devices not fully compliant with HID standard
 *
 *  Copyright (c) 2017 Nikolai Kondrashov
 */

/*
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 */

#include <linux/device.h>
#include <linux/hid.h>
#include <linux/module.h>
#include <linux/usb.h>

#include "hid-ids.h"

/* Size of the original descriptor of tablet with PID e621 */
#define PID_E621_RDESC_ORIG_SIZE	408

/* Fixed report descriptor of tablet with PID e621 */
static __u8 pid_e621_rdesc_fixed[] = {
	0x05, 0x0D,             /*  Usage Page (Digitizer),             */
	0x09, 0x02,             /*  Usage (Pen),                        */
	0xA1, 0x01,             /*  Collection (Application),           */
	0x85, 0x02,             /*      Report ID (2),                  */
	0x09, 0x20,             /*      Usage (Stylus),                 */
	0xA0,                   /*      Collection (Physical),          */
	0x75, 0x10,             /*          Report Size (16),           */
	0x95, 0x01,             /*          Report Count (1),           */
	0xA4,                   /*          Push,                       */
	0x05, 0x01,             /*          Usage Page (Desktop),       */
	0x65, 0x13,             /*          Unit (Inch),                */
	0x55, 0xFD,             /*          Unit Exponent (-3),         */
	0x34,                   /*          Physical Minimum (0),       */
	0x09, 0x30,             /*          Usage (X),                  */
	0x46, 0xB1, 0x21,       /*          Physical Maximum (8625),    */
	0x27, 0x2C, 0xAB,
		0x00, 0x00,     /*          Logical Maximum (43820),    */
	0x81, 0x02,             /*          Input (Variable),           */
	0x09, 0x31,             /*          Usage (Y),                  */
	0x46, 0x0E, 0x15,       /*          Physical Maximum (5390),    */
	0x26, 0xF6, 0x6A,       /*          Logical Maximum (27382),    */
	0x81, 0x02,             /*          Input (Variable),           */
	0xB4,                   /*          Pop,                        */
	0x14,                   /*          Logical Minimum (0),        */
	0x25, 0x01,             /*          Logical Maximum (1),        */
	0x75, 0x01,             /*          Report Size (1),            */
	0x95, 0x01,             /*          Report Count (1),           */
	0x81, 0x03,             /*          Input (Constant, Variable), */
	0x09, 0x32,             /*          Usage (In Range),           */
	0x09, 0x42,             /*          Usage (Tip Switch),         */
	0x95, 0x02,             /*          Report Count (2),           */
	0x81, 0x02,             /*          Input (Variable),           */
	0x95, 0x05,             /*          Report Count (5),           */
	0x81, 0x03,             /*          Input (Constant, Variable), */
	0x75, 0x10,             /*          Report Size (16),           */
	0x95, 0x01,             /*          Report Count (1),           */
	0x09, 0x30,             /*          Usage (Tip Pressure),       */
	0x26, 0xFF, 0x07,       /*          Logical Maximum (2047),     */
	0x81, 0x02,             /*          Input (Variable),           */
	0x75, 0x10,             /*          Report Size (16),           */
	0x95, 0x01,             /*          Report Count (1),           */
	0x81, 0x03,             /*          Input (Constant, Variable), */
	0xC0,                   /*      End Collection,                 */
	0xC0                    /*  End Collection                      */
};

static __u8 *viewsonic_report_fixup(struct hid_device *hdev, __u8 *rdesc,
				    unsigned int *rsize)
{
	switch (hdev->product) {
	case USB_DEVICE_ID_VIEWSONIC_PID_E621:
		if (*rsize == PID_E621_RDESC_ORIG_SIZE) {
			rdesc = pid_e621_rdesc_fixed;
			*rsize = sizeof(pid_e621_rdesc_fixed);
		}
		break;
	}

	return rdesc;
}

static int viewsonic_probe(struct hid_device *hdev,
			   const struct hid_device_id *id)
{
	int rc;

	hdev->quirks |= HID_QUIRK_MULTI_INPUT;

	rc = hid_parse(hdev);
	if (rc) {
		hid_err(hdev, "parse failed\n");
		return rc;
	}

	rc = hid_hw_start(hdev, HID_CONNECT_DEFAULT);
	if (rc) {
		hid_err(hdev, "hw start failed\n");
		return rc;
	}

	return 0;
}

static const struct hid_device_id viewsonic_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_VIEWSONIC,
				USB_DEVICE_ID_VIEWSONIC_PID_E621) },
	{ }
};
MODULE_DEVICE_TABLE(hid, viewsonic_devices);

static struct hid_driver viewsonic_driver = {
	.name = "viewsonic",
	.id_table = viewsonic_devices,
	.probe = viewsonic_probe,
	.report_fixup = viewsonic_report_fixup,
};
module_hid_driver(viewsonic_driver);

MODULE_LICENSE("GPL");
MODULE_VERSION("7");
