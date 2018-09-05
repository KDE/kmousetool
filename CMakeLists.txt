project(kmousetool)

cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package (ECM 1.3.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(CheckIncludeFiles)
include(KDEInstallDirs)
include(ECMInstallIcons)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets)

find_package (KF5 REQUIRED
    DBusAddons
    DocTools
    I18n
    IconThemes
    Notifications
    XmlGui
)

find_package(Phonon4Qt5 REQUIRED NO_MODULE)

add_definitions( -DQT_NO_CAST_FROM_ASCII )
add_definitions( -DQT_NO_CAST_TO_ASCII )
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)

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

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
