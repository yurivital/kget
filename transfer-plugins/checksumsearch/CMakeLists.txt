include_directories(
   ../../
)

set(kget_checksumsearchfactory_PART_SRCS
  checksumsearch.cpp
  checksumsearchtransferdatasource.cpp
  checksumsearchfactory.cpp
  ../../kget_debug.cpp
)

kconfig_add_kcfg_files(kget_checksumsearchfactory_PART_SRCS checksumsearchsettings.kcfgc)
add_library(kget_checksumsearchfactory MODULE ${kget_checksumsearchfactory_PART_SRCS})
kcoreaddons_desktop_to_json(kget_checksumsearchfactory kget_checksumsearchfactory.desktop)
target_link_libraries(kget_checksumsearchfactory kgetcore)

install(TARGETS kget_checksumsearchfactory DESTINATION  ${KGET_PLUGIN_INSTALL_DIR})
install(FILES kget_checksumsearchfactory.kcfg DESTINATION ${KCFG_INSTALL_DIR})

###Build KCM-Module
set(kcm_kget_checksumsearchfactory_PART_SRCS
../../kget_debug.cpp
checksumsearch.cpp
dlgchecksumsearch.cpp
)

ki18n_wrap_ui(kcm_kget_checksumsearchfactory_PART_SRCS
checksumsearch.ui
checksumsearchadddlg.ui
)

kconfig_add_kcfg_files(kcm_kget_checksumsearchfactory_PART_SRCS checksumsearchsettings.kcfgc)
add_library(kcm_kget_checksumsearchfactory MODULE ${kcm_kget_checksumsearchfactory_PART_SRCS})
target_link_libraries(kcm_kget_checksumsearchfactory kgetcore)
install(TARGETS kcm_kget_checksumsearchfactory DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES kget_checksumsearchfactory_config.desktop DESTINATION ${SERVICES_INSTALL_DIR})
