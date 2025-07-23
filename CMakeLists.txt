cmake_minimum_required(VERSION 3.16 FATAL_ERROR)

# Release service version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "25")
set (RELEASE_SERVICE_VERSION_MINOR "07")
set (RELEASE_SERVICE_VERSION_MICRO "90")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(kmousetool VERSION "${RELEASE_SERVICE_VERSION}")

set(QT_MIN_VERSION "6.7.0")
set(KF_MIN_VERSION "6.3.0")

find_package (ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

include(ECMInstallIcons)
include(CheckIncludeFiles)
include(FeatureSummary)
include(KDEGitCommitHooks)
include(KDEClangFormat)
include(ECMDeprecationSettings)
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h *.c)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
    Gui
    Multimedia
    Widgets
)

find_package(KF6 ${KF_MIN_VERSION} REQUIRED
    DBusAddons
    I18n
    IconThemes
    XmlGui
    WindowSystem
    CoreAddons
    StatusNotifierItem
    Config
    WidgetsAddons
)

find_package(KF6DocTools ${KF_MIN_VERSION})
set_package_properties(KF6DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)

find_package(X11 REQUIRED)
if(NOT X11_Xt_FOUND)
    message(FATAL_ERROR "Cannot find X11 Xt component (libxt header file X11/Intrinsic.h)")
endif(NOT X11_Xt_FOUND)
if(NOT X11_XTest_FOUND)
    message(FATAL_ERROR "Cannot find X11 XTest component (libxcb-xtest header file X11/extensions/XTest.h)")
endif(NOT X11_XTest_FOUND)

ecm_set_disabled_deprecation_versions(QT 6.8.0  KF 6.11.0)

add_subdirectory( kmousetool )
ki18n_install(po)
if(KF6DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
