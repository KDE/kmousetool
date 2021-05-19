project(kmousetool)

cmake_minimum_required (VERSION 3.16 FATAL_ERROR)
set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.83.0")


find_package (ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(CheckIncludeFiles)
include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets)

find_package (KF5 ${KF5_MIN_VERSION} REQUIRED
    DBusAddons
    DocTools
    I18n
    IconThemes
    Notifications
    XmlGui
    WindowSystem
)

find_package(Phonon4Qt5 REQUIRED NO_MODULE)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})


find_package(X11 REQUIRED)
if(NOT X11_Xt_FOUND)
    message(FATAL_ERROR "Cannot find X11 Xt component (libxt header file X11/Intrinsic.h)")
endif(NOT X11_Xt_FOUND)
if(NOT X11_XTest_FOUND)
    message(FATAL_ERROR "Cannot find X11 XTest component (libxcb-xtest header file X11/extensions/XTest.h)")
endif(NOT X11_XTest_FOUND)

add_subdirectory( doc )
add_subdirectory( kmousetool )
ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
