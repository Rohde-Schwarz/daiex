from conans import ConanFile
from conans import CMake
from conans import tools
import os
import subprocess 
import shutil
import platform

required_conan_version = ">=1.43.0"

class RsDaiexConan(ConanFile):
    name = "rs_daiex"
    license = "Apache-2.0"
    description = "I/Q Data Import Export library (daiex)"
    url = "https://code.rsint.net/SWP/daiex"
    author = "R&S developers"
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto"
    }
    options = {
        "with_doc": [True, False],
        "with_test": [True, False],
        "with_dotNet": [True, False],
        "inject_library_name_in_include": [True, False],
        "developer_mode": [True, False]
    }
    default_options = {
        "with_doc": False,
        "with_test": False,
        "with_dotNet": False,
        "inject_library_name_in_include": False,
        "developer_mode": False
    }
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake_find_package"
    no_copy_source = True
    _cmake = None

    def set_version(self):
        with open(os.path.join(os.path.dirname(os.path.abspath(__file__)), "Version.txt")) as f:
            self.version = f.read()


    def build_requirements(self):
        self.build_requires("cmake/3.22.3")
        
    def requirements(self):
        self.requires("zlib/1.2.12")
        self.requires("libarchive/3.5.1")
        self.requires("hdf5/1.12.1")
        self.requires("matio/1.5.23")
        self.requires("pugixml/1.11")
        if self.options.with_test:
            self.requires("gtest/1.10.0")


    def _configure_cmake(self):
        if self._cmake is None:
            self._cmake = CMake(self)
            self._cmake.definitions["CMAKE_BUILD_TYPE"] = str(self.settings.build_type).capitalize()
            self._cmake.definitions["BUILD_DOC"] = self.options.with_doc
            self._cmake.definitions["BUILD_TEST"] = self.options.with_test
            self._cmake.definitions["BUILD_DOT_NET_WRAPPER"] = self.options.with_dotNet
            if self.options.inject_library_name_in_include:
                self._cmake.definitions["CMAKE_INSTALL_INCLUDEDIR"] = os.path.join("include", "daiex")
            self._cmake.configure()
        return self._cmake

    def build(self):
        build_target = None
        if self.options.developer_mode:
            build_target="package"
        cmake = self._configure_cmake()
        cmake.build(target=build_target)
        pass

    
    def package_id(self):
        del self.info.options.with_tests

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        self.cpp_info.components["daiex"].names["cmake_find_package"] = "daiex"
        self.cpp_info.components["daiex"].libs = ["daiex"]
        self.cpp_info.components["daiex"].requires = [] 
        # Necessarry as otherwise the requirements aren't consumed
        self.cpp_info.components["dummy"].requires = ["zlib::zlib", "libarchive::libarchive", "hdf5::hdf5", "matio::matio", "pugixml::pugixml"]
