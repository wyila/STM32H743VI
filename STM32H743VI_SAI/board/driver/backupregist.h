#ifndef __BACKUP_REGISTER_H
#define __BACKUP_REGISTER_H

#define BAKRAM_BASE     0x38800000
#define BAKRAM_SIZE     4096

#define Update_Iden     (*(__IO uint32_t *)(BAKRAM_BASE + 4))

void Backup_Register_Init(void);

#endif

