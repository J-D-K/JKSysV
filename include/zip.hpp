#pragma once
#include "MiniZip.hpp"
#include "fslib.hpp"

void copy_directory_to_zip(const fslib::Path &source, MiniZip &zip);