BIN=/usr/local/bin

gedserv: date.c token.c chart.c bit.c socket.c croll.c hash.c stringfunc.c btree.c ged_dump.c rel_index.c relationship.c main.c
	cc -DLINUX -g -o gedserv date.c token.c chart.c bit.c socket.c croll.c hash.c stringfunc.c btree.c ged_dump.c rel_index.c relationship.c main.c

install: gedserv
	-mv ${BIN}/gedserv.active ${BIN}/gedserv.active.old
	cp gedserv ${BIN}/gedserv.active
