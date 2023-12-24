all:
	gcc ../main.c ../glad_gl.c -I ../inc -Ofast -lglfw -lm -o tuxvsdragon

install:
	cp tuxvsdragon $(DESTDIR)

uninstall:
	rm $(DESTDIR)/tuxvsdragon

clean:
	rm tuxvsdragon