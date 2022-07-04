include config.mk

SRC	= commands.c download.c getters.c linked_list.c main.c parser.c
#SRC	= *.c # This works
OBJ	= ${SRC:.c=.o}

all: options kemono-dlp

options:
	@echo "CFLAGS = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC = ${CC}"

kemono-dlp: clean
	${CC} -c ${SRC} ${CFLAGS} $<
	${CC} ${OBJ} -o ${TARGET} ${LDFLAGS}
	rm -f ${TARGET}.core
	rm -f ${OBJ}

clean:
	rm -f ${TARGET} ${OBJ}

install: kemono-dlp
	cp -f ${TARGET} ${REFIX}/bin
	cp -f ${TARGET}.1 ${REFIX}/share/man/man1
	strip ${REFIX}/bin/${TARGET}
	chmod 755 ${REFIX}/bin/${TARGET}

.PHONY: all kemono-dlp clean install
