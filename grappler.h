

#ifndef GRAPPLER_H
#define GRAPPLER_H

#ifdef __cplusplus
extern "C" {
#endif

void grap_init();

void grap_open();
void grap_close();
char grap_isOpen();
char grap_isClosed();

/*
 * Internal function. Do not call!
 */
void grap_tick();

#ifdef __cplusplus
}
#endif

#endif //GRAPPLER_H
