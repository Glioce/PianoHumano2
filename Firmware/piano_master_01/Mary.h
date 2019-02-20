// Mary Had a Litlle Lamb

byte     maryAdr[] = {  1,  2,  3,  4,    3,  2,  1,  2,  3,  3,  3,   2,  2,  2,   3,  4,  4,   3,  2,  1,  2,  3,  3,  3,   2,  2,  3,  2,  1};
//char   maryCom[] = {'N','N','N','N',  'T','T','T','T','T','T','T', 'T','T','T', 'T','T','T', 'T','T','T','T','T','T','T', 'T','T','T','T','T'};
char     maryCom[] = "NNNNTTTTTTTTTTTTTTTTTTTTTTTTT";
uint16_t maryVal[] = { 67, 69, 71, 74,  150,150,150,150,150,150,350, 150,150,350, 150,150,350, 150,150,150,150,150,150,350, 150,150,150,150,350};
uint16_t maryDel[] = {  0,  0,  0,  0,  200,200,200,200,200,200,400, 200,200,400, 200,200,400, 200,200,200,200,200,200,400, 200,200,200,200,400};

Tune Mary("Mary Had a Little Lamb", sizeof(maryAdr),
maryAdr, maryCom, maryVal, maryDel);
