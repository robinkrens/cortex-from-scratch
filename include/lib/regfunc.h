/* regfunc.h */
/* DEPRECATED extern char * regtohex(uint32_t ); */
extern uint32_t hextoreg(char *); // TODO: scanf
extern void rsetbit(volatile uint32_t *, short);
extern void rclrbit(volatile uint32_t *, short);
extern void rsetbitsfrom(volatile uint32_t *, short, int);
extern int rchkbit(volatile uint32_t *, short);
extern void rwrite(volatile uint32_t *, uint32_t);
extern void _block(uint16_t us);

/* DEPRECATED 
 * extern void regw_u8(volatile uint32_t *, uint8_t, short, short);
 * extern void regw_u32(volatile uint32_t *, uint32_t, short, short);
*/
