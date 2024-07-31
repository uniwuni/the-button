#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID  0x2e5a
#define PRODUCT_ID 0x2015
#define ENDPOINT_OUT 0x02  // Adjust the endpoint address as needed
#define TIMEOUT 1000       // Timeout for the transfer in milliseconds

int main(int argc, char *argv[]) {
    libusb_device_handle *handle;
    libusb_context *ctx = NULL;
    int r;
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <on/off>\n", argv[0]);
        return EXIT_FAILURE;
    }
    unsigned char data[32] =
        {0x30, 0x00, 0x30, 0x30, //0x31 turns on, 0x30 turns off
         0x30, 0x30, 0x30, 0x30,
         0x30, 0x30, 0x30, 0x30,
         0x30, 0x30, 0x30, 0x30,
         0x00, 0x3e, 0x00, 0x0d,
         0x00, 0xa0, 0x00, 0x20,
         0x00, 0x20, 0x00, 0x3c,
         0x00, 0x21, 0x00, 0x2d};
    if(argv[1][0] == 'o' && argv[1][1] == 'n') {
        data[1] = 0x31;
    } else if(argv[1][0] == 'o' && argv[1][1] == 'f' && argv[1][2] == 'f') {
        data[1] = 0x30;
    } else {
        fprintf(stderr, "Usage: %s <on/off>\n", argv[0]);
        return EXIT_FAILURE;
    }
    // Initialize libusb
    r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "Failed to initialize libusb: %s\n", libusb_error_name(r));
        return EXIT_FAILURE;
    }

    // Open the device with the specified vendor and product IDs
    handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);
    if (handle == NULL) {
        fprintf(stderr, "Could not open USB device\n");
        libusb_exit(ctx);
        return EXIT_FAILURE;
    }
    // Detach the kernel driver if necessary
    int interface_number = 0;
    if (libusb_kernel_driver_active(handle, interface_number) == 1) {
        r = libusb_detach_kernel_driver(handle, interface_number);
        if (r < 0) {
            fprintf(stderr, "Could not detach kernel driver: %s\n", libusb_error_name(r));
            libusb_close(handle);
            libusb_exit(ctx);
            return EXIT_FAILURE;
        }
    }
    // Claim the interface (you might need to adjust the interface number)
    r = libusb_claim_interface(handle, interface_number);
    if (r < 0) {
        fprintf(stderr, "Failed to claim interface: %s\n", libusb_error_name(r));
        libusb_close(handle);
        libusb_exit(ctx);
        return EXIT_FAILURE;
    }

    // Perform the bulk transfer
    int actual_length;
    r = libusb_bulk_transfer(handle, ENDPOINT_OUT, data, sizeof(data), &actual_length, TIMEOUT);
    if (r == 0 && actual_length == sizeof(data)) {
        printf("Successfully wrote %d bytes\n", actual_length);
    } else {
        fprintf(stderr, "Error during bulk transfer: %s\n", libusb_error_name(r));
    }

    // Release the interface and close the device
    libusb_release_interface(handle, interface_number);
    libusb_attach_kernel_driver(handle, interface_number);
    libusb_close(handle);
    libusb_exit(ctx);

    return EXIT_SUCCESS;
}
