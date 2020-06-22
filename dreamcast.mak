
	
all-after : 
	$(KOS_OBJCOPY) -O binary -R .stack $(TARGET) DCRACE.bin
	$(KOS_BASE)/utils/scramble/scramble DCRACE.bin 1st_read.bin
	#$(KOS_BASE)/utils/dctool/dc-tool-ip  -x DCRACE.bin -i dc.iso
	mkiso.bat
