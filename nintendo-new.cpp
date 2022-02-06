
#include <iostream>
#include <iomanip>
#include <vector>

/*
input
32
46508fb7 6677e201
output
b0c152f9 ebf2831f
ebf2831f b0c152f9

input
32
000073af 00000000
output
00000001 000073af
00000083 000000e5
000000e5 00000083
000073af 00000001

input
32
738377c1 00000000
output
00000001 738377c1
0000b0c5 0000cd55
0000cd55 0000b0c5
738377c1 00000001

input
32
46508fb7 6677e201
output
b0c152f9 ebf2831f
ebf2831f b0c152f9

input
64
f3268b49 661859eb 0b324559 65ee6bda
output
0cf5c2bf 9aba68ef c18fb79b de70eef7
c18fb79b de70eef7 0cf5c2bf 9aba68ef

input
128
a91db473 fcea8db4 f3bb434a 8dba2f16 51abc87e 92c44759 5c1a16d3 6111c6f4
output
a30d28bd bda19675 3f95d074 b6f69434 c58f4047 d73fe36a 24be2846 e2ebe432
c58f4047 d73fe36a 24be2846 e2ebe432 a30d28bd bda19675 3f95d074 b6f69434

input
256
4af6fc33 39029380 465c5267 c72f6a8b 0906e6d0 ca60550f 14a5e47c 42ad10fb 4a3bb446 bb74360a 5ea02b9c 23c68553 3fade253 e270ba24 39e141ad 6c38c43d
output
320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299 33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4
33749a7f 6cc9659d dc503569 ef4d0ef5 73b746c5 b8fb36d3 7616e9d6 b21251c4 320a18d5 b61b13f6 1aaaa61c 0afe2a41 1a4ff107 84cc2efc 956ff31d fa595299
*/

using namespace std;



unsigned int *encode(const unsigned int *a, int size)
{
  unsigned int *b = new unsigned int[size / 16]; // <- output tab

  for (int i = 0; i < size / 16; i++)
  { // Write size / 16 zeros to b
    b[i] = 0;
  }

  for (int i = 0; i < size; i++)
    for (int j = 0; j < size; j++)
    {
      unsigned int jr_shift = int(j % 32);
      unsigned int ijl_shift = int((i + j) % 32);
      unsigned int out_index = int((i + j) / 32);
      unsigned int j32_index = int(j / 32);
      unsigned int offset = int(size / 32);

      unsigned int ir_shift = int(i % 32);
      unsigned int i32_index = int(i / 32);
      unsigned int a_ir_shifted = int(a[i32_index] >> ir_shift);
      unsigned int bit = int(a_ir_shifted & (a[j32_index + offset] >> jr_shift) & 1);
      b[out_index] ^=  bit << ijl_shift; // Magic centaurian operation
    }
//        << 7             a[0] >> 1            a[1] >> 6           0x1
// [1]0 0 0 0 0 0 0  = 0 0 1 1 0 1[1]0  &  1[1]0 0 1 0 0 1  &  0 0 0 0 0 0 0[1]
//  0 1 0 1 0 1 1 0  ^= 1 0 0 0 0 0 0 0
// xor a^a = 0 a^b = 1
  return b;
}

unsigned int *decode(const unsigned int *a, int size)
{
  unsigned int *b = new unsigned int[size / 16]; // <- output tab

  for (int i = 0; i < size / 16; i++)
  { // Write size / 16 zeros to b
    b[i] = a[i];
  }

  for (int i = size-1; i >= 0; i--)
    for (int j = size-1; j >= 0; j--)
    {
      unsigned int jr_shift = int(j % 32);
      unsigned int ijl_shift = int((i + j) % 32);
      unsigned int out_index = int((i + j) / 32);
      unsigned int j32_index = int(j / 32);
      unsigned int offset = int(size / 32);

      unsigned int ir_shift = int(i % 32);
      unsigned int i32_index = int(i / 32);
      unsigned int a_ir_shifted = int(a[i32_index] >> ir_shift);
      unsigned int bit = 1 << ijl_shift & b[out_index];

      b[out_index] ^=  bit // Magic centaurian operation
    }
  

  return b;
}

int main()
{
  int size = 32;

  // cin >> size;

  unsigned int *a = new unsigned int[size / 16]; // <- input tab to encrypt

  a[0] = 0x000073af;
  a[1] = 0x0;
  // for (int i = 0; i < size / 16; i++)
  // { // Read size / 16 integers to a
  //   cin >> hex >> a[i];
  // }

  unsigned int *d = decode(a, size);

  unsigned int *b = encode(d, size);

  for (int i = 0; i < size / 16; i++)
  {
    if (a[i] != b[i])
    {
      cerr << "values don't match: b:[" << setfill('0') << setw(8) << hex << b[i] << "] a:[" << setfill('0') << setw(8) << hex << a[i] << "]" << endl; // print result
    }

    if (i > 0)
    {
      cout << ' ';
    }
    cout << setfill('0') << setw(8) << hex << b[i]; // print result
  }
  cout << endl;

  /*
    Good luck humans
 */
  return 0;
}
