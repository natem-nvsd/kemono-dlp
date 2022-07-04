# config.mk

PREFIX = /usr/local
TARGET = "kemono-dlp"

CURLINC = `pkg-config --cflags libcurl`
CURLLIB = `pkg-config --libs libcurl`

JSONINC = `pkg-config --cflags json-c`
JSONLIB = `pkg-config --libs json-c`

PCRE2INC = `pcre2-config --cflags-posix`
PCRE2LIB = `pcre2-config --libs-posix`

INCS = ${CURLINC} ${JSONINC} ${PCRE2INC}
LIBS = ${CURLLIB} ${JSONLIB} ${PCRE2LIB}

CFLAGS = -g -std=c99 -pedantic -Wall -O -D_FORTIFY_SOURCE=2 ${INCS}
LDFLAGS = ${LIBS}

CC = cc
