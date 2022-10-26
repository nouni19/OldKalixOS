#include "fat32.h"
#include "ramdisk.h"
#include "../mem/pmm_mem.h"
#include "../drivers/vga.h"
#include "../misc/utils.h"
bootrec_t* BRec;
FSInfo_t* FSinfo;
uint32_t* FAT;
#define FATsec(fati) ((BRec->bpb.ressec + (fati) * BRec->ebr.secperFAT))
#define DATAsec ((BRec->bpb.ressec + (BRec->bpb.FATnum * BRec->ebr.secperFAT)))
#define Clsec BRec->bpb.secpercl
void fat32init(){
	char loadedAt[1024];
	BRec = (bootrec_t*)loadedAt;
	FSinfo = (FSInfo_t*)(loadedAt+512);
	rdiskread(loadedAt, 0, 1);
	rdiskread(&loadedAt[512], BRec->ebr.FSInfosec, 1);
	FAT = (uint32_t*)(0x12345678);
	rdiskread(FAT, FATsec(0), BRec->ebr.secperFAT);
}
char* getVolumeLabel(char* buf){
	for(int i = 0; i<11; i++){
		buf[i] = BRec->ebr.volumelabel[i];
	}
	buf[11] = '\0';
	return buf;
}

uint32_t locateFileCl(char* dir){
	uint32_t ccl = BRec->ebr.rootclnum;
	dire* cdire = pmalloc((BRec->bpb.secpercl / 8)+1) + hhdm;
	char tempstr[12];
	tempstr[11] = '\0';
	uint32_t stroffset = 0;
newloop1:
	rdiskread(cdire, ccl*BRec->bpb.secpercl, BRec->bpb.secpercl);
	for(int i = 0; i<BRec->bpb.secpercl*16; i++){
		if(cdire[i].entr.attr == 0x0F) continue;
		memcpy(tempstr, dir+stroffset, 11);
		if(strcmp(cdire[i].entr.name, tempstr) == 0){
			if(dir[(stroffset/11*11)+11] == '\0'){ 
				pfree((uint64_t)cdire, (BRec->bpb.secpercl / 8)+1);
				return (cdire[i].entr.clhigh<<16) | cdire[i].entr.cllow; 
			}
			ccl = (cdire[i].entr.clhigh<<16) | cdire[i].entr.cllow;
			stroffset += 12;
			goto newloop1;
		}
	}
	return 0;
}
/*
file_t* listDir(char* dir, file_t* buf){
	uint32_t cl = locateFileCl(dir);
	dire* testeroo = pmalloc((BRec->bpb.secpercl / 8)+1) + hhdm;
	rdiskread(testeroo, (cl * BRec->bpb.secpercl) + DATAsec, BRec->bpb.secpercl);
	printsl(0, 2, testeroo[1].entr.name, 0xff, 0, 11);
	pfree(testeroo-hhdm, (BRec->bpb.secpercl / 8)+1);
}
*/