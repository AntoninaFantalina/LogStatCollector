# Download RapidJSON
include(ExternalProject)
ExternalProject_Add(
    rapidjson
    PREFIX "lib/rapidjson"
    GIT_REPOSITORY "https://github.com/Tencent/rapidjson.git"
    GIT_TAG 012be8528783cdbf4b7a9e64f78bd8f056b97e24
    TIMEOUT 10
    CMAKE_ARGS
        -DRAPIDJSON_BUILD_TESTS=OFF
        -DRAPIDJSON_BUILD_DOC=OFF
        -DRAPIDJSON_BUILD_EXAMPLES=OFF
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    UPDATE_COMMAND ""
)

# Prepare RapidJSON (RapidJSON is a header-only library)
ExternalProject_Get_Property(rapidjson source_dir)
set(RAPIDJSON_INCLUDE_DIR ${source_dir}/include)
