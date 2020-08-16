CC = em++
all: *.cpp *.h
	$(CC) *.cpp -O3 -s TOTAL_MEMORY=134217728 -lwebsocket.js \
	-s DISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s DISABLE_EXCEPTION_CATCHING=0 \
	-s EXPORTED_FUNCTIONS='["_main"]' -s 'EXTRA_EXPORTED_RUNTIME_METHODS=["ccall", "cwrap"]' \
	-s ASSERTIONS=1 -std=c++11 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s FORCE_FILESYSTEM=1 -s SDL2_IMAGE_FORMATS='["png"]' \
	-s SAFE_HEAP=0 -s WARN_UNALIGNED=1 -fsanitize=address -Wcast-align -Wover-aligned \
	 -s STB_IMAGE=1 --preload-file assets --use-preload-plugins --embed-file walkSheet.png -o index.html


