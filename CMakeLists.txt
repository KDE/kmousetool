cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Release service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "22")
set (RELEASE_SERVICE_VERSION_MINOR "12")
set (RELEASE_SERVICE_VERSION_MICRO "2")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kmousetool VERSION "${RELEASE_SERVICE_VERSION}")

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.90.0")

find_package (ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(CheckIncludeFiles)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets)

find_package (KF5 ${KF5_MIN_VERSION} REQUIRED
    DBusAddons
    I18n
    IconThemes
    Notifications
    XmlGui
    WindowSystem
)
find_package(KF5DocTools ${KF5_MIN_VERSION})
set_package_properties(KF5DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)
find_package(Phonon4Qt${QT_MAJOR_VERSION} REQUIRED NO_MODULE)

find_package(X11 REQUIRED)
if(NOT X11_Xt_FOUND)
    message(FATAL_ERROR "Cannot find X11 Xt component (libxt header file X11/Intrinsic.h)")
endif(NOT X11_Xt_FOUND)
if(NOT X11_XTest_FOUND)
    message(FATAL_ERROR "Cannot find X11 XTest component (libxcb-xtest header file X11/extensions/XTest.h)")
endif(NOT X11_XTest_FOUND)

add_definitions(
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050F00
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x055500
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000
)

add_subdirectory( kmousetool )
ki18n_install(po)
if(KF5DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
