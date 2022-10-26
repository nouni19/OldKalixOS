#include "stdint.h"
typedef struct{
	uint8_t first3[3]; //0
	uint8_t OEM[8]; //3
	uint16_t bytespersec; //11
	uint8_t secpercl; //13
	uint16_t ressec; //14
	uint8_t FATnum; //16
	uint16_t rdirnum; //17
	uint16_t totalsec; //19
	uint8_t mdt; //21
	uint16_t reserved; //22
	uint16_t secpertr; //24
	uint16_t headnum; //26
	uint32_t hiddensecnum; //28
	uint32_t lseccount; //32
} __attribute__((packed)) BPB;
typedef struct{
	uint32_t secperFAT; //36
	uint16_t flags; //40
	uint16_t FATver; //42
	uint32_t rootclnum; //44
	uint16_t FSInfosec; //48
	uint16_t backupsec; //50
	uint64_t reserved; //52
	uint32_t reserved2;
	uint8_t drivenum; //64
	uint8_t reserved3; //65
	uint8_t signature; //66
	uint32_t volumeserial; //67
	uint8_t volumelabel[11]; //71
	uint8_t sysid[8];
	uint8_t bootcode[420];
	uint16_t bootsign; 	 
} __attribute__((packed)) EBR;
typedef struct{
	BPB bpb;
	EBR ebr;
} __attribute__((packed)) bootrec_t;
typedef struct{
	uint32_t signature; //0
	uint8_t reserved[480]; //4
	uint32_t signature2; //484
	uint32_t freeclcount; //488
	uint32_t recentcl; //492
	uint8_t reserved2[12]; //496
	uint32_t signature3; //508
} __attribute__((packed)) FSInfo_t;

typedef union {
	struct {
	    char name[11];
	    uint8_t attr;
	    uint8_t data1[8];
	    uint16_t clhigh;
	    uint8_t data2[4];
	    uint16_t cllow;
	    uint32_t size;
	} __attribute__((packed)) entr;
	struct { 
		uint8_t order;
		char first5[10];
		uint8_t attr;
		uint8_t longtype;
		uint8_t checksum;
		char next6[12];
		uint16_t zero;
		char final2[4];
	} __attribute__((packed)) lfne;
} dire;

typedef struct{
	uint32_t bytes;
	uint8_t attr;
	char* name;
} file_t;

void fat32init();
char* getVolumeLabel(char* buf);
uint32_t locateFileCl(char* dir);
file_t* listDir(char* dir, file_t* buf);