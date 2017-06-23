all:
	gcc -o webserv webserv.c serverUtil.c caseUtil.c
	gcc histogram.c -o histogram
clean:
	rm webserv || true
	rm histogram || true
