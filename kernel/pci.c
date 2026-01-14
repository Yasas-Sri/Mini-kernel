#include "pci.h"
#include "../drivers/vga.h"

// Low-level port I/O
static inline void outl(uint16_t port, uint32_t val)
{
    __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Read PCI config space
static uint32_t pci_read_dword(uint8_t bus, uint8_t device,
                               uint8_t function, uint8_t offset)
{
    uint32_t address;

    address = (1 << 31) | ((uint32_t)bus << 16) | ((uint32_t)device << 11) | ((uint32_t)function << 8) | (offset & 0xFC);

    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

static uint16_t pci_read_word(uint8_t bus, uint8_t device,
                              uint8_t function, uint8_t offset)
{
    uint32_t value = pci_read_dword(bus, device, function, offset);
    return (value >> ((offset & 2) * 8)) & 0xFFFF;
}

// Scan bus 0 only (enough for QEMU)
void pci_scan_bus(void)
{
    vga_write_string("\nPCI Scan:\n");

    for (uint8_t dev = 0; dev < 32; dev++)
    {
        uint16_t vendor = pci_read_word(0, dev, 0, PCI_VENDOR_ID);
        if (vendor == 0xFFFF)
            continue;

        uint16_t device = pci_read_word(0, dev, 0, PCI_DEVICE_ID);
        uint8_t class = pci_read_word(0, dev, 0, PCI_CLASS_CODE) >> 8;
        uint8_t irq = pci_read_word(0, dev, 0, PCI_INTERRUPT_LINE) & 0xFF;

        vga_write_string(" Device ");
        vga_write_hex(dev);
        vga_write_string(": Vendor=");
        vga_write_hex(vendor);
        vga_write_string(" Device=");
        vga_write_hex(device);
        vga_write_string(" Class=");
        vga_write_hex(class);
        vga_write_string(" IRQ=");
        vga_write_hex(irq);
        vga_write_string("\n");
    }
}

// Init entry point
void pci_init(void)
{
    vga_write_string("[*] Initializing PCI...\n");
}

void pci_print_devices()
{
    // Simply call scan_bus for now
    pci_scan_bus();
}
