mtserver:
	gcc -Wall -o mtserver mtserver.c

mtclient:
	gcc -Wall -o mtclient mtclient.c

clean:
	rm -f mtserver mtclient
