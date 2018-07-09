// Clase para almacenar las canciones

class Tune {
  public:
    String name; //name or title
    uint16_t len; //length
    byte *adr; //array of addresses
    char *com; //array of commands
    uint16_t *val; //array of values or arguments
    uint16_t *del; //array of delay times

    // Constructor
    Tune(String n, uint16_t l, byte a[],
      char c[], uint16_t v[], uint16_t d[])
      : name(n), len(l), adr(a), com(c), val(v), del(d) {}
    
    /*Tune(String n, unsigned int l, byte *a,
         byte *c, unsigned int *v, unsigned int *d)
      : name(n), len(l), adr(a), com(c), val(v), del(d) {}
      */
};
