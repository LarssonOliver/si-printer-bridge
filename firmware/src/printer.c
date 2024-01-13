#include <stdint.h>
#include <tusb.h>

#include "console.h"
#include "printer.h"

// Only allow printing if initialized.
static int s_initialized = 0;

static uint16_t count_interface_total_len(const tusb_desc_interface_t *desc_itf,
                                          uint8_t itf_count, uint16_t max_len);

static int open_printer_interface(uint8_t dev_addr,
                                  const tusb_desc_interface_t *desc_itf,
                                  uint16_t max_len);

// Initialize a printer. Returns 0 on success.
int printer_init(uint8_t dev_addr) {
  s_initialized = 0;

  uint8_t buffer[256];
  tusb_desc_configuration_t *desc_configuration =
      (tusb_desc_configuration_t *)buffer;

  uint8_t res = tuh_descriptor_get_configuration_sync(dev_addr, 0, &buffer,
                                                      sizeof(buffer));

  if (res != XFER_RESULT_SUCCESS)
    return -1;

  const uint8_t *desc_end = (const uint8_t *)desc_configuration +
                            tu_le16toh(desc_configuration->wTotalLength);

  const uint8_t *p_desc = tu_desc_next(desc_configuration);

  while (p_desc < desc_end) {
    uint8_t assoc_itf_count = 1;

    if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE_ASSOCIATION) {
      const tusb_desc_interface_assoc_t *desc_iad =
          (const tusb_desc_interface_assoc_t *)p_desc;
      assoc_itf_count = desc_iad->bInterfaceCount;
      p_desc = tu_desc_next(p_desc);
    }

    if (tu_desc_type(p_desc) != TUSB_DESC_INTERFACE) {
      console_printf("Unexpected descriptor type: %02x\r\n",
                     tu_desc_type(p_desc));
      return -1;
    }

    const tusb_desc_interface_t *desc_itf =
        (const tusb_desc_interface_t *)p_desc;

    const uint16_t drv_len = count_interface_total_len(
        desc_itf, assoc_itf_count, (desc_end - p_desc));

    if (drv_len < sizeof(tusb_desc_interface_t))
      return -1;

    console_printf("  Interface: %u\r\n", desc_itf->bInterfaceNumber);
    console_printf("    Class: 0x%02x\r\n", desc_itf->bInterfaceClass);
    console_printf("    Subclass: 0x%02x\r\n", desc_itf->bInterfaceSubClass);
    console_printf("    Protocol: 0x%02x\r\n", desc_itf->bInterfaceProtocol);
    console_printf("    Endpoints: %u\r\n", desc_itf->bNumEndpoints);

    if (desc_itf->bInterfaceClass == TUSB_CLASS_PRINTER)
      return open_printer_interface(dev_addr, desc_itf, drv_len);

    p_desc += drv_len;
  }

  return 0;
}

static uint16_t count_interface_total_len(const tusb_desc_interface_t *desc_itf,
                                          uint8_t itf_count, uint16_t max_len) {

  const uint8_t *p_desc = (const uint8_t *)desc_itf;
  uint16_t len = 0;

  while (itf_count--) {
    len += tu_desc_len(p_desc);
    p_desc = tu_desc_next(p_desc);

    while (len < max_len) {
      if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE_ASSOCIATION)
        return len;

      if (tu_desc_type(p_desc) == TUSB_DESC_INTERFACE &&
          ((const tusb_desc_interface_t *)p_desc)->bAlternateSetting == 0)
        break;

      len += tu_desc_len(p_desc);
      p_desc = tu_desc_next(p_desc);
    }
  }

  return len;
}

static int open_printer_interface(uint8_t dev_addr,
                                  const tusb_desc_interface_t *desc_itf,
                                  uint16_t max_len) {
  (void)dev_addr;
  (void)desc_itf;
  (void)max_len;

  return 0;
}
