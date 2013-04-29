#include <linux/sys_stat.h>
#include <linux/string.h>
#include <linux/module.h>

#define MAX_RECORDS	32	
#define MAX_SYS_CALLS   16	

struct SysStat {
	int uid;
	int count;
	char comm[16];
	char sys_call[16];	
};

static struct SysStat record[MAX_SYS_CALLS][MAX_RECORDS];
static int recordPointer[MAX_SYS_CALLS];
static int sysCallPointer = -1;

int maxCount(int sys_call_index);
void addNewRecord(char *comm, int uid, char *sys_call, int sys_call_index);
void addNewSysCall(char *comm, int uid, char *sys_call);

void cse812_clear(void) {
	int i;
	int j;
	for (i = 0; i < MAX_SYS_CALLS; i++) {
		for (j = 0; j < MAX_RECORDS; j++) {
			strcpy(record[i][j].comm,"");
			record[i][j].uid = -1;
			strcpy(record[i][j].sys_call,"");
			record[i][j].count = 0;
		}	
		recordPointer[i] = -1;
	}
	sysCallPointer = 0;
	printk("Record cleared");
}

void cse812_off(void) {
	
}

void cse812_set(char *comm, int uid, char *sys_call) {
	int i;
	int j;
	bool sysCallPresent = false;
	bool recordPresent = false;

	if (sysCallPointer >= 0) {
		for (i = 0; i < MAX_SYS_CALLS; i++) {
			if (strcmp(record[i][0].sys_call,sys_call) == 0) { // Only do if initialized, and the sys_call is equal 
				sysCallPresent = true;
				if (recordPointer[i] >= 0) {	
					for (j = 0; j < MAX_RECORDS; j++) {
						if ((strcmp(record[i][j].comm,comm) == 0) && (record[i][j].uid == uid)) {
							recordPresent = true;
							record[i][j].count++;
							//printk("System call %s (%d) invoked from process %s at index %d, increasing %s to %d\n", sys_call, i, comm, j, record[i][j].comm, record[i][j].count);
							break;	
						}
					}
				}
				if (!recordPresent) { 
					addNewRecord(comm,uid,sys_call,i);
					
				}	
			}
		}
		if (!sysCallPresent) addNewSysCall(comm,uid,sys_call);
	}
}

void addNewSysCall(char *comm, int uid, char *sys_call)
{
	printk("Adding new sys_call %s\n", sys_call);
	addNewRecord(comm,uid,sys_call,sysCallPointer);
	sysCallPointer++;
	if (sysCallPointer >= MAX_SYS_CALLS) sysCallPointer = 0;
}

void addNewRecord(char *comm, int uid, char *sys_call, int sys_call_index)
{
	//printk("Adding new record %s %d\n", comm, uid);
	recordPointer[sys_call_index]++;
	strcpy(record[sys_call_index][recordPointer[sys_call_index]].comm,comm);
	strcpy(record[sys_call_index][recordPointer[sys_call_index]].sys_call,sys_call);	
	record[sys_call_index][recordPointer[sys_call_index]].uid = uid;
	if (recordPointer[sys_call_index] >= MAX_RECORDS) recordPointer[sys_call_index] = 0;
}

int cse812_fetch(char *buf, int i) {
	// Comment
	
	int j;
	int k;
	int l;
	char comm[16];
	char sys_call[16];	
	char uid[5];
	char count[11];

	double scaledCount;	
	int ticks;	
	int max;

	strcpy(buf,"");
	
	// for (i = 0; i < MAX_SYS_CALLS; i++) {	
		// Need condition for not printing
	if (strcmp(record[i][0].sys_call, "") != 0) {	
		max = maxCount(i);
		strcat(buf, "System call: ");
		sprintf(sys_call, "%15s\n", record[i][0].sys_call);
		strcat(buf, sys_call);
		// Header
		for (j = 0; j < MAX_RECORDS; j++) {
			if (record[i][j].count > 0) { 
				strcat(buf, "Image: ");
				sprintf(comm, "%15s ", record[i][j].comm);
				strcat(buf, comm);
				strcat(buf, "UID: ");
				sprintf(uid, "%04d ", record[i][j].uid);
				strcat(buf, uid); 	
				strcat(buf, "Count: ");
				scaledCount = (double)record[i][j].count/(double)max; 
				ticks = scaledCount*10;
				if (ticks >= 0 && ticks <= 10) {	
					for (k = 0; k < ticks; k++) strcat(buf, "+"); 
					for (l = ticks; l < 10; l++) strcat(buf, " ");	
				}	
				sprintf(count, "%d ", record[i][j].count);
				strcat(buf, count);	
				strcat(buf, "\n");
			}
		}
	}
	return sysCallPointer;
	
}

int maxCount(int sys_call_index)
{
	int i;
	int max;
	max = -1;	
	for (i = 0; i < MAX_RECORDS; i++) {
		if (record[sys_call_index][i].count > max) max = record[sys_call_index][i].count;	
	}  
	return max;
}

EXPORT_SYMBOL(cse812_clear);
EXPORT_SYMBOL(cse812_fetch);
EXPORT_SYMBOL(cse812_off);
