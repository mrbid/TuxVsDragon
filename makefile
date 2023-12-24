all:
	cc main.c glad_gl.c -I inc -Ofast -lglfw -lm -o release/TuxVsDragon_linux
	strip --strip-unneeded release/TuxVsDragon_linux
	upx --lzma --best release/TuxVsDragon_linux

test:
	cc main.c glad_gl.c -I inc -Ofast -lglfw -lm -o /tmp/tuxvsdragon_test
	/tmp/tuxvsdragon_test
	rm /tmp/tuxvsdragon_test

web:
	emcc main.c glad_gl.c -DWEB -O3 --closure 1 -s FILESYSTEM=0 -s USE_GLFW=3 -s ENVIRONMENT=web -s TOTAL_MEMORY=256MB -I inc -o release/web/index.html --shell-file t.html

run:
	emrun release/web/index.html

win:
	i686-w64-mingw32-gcc main.c glad_gl.c -Ofast -I inc -L. -lglfw3dll -lm -o release/TuxVsDragon_windows.exe
	strip --strip-unneeded release/TuxVsDragon_windows.exe
	upx --lzma --best release/TuxVsDragon_windows.exe

clean:
	rm -f release/TuxVsDragon_linux
	rm -f release/TuxVsDragon_windows.exe
	rm -f release/web/index.html
	rm -f release/web/index.js
	rm -f release/web/index.wasm
