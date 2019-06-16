set(libexif_SRCS
  exif-byte-order.c
  exif-content.c
  exif-data.c
  exif-entry.c
  exif-format.c
  exif-ifd.c
  exif-loader.c
  exif-log.c
  exif-mem.c
  exif-mnote-data.c
  exif-tag.c
  exif-utils.c

  canon/exif-mnote-data-canon.c
  canon/mnote-canon-entry.c
  canon/mnote-canon-tag.c

  fuji/exif-mnote-data-fuji.c
  fuji/mnote-fuji-entry.c
  fuji/mnote-fuji-tag.c

  olympus/exif-mnote-data-olympus.c
  olympus/mnote-olympus-entry.c
  olympus/mnote-olympus-tag.c

  pentax/exif-mnote-data-pentax.c
  pentax/mnote-pentax-entry.c
  pentax/mnote-pentax-tag.c
)

add_library(exif_objlib OBJECT ${libexif_SRCS})

set_property(TARGET exif_objlib PROPERTY C_STANDARD 11)

if(MSVC)
  target_compile_definitions(exif_objlib PRIVATE _CRT_SECURE_NO_WARNINGS)
  target_compile_options(exif_objlib PRIVATE "/w")
else()
  target_compile_options(exif_objlib PRIVATE "-std=c11")
endif()

target_include_directories(exif_objlib PRIVATE ${CMAKE_SOURCE_DIR})

