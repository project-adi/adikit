#ifndef __X86_64_H__
#define __X86_64_H__

#include <stdint.h>

#include <core.h>

typedef struct {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rsi;
	uint64_t rdi;
	uint64_t rbp;
	uint64_t r8;
	uint64_t r9;
	uint64_t r10;
	uint64_t r11;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	uint64_t interrupt;
	uint64_t error;

	uint64_t rip;
	uint64_t cs;
	uint64_t rflags;
	uint64_t rsp;
	uint64_t ss;
} context_t;

typedef void (*handler)(adi_device_t* dev, context_t* ctx); ;

typedef struct {
    metalanguage_t (*new)();
    bool (*register_irq)(uint8_t irq, handler h);
    bool (*unregister_irq)(uint8_t irq);

    void (*outb)(uint16_t port, uint8_t value);
    uint8_t (*inb)(uint16_t port);

    void (*outw)(uint16_t port, uint16_t value);
    uint16_t (*inw)(uint16_t port);

    void (*outl)(uint16_t port, uint32_t value);
    uint32_t (*inl)(uint16_t port); 
} metalang_x86_64_t;

#endif // __X86_64_H__