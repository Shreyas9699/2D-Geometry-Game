#pragma once

// This is a common header file that includes all commonly used header files in this project

#include <SFML/Graphics.hpp>
#include <SFML/audio.hpp>

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <vector>
#include <memory>
#include <map>
#include <algorithm>

#include "Vec2.h"

// Creates a new alias (ptr) that is equivalent to std::shared_ptr<T>
template <class T> using ptr = std::shared_ptr<T>;