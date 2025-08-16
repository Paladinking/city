from comp_backend import *
import pathlib

CLFLAGS="/favor:AMD64 /FS /Zi /DEBUG /arch:AVX2 /MD"
LINKFLAGS="/DEBUG"
DLLFLAGS=""

BUILD_DIR = "build"
BIN_DIR = "bin"

CLFLAGS_DBG = "-g -Og -O0 -march=native"
LINKFLAGS_DBG = "-g -Og -O0 -march=native"

BUILD_DIR_DBG = "build-gcc"
BIN_DIR_DBG = "bin-gcc"

BUILD_DIR_ZIG = "build-zig"
BIN_DIR_ZIG = "bin-zig"

WEB_BUILD_DIR = "build-web"
WEB_BIN_DIR = "bin-web"


WORKDIR = pathlib.Path(__file__).parent.resolve()

add_backend("Msvc", "Msvc", BUILD_DIR, BIN_DIR, WORKDIR, CLFLAGS, LINKFLAGS)
add_backend("Mingw", "Mingw", BUILD_DIR_DBG, BIN_DIR_DBG, WORKDIR, CLFLAGS_DBG, LINKFLAGS_DBG)
add_backend("Zigcc", "Zigcc", BUILD_DIR_ZIG, BIN_DIR_ZIG, WORKDIR, CLFLAGS_DBG, LINKFLAGS_DBG)
add_backend("Emcc", "Emcc", WEB_BUILD_DIR, WEB_BIN_DIR, WORKDIR, "", "")

set_backend("Msvc")

def main():
    sdl3 = find_package("SDL3")
    sdl3_image = find_package("SDL3_image")
    box2d = find_package("box2d")
    packages = [sdl3, sdl3_image, box2d]


    embed = Executable("embed.exe", "embed.c")

    imgs = [f"map/png/tile_{x}_{y}.png" for x in range(16) for y in range(11)]
    line = ' '.join(imgs)

    header, obj = Command(["images.h", "images.obj"],
                          f"{embed.product} -o tools/images.obj -H tools/images.h -s %f {line}",
                          "embed.c", *imgs, directory="tools")

    img_obj = Object("images.obj", "src/images.c", includes=["tools"],
                     packages=packages, depends=[header.product])

    src: List[Any] = ["src/main.c", "src/polygon.c"]

    if backend().ecmascript:
        name = "city.js"
        #module = '--js-library web/tetrislib.js -sEXPORT_NAME="\'CityModule\'" -sMODULARIZE=1'
        #tetris_flags += ' ' + module
        flags = ''
    else:
        name = "city.exe"
        flags = ''

    CopyToBin(*sdl3.dlls, *sdl3_image.dlls, *box2d.dlls)
    exe = Executable(name, *src, obj, img_obj, 
                     packages=packages, extra_link_flags=flags)
    
    build(__file__)


if __name__ == "__main__":
    main()
