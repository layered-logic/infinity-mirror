# core/webapp_assets — embed + serve the Vite-built webapp bundle.
#
# EMBED_FILES references files under Firmware/v1/webapp/dist/. The npm
# build script (Firmware/v1/webapp/package.json's "build") produces
# .gz siblings via scripts/gzip-dist.mjs after vite build. If any .gz
# is missing here, IDF's component manager fails at configure time
# with a "file not found" error — surfaced clearly enough that "go run
# npm run build" is the obvious fix.

set(_webapp_dist "${CMAKE_CURRENT_LIST_DIR}/../../webapp/dist")
set(_webapp_files
    "${_webapp_dist}/index.html.gz"
    "${_webapp_dist}/app.js.gz"
    "${_webapp_dist}/app.css.gz"
)

# Friendlier error than the raw "EMBED_FILES file does not exist" one.
foreach(_f ${_webapp_files})
    if(NOT EXISTS "${_f}")
        message(FATAL_ERROR
            "Webapp asset missing: ${_f}\n"
            "Run `npm run build` in Firmware/v1/webapp/ first "
            "(produces dist/*.gz via the postbuild script).")
    endif()
endforeach()

idf_component_register(
    SRCS "webapp_assets.c"
    INCLUDE_DIRS "."
    REQUIRES esp_http_server
    EMBED_FILES ${_webapp_files}
)
