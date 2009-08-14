###########################################################
#                                                         #
# Look for dependencies required by several libraries     #
#                                                         #
###########################################################

message( STATUS "== GLOBAL DEPENDENCY CHECK ==" )

# 
# Qt
# 

find_package( Qt4 )
# we don't have a specific version requirement.
# individual libraries may check the version themselves.
