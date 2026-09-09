openrepo: main.o stringview.o
	cc main.o stringview.o -ggdb -o openrepo

main.o: main.c
	cc main.c -ggdb -Wall -Wextra -c -o main.o

stringview.o: stringView.h stringView.c
	cc stringView.c -ggdb -Wall -Wextra -c -o stringview.o

.PHONY: clean
clean:
	rm -f ./main.o ./stringview.o ./openrepo