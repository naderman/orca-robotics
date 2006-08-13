SET (DROP_METHOD "xmlrpc")
SET (DROP_SITE "http://opium.acfr.usyd.edu.au:8081")
SET (DROP_LOCATION "orca2")
SET (COMPRESS_SUBMISSION ON)

# Dashboard is opened for submissions for a 24 hour period starting at
# the specified NIGHLY_START_TIME. Time is specified in 24 hour format.
SET (NIGHTLY_START_TIME "04:00:00 EAST")
FIND_PROGRAM(MEMORYCHECK_COMMAND
    NAMES valgrind
    PATHS
    DOC "Path to valgrind, used for memory error detection."
    )
  SET(MEMORYCHECK_SUPPRESSIONS_FILE "" CACHE FILEPATH "File that contains suppressions for the memmory checker")
