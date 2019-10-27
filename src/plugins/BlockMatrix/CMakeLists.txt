include_directories(settings)

set( BlockMatrix_SRCS
	blockmatrix_plugin.cpp
    bm_interceptor.cpp
	)

ecm_create_qm_loader( BlockMatrix_SRCS falkon_blockmatrix_qt )

add_library(BlockMatrix MODULE ${BlockMatrix_SRCS} ${UIS} ${RSCS})
install(TARGETS BlockMatrix DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(BlockMatrix FalkonPrivate)

