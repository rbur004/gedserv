gedserv: date.c token.c chart.c bit.c socket.c croll.c hash.c stringfunc.c btree.c ged_dump.c rel_index.c relationship.c
	cc -DLINUX -g -o gedserv date.c token.c chart.c bit.c socket.c croll.c hash.c stringfunc.c btree.c ged_dump.c rel_index.c relationship.c

install: gedserv
	-mv /services/www/bin/gedserv.active /services/www/bin/gedserv.active.old
	cp gedserv /services/www/bin/gedserv.active
	/services/www/bin/gedserv.start
