#include <string>
#include <cmath>
#include <catch/catch.hpp>
#define TDD_MODE
#include "QbGifBuilder/include/QbLzwEncoder.h"
#include "QbGifBuilder/include/QbLzwDecoder.h"

#include "tdd-tools.inc.cpp"

using namespace std;
using namespace Catch::Matchers;

// #define SHOW_LOGS 1
#define ARRAY_MAX_SIZE 1000000

string test_loop(__u_char decoded[], int first, int nb_loops, int initialCodeSize) {
  string result = "";
  __u_char encoded[ARRAY_MAX_SIZE*2];
  string error_list = "";
  for (int dec_size = first; dec_size < first + nb_loops; dec_size++) {
    #if SHOW_LOGS == 1
    if (dec_size % 100 == 0) printf("Test : %d / %d\n" , dec_size, nb_loops);
    #endif
    size_t len = dec_size;
    vector<__u_char> res = QbLzwEncoder::encode (initialCodeSize,
                                                 decoded,
                                                 len);
    for(int i = 0; i < res.size(); i++) {
      encoded[i] = res[i];
    }
    vector<__u_char> decoded2 = QbLzwDecoder::decode (initialCodeSize, encoded, res.size());
    bool has_error = false;
    if (decoded2.size() != dec_size) {
      has_error = true;
    }
    for (int i = 0; !has_error && i < decoded2.size(); i++) {
      if (decoded2[i] != decoded[i]) {
        has_error = true;
      }
    }
    if (has_error) {
      if (!result.empty()) {
        result += " - ";
      }
      result += std::to_string(dec_size);
#if SHOW_LOGS == 1
      printf("\n");
      printf("====== ERROR for dec_size = %d\n", dec_size);
      printf("\n");
      printf("Size decoded = %d - size decoded2 = %d \n", dec_size, (int)decoded2.size() );
      printf("\n");
      printf("\n");
      printf("...... decode .......\n" );
      for( int i = 0; i < dec_size; i++ ) {
        printf("%02x,", decoded[i]);
      }
      printf("\n");
      printf("\n");
      printf("\n");
      printf("\n");
      printf("...... decode2 .......\n" );
      for( int i = 0; i < decoded2.size(); i++ ) {
        printf("%02x,", decoded2[i]);
      }
      printf("\n");
      printf("\n");
#endif
    }
  }  
  return result;
}

void populate_decoded_array(__u_char *decoded, int max_val) {
  int seq_size = 1;
  int pos = 0;
  while (pos < ARRAY_MAX_SIZE) {
    int seq_min_pos = pos;
    int seq_max_pos = pos + (int)pow( max_val, seq_size);
    if( seq_max_pos > ARRAY_MAX_SIZE )
      seq_max_pos = ARRAY_MAX_SIZE;
    for (int i = seq_min_pos; i < seq_max_pos; i++ ) {
      int relative_pos = i - seq_min_pos;
      int digit = seq_size - relative_pos % seq_size;
      int number = relative_pos / seq_size;
      decoded[i] = number / (int)pow(max_val, digit - 1) % max_val;
    }
    pos = seq_max_pos;
    seq_size++;
  }
}

TEST_CASE( "Compare encoded-decoded to initial with 0,1,2,3...", "[QbLzwEncoder-QbLzwDecoder]" ) {
  __u_char decoded[ARRAY_MAX_SIZE];
  for( int i = 0 ; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = i & 0xFF;
  }

  int initialCodeSize = 8;
  int first_size;
  int nb_loops;
  string error_list;

  first_size = 0;
  nb_loops = 1000;
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK( error_list == "" );

  // Last size with no dictionary reset
  first_size = 30450;
  nb_loops = 1;
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK(error_list == "");

  // test dictionary reset
  first_size = 30451;
  nb_loops = 1;
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK(error_list == "");
}

TEST_CASE( "Test various initialCodeSizes", "[QbLzwEncoder-QbLzwDecoder]" ) {
  __u_char decoded[ARRAY_MAX_SIZE];
  int first_size = 14;
  int nb_loops = 1;
  int initialCodeSize ;
  string error_list;

  // initialCodeSize = 6 and array = 0,0,0,0,...
  initialCodeSize = 6;
  first_size = 0;
  nb_loops = 300;
  for( int i = 0 ; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = 0; // i & ( (1 << initialCodeSize) - 1);
  }
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK( error_list == "" );


  // initialCodeSize = 6 and array = 1,2,3,4,...
  initialCodeSize = 6;
  first_size = 0;
  nb_loops = 300;
  for( int i = 0 ; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = i & ( (1 << initialCodeSize) - 1);
  }
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK( error_list == "" );

  // initialCodeSize = 7 and array = 0,0,0,0,...
  initialCodeSize = 7;
  first_size = 0;
  nb_loops = 300;
  for( int i = 0 ; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = 0; // i & ( (1 << initialCodeSize) - 1);
  }
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK( error_list == "" );


  // initialCodeSize = 7 and array = 1,2,3,4,...
  initialCodeSize = 7;
  first_size = 0;
  nb_loops = 300;
  for( int i = 0 ; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = i & ( (1 << initialCodeSize) - 1);
  }
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK( error_list == "" );

  // initialCodeSize = 9 and array = 0,0,0,0,...
  initialCodeSize = 9;
  first_size = 0;
  nb_loops = 300;
  for( int i = 0 ; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = 0; // i & ( (1 << initialCodeSize) - 1);
  }
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK( error_list == "" );

  // initialCodeSize = 9 and array = 1,2,3,4,...
  initialCodeSize = 9;
  first_size = 0;
  nb_loops = 300;
  for( int i = 0 ; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = i & ( (1 << initialCodeSize) - 1);
  }
  error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK( error_list == "" );
}

TEST_CASE( "Compare encoded-decoded to initial with 0,1,..256,01,..012", "[QbLzwEncoder-QbLzwDecoder]" ) {
  __u_char decoded[ARRAY_MAX_SIZE];
  populate_decoded_array( decoded, 256);
  int first_size = 1;
  int nb_loops = 1000;
  int initialCodeSize = 8;
  string error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK(error_list == "");
}

TEST_CASE( "Compare encoded-decoded to initial with 0,1,2,00,01,..012", "[QbLzwEncoder-QbLzwDecoder]" ) {
  __u_char decoded[ARRAY_MAX_SIZE];
  populate_decoded_array( decoded, 3);
  int first_size = 1;
  int nb_loops = 1000;
  int initialCodeSize = 8;
  string error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK(error_list == "");
}

TEST_CASE( "Compare encoded-decoded to initial with random array", "[QbLzwEncoder-QbLzwDecoder]" ) {
  __u_char decoded[ARRAY_MAX_SIZE];
  for (int i = 0; i < ARRAY_MAX_SIZE; i++) {
    decoded[i] = rand() % 0xFF;
  }
  int initialCodeSize = 8;
  int first_size = ARRAY_MAX_SIZE;
  int nb_loops = 1;
  string error_list = test_loop(decoded, first_size, nb_loops, initialCodeSize);
  CHECK(error_list == "");
}
