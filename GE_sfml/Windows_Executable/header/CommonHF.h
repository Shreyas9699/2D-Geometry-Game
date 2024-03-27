#pragma once

// This is a common header file that includes all commonly used header files in this project

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream> 
#include <fstream> 
#include <sstream>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include <algorithm>
#include <random>
#include <thread>

#include "Vec2.h"

#define M_PI       3.14159265358979323846   // pi

// Creates a new alias (ptr) that is equivalent to std::shared_ptr<T>
template <class T> using ptr = std::shared_ptr<T>;