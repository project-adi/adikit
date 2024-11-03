#ifndef __PCI_H__
#define __PCI_H__

#include <stdint.h>

#include <core.h>

typedef struct {
    metalanguage_t (*new)();
    uint8_t (*pci_read)(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg);
    void (*pci_write)(uint8_t bus, uint8_t dev, uint8_t func, uint8_t reg, uint8_t value); 
} metalang_pci_t;

#endif // __PCI_H__