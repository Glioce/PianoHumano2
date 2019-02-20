// Escala por defecto

byte     escala0Adr[] = {  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14};
char     escala0Com[] = {'N','N','N','N','N','N','N','N','N','N','N','N','N','N'};
uint16_t escala0Val[] = { 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73};
uint16_t escala0Del[] = {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};

Tune Escala0("Escala Default", 14, //nombre y longitud
escala0Adr, escala0Com, escala0Val, escala0Del); //arreglos
