/* Address to comunitcate with the arm11 thread */
#define ARM11COMMAND_ADDRESS 0x1FFF3000

#define ARM11_COMMANDS typedef struct { \
						    vu32 a11ControllValue; \
						    vu32 a11threadRunning; \
						    vu32 version; \
						    vu32 brightness; \
						    vu32 fbTopLeft; \
						    vu32 fbTopRigth; \
						    vu32 fbBottom; \
						    vu32 setBrightness; \
						    vu32 enableLCD; \
						    vu32 mode; \
						    vu32 changeMode; \
						} a11Commands;

/* Framebuffer Addresses */
#define FB_TOP_LEFT 0x18300000
#define FB_TOP_RIGHT 0x18300000
#define FB_BOTTOM 0x18346500
//#define FB_TOP_RIGHT 0x1838CA00 //for 3d?

#define DEFAULT_BRIGHTNESS		0xFF

/* Screen contants */
#define BYTES_PER_PIXEL 3
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 400

#define SCREEN_SIZE (BYTES_PER_PIXEL * SCREEN_WIDTH * SCREEN_HEIGHT)

#define MODE_MAIN 1
#define MODE_DRAW 2

#define ARM11_DONE 0
#define DISABLE_SCREEN 1
#define ENABLE_SCREEN 2
#define ENABLE_SCREEN_3D 3 //notImplemented
