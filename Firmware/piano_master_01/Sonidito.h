// El Sonidito

byte     soniAdr[] = {  8,   8,8,8,8,8,8,8, 8,8,8,8,8,8,8};
char     soniCom[] = {'N', 'T','T','T','T','T','T','T', 'T','T','T','T','T','T','T'};
uint16_t soniVal[] = { 72, 100,100,100,100,100,100,100, 100,100,100,100,100,100,100};
uint16_t soniDel[] = {  0, 200,200,200,200,200,200,400, 200,200,200,200,200,200,400};

Tune Sonidito("El Sonidito", sizeof(soniAdr), //nombre y longitud
soniAdr, soniCom, soniVal, soniDel); //arreglos
