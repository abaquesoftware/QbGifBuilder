#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbStreamTools.h"

using namespace std;
using namespace Catch::Matchers;

#include "tdd-QbStreamTools.cpp"
#include "tdd-QbLzwDictionaryEntry.cpp"
#include "tdd-QbLzwDictionary.cpp"
#include "tdd-QbLzwEncoder.cpp"
#include "tdd-QbLzwDecoder.cpp"
#include "tdd-QbLzwEncoder-QbLzwDecoder.cpp"
#include "tdd-QbGifFrame.cpp"
#include "tdd-QbGifBitmap.cpp"
#include "tdd-QbGifBuilder.cpp"
