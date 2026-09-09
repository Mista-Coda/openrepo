INSTALLPATH = /bin/openrepo

openrepo: main.o stringview.o
	cc main.o stringview.o -ggdb -o openrepo

main.o: src/main.c
	cc src/main.c -ggdb -Wall -Wextra -c -o main.o

stringview.o: src/stringView.h src/stringView.c
	cc src/stringView.c -ggdb -Wall -Wextra -c -o stringview.o

.PHONY: clean install uninstall
install: openrepo
	install -Dm755 openrepo $(INSTALLPATH)

uninstall:
	rm -f $(INSTALLPATH)

clean:
	rm -f ./main.o ./stringview.o ./openrepo