/* regfunc.h */
extern char * regtohex(uint32_t );
extern uint32_t hextoreg(char *);
extern void regw_u8(volatile uint32_t *, uint8_t, short, short);
extern void regw_u32(volatile uint32_t *, uint32_t, short, short);
