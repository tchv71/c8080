all: nc.com

nc.com: *.c *.h
	../../c8080 -OCPM -DARCH_CPM_CCP -DARCH_PALMIRA2 -I../../include/micro80 -onc.com -A..\..\include\sjasmplus.exe dir.c nc.c panel.c windows.c

.PHONY: clean
clean:
	del nc.com nc.lst nc.asm