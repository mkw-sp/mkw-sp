#pragma once

typedef struct OSContext
{
	unsigned int gprs[32];

	unsigned int cr;
	unsigned int lr;
	unsigned int ctr;
	unsigned int xer;

	double fprs[32];
	unsigned int _pad400;
	unsigned int fpscr;

	unsigned int srr0;
	unsigned int srr1;

	unsigned short mode;
	unsigned short state;

	unsigned int gqrs[8];
	unsigned int _pad452;
	double psf[32];
} OSContext;

unsigned int OSGetStackPointer(void);
void OSFillFPUContext(OSContext* osContext);
