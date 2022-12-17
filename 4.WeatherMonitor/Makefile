CC=gcc

CFLAGS=-c -fPIC -Wall -Wextra -Wpedantic -std=c11

all:
	##git
	sudo apt -y install build-essential libssl-dev libcurl4-gnutls-dev libexpat1-dev gettext unzip
	wget https://github.com/git/git/archive/v2.28.0.zip
	unzip v2.28.0.zip
	cd git-*
	$(make prefix=/usr/local all)
	$(sudo make prefix=/usr/local install)
	cd ..
	rm v*.zip
	rm -Rf git-*
	git clone https://github.com/git/git
	cd git
	$(make prefix=/usr/local all)
	$(sudo make prefix=/usr/local install)
	cd ..
	rm -Rf git*
	
	##curl
	sudo apt install curl
	
	##jansson
	if [ -d "./jansson" ]; then \
		rm -Rf ./jansson/; \
	else \
		echo "./jansson not found."; \
	fi
	git clone https://github.com/akheron/jansson.git
	cp -r ./jansson/src ./jansson_/
	rm -Rf ./jansson/
	mv ./jansson_/ ./jansson/
	cp ./jansson/jansson_config.h.in ./jansson/jansson_private_config.h
	mv ./jansson/jansson_config.h.in ./jansson/jansson_config.h
	sed -i 's/@json_inline@/inline/g' ./jansson/jansson_config.h	
	sed -i 's/#define JSON_INTEGER_IS_LONG_LONG @json_have_long_long@/#define JSON_INTEGER_IS_LONG_LONG 1/g' ./jansson/jansson_config.h
	sed -i 's/#define JSON_HAVE_LOCALECONV @json_have_localeconv@/#define JSON_HAVE_LOCALECONV 0/g' ./jansson/jansson_config.h
	sed -i 's/#define JSON_HAVE_ATOMIC_BUILTINS @json_have_atomic_builtins@/\/\/#define JSON_HAVE_ATOMIC_BUILTINS @json_have_atomic_builtins@/g' ./jansson/jansson_config.h
	sed -i 's/#define JSON_HAVE_SYNC_BUILTINS @json_have_sync_builtins@/\/\/#define JSON_HAVE_SYNC_BUILTINS @json_have_sync_builtins@/g' ./jansson/jansson_config.h
	sed -i 's/#include <jansson_config.h>/#include "jansson_config.h"/g' ./jansson/hashtable.c
	sed -i 's/#ifdef HAVE_CONFIG_H/#include <stdio.h>\n#include <stdlib.h>\n#include <stdint.h>\n#ifdef HAVE_CONFIG_H/g' ./jansson/hashtable_seed.c
	sed -i 's/#ifndef UTF_H/#include <stdio.h>\n#include <stdlib.h>\n#include <stdint.h>\n#ifndef UTF_H/g' ./jansson/utf.h
	sed -i 's/#ifdef HAVE_CONFIG_H/#include <stdio.h>\n#include <stdlib.h>\n#include <stdint.h>\n#ifdef HAVE_CONFIG_H/g' ./jansson/hashtable.c
	
	$(CC) $(CFLAGS) ./jansson/hashtable_seed.c
	$(CC) $(CFLAGS) ./jansson/hashtable.c
	$(CC) $(CFLAGS) ./jansson/dump.c
	$(CC) $(CFLAGS) ./jansson/error.c
	$(CC) $(CFLAGS) ./jansson/load.c
	$(CC) $(CFLAGS) ./jansson/memory.c
	$(CC) $(CFLAGS) ./jansson/pack_unpack.c
	$(CC) $(CFLAGS) ./jansson/strbuffer.c
	$(CC) $(CFLAGS) ./jansson/strconv.c
	$(CC) $(CFLAGS) ./jansson/utf.c
	$(CC) $(CFLAGS) ./jansson/value.c
	$(CC) $(CFLAGS) ./jansson/version.c
	$(CC) $(CFLAGS) ./main.c
	
	$(CC) -shared -o weather_monitor.so *.o
	find -name "*.o" -type f -delete
