#include <p16f690.inc>

__CONFIG(1, _INTRC_OSC_NOCLKOUT);
__CONFIG(2, _WDT_OFF);
__CONFIG(3, _PWRTE_ON);
__CONFIG(4, _MCLRE_OFF);
__CONFIG(5, _CP_OFF);
__CONFIG(6, _CPD_OFF);
__CONFIG(7, _BOR_ON);
__CONFIG(8, _IESO_OFF)
__CONFIG(9, _FCMEN_OFF);

unsigned char previous_input;
unsigned char current_input;
unsigned char changed_input;

unsigned char input_rise[16];
unsigned char input_value[16];

void main()
{
  STATUS &= 0x00;
  PORTA &= 0x00;
  PORTB &= 0x00;
  PORTC &= 0x00;

  STATUS |= 1 << RP0;
  OSCCON |= 0xE0;

  OPTION_REG |= 0x80;

  TRISA = WPUA = IOCA = 0xFF;
  TRISB = 0xEF;
  TRISC = 0xC1;

  TXSTA &= 0x24;
  BAUDCTL &= 0x08;

  SPBRG &= 0x10;
  SPBRGH &= 0x00;

  STATUS &= 0 << RP0;
  STATUS |= 1 << RP1;

  ANSEL &= 0x00;
  ANSELH &= 0x00;

  WPUB = IOCB = 0x50;
  STATUS &= 0x00;

  RCSTA |= 0x90;
  T1CON |= 0x11;
  INTCON |= 0xC8;
}

void interrupt IRS(void)
{
  unsigned char timer_h, timer_h;
  int i;

  if (RABIF == 0) return;

  timer_l = TMR1L;
  timer_h = TMR1H;

  if (TMR1H - timer_h != 0) {
    timer_h = TMR1H;
    timer_l = TMR1L;
  }

  current_input = PORTA;
  if (RB6) current |= 1 << 6;
  if (RB4) current |= 1 << 7;

  INTCON &= 0xFE; // Clears RABIF bit

  changed_input = current_input ^ previous_input;
  previous_input = current_input;

  for (i = 0; i < 16; i++) {
    if (input_mask & input_changed == 0) {
      input_mask << 1;
      i++; // check it
      continue;
    }

    if (input_current & input_mask != 0) { // Input Rise
      input_rise[i] = timer_l;
      i++;
      input_rise[i] = timer_h;
    } else {
      input_value[i] = timer_l - input_rise[i];
      i++;
      input_value[i] = timer_h - input_rise[i];
    }
  }

}
