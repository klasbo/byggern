/*
 * slider.h
 *
 * Created: 10.09.2014 17:08:54
 *  Author: adelaidm
 */ 


#ifndef SLIDER_H_
#define SLIDER_H_

typedef struct {
    unsigned int L;
    unsigned int R;
} SLI_pos_t ;

SLI_pos_t SLI_getSliderPosition(void);


#endif /* SLIDER_H_ */