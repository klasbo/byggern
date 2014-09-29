

/*
    
    for (int i = 0; i < 0x800; i++){
        ext_ram[i] = 0;
    }
    
    OLED_printf("test\nmultiline\n  line3\n      right edge\n\n\n\nlower line");
    
    _delay_ms(1000);    
    
    for(int i = 0; i < FONT_WIDTH; i++){
        draw_upper_n(pgm_read_byte(&font['f'-' '][i]), i+100, 30, 5);        
        draw_lower_n(pgm_read_byte(&font['g'-' '][i]), i,     44, 4);
    }
    
    for(int i = 40; i < 80; i++){
        draw_upper_n(1, i, 20, 1);
        draw_upper_n(1, i, 40, 1);
    }
    for(int i = 20; i < 40; i++){
        draw_upper_n(1, 40, i, 1);
        draw_upper_n(1, 80, i, 1);
    }
    
    
    
    OLED_reset();
    render();
    
    _delay_ms(2000);
    
    OLED_printf("\nnew\nmultiline\ntest\nthing\n");
    printf("timer tick, 3: %d\n", (uint16_t)(TCNT3H<<8 | TCNT3L) );
    
    SRAM_test();
*/
