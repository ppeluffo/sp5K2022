/*
 * sp5K2022_tkCmd.c
 *
 *  Created on: 28 jul. 2022
 *      Author: pablo
 */


#include "sp5K2022.h"


static void cmdClsFunction(void);
static void cmdHelpFunction(void);
static void cmdResetFunction(void);
static void cmdStatusFunction(void);
static void cmdWriteFunction(void);
static void cmdReadFunction(void);
static void cmdConfigFunction(void);
static void cmdTestFunction(void);

static void pv_snprintfP_OK(void );
static void pv_snprintfP_ERR(void );

//------------------------------------------------------------------------------
void tkCmd(void * pvParameters)
{

	// Esta es la primer tarea que arranca.

( void ) pvParameters;

    while ( ! starting_flag )
        vTaskDelay( ( TickType_t)( 100 / portTICK_PERIOD_MS ) );

uint8_t c = 0;

    FRTOS_CMD_init();

    FRTOS_CMD_register( "cls", cmdClsFunction );
	FRTOS_CMD_register( "help", cmdHelpFunction );
    FRTOS_CMD_register( "reset", cmdResetFunction );
    FRTOS_CMD_register( "status", cmdStatusFunction );
    FRTOS_CMD_register( "write", cmdWriteFunction );
    FRTOS_CMD_register( "read", cmdReadFunction );
    FRTOS_CMD_register( "config", cmdConfigFunction );
    FRTOS_CMD_register( "test", cmdTestFunction );

    xprintf_P(PSTR("Starting tkCmd..\r\n" ));
    xprintf_P(PSTR("Spymovil %s %s %s %s \r\n") , HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);

	// loop
	for( ;; )
	{
        //kick_wdt(CMD_WDG_bp);

		c = '\0';	// Lo borro para que luego del un CR no resetee siempre el timer.
		// el read se bloquea 50ms. lo que genera la espera.
		while ( frtos_read( fdTERM, (char *)&c, 1 ) == 1 ) {
            FRTOS_CMD_process(c);
        }
        //vTaskDelay( ( TickType_t)( 10 / portTICK_PERIOD_MS ) );
	}
}
//------------------------------------------------------------------------------
static void cmdTestFunction(void)
{

    FRTOS_CMD_makeArgv();

int8_t i;
char c;
float f;
char s1[20];

	xprintf("Test function\r\n");
    i=10;
    xprintf("Integer: %d\r\n", i);
    c='P';
    xprintf("Char: %c\r\n", c);
    f=12.32;
    xprintf("FLoat: %0.3f\r\n", f);

    strncpy(s1,"Pablo Peluffo", 20);
    xprintf("String: %s\r\n", s1);
    //
    xprintf("Todo junto: [d]=%d, [c]=%c, [s]=%s, [f]=%0.3f\r\n",i,c,s1,f);

    // STRINGS IN ROM

    xprintf_P(PSTR("Strings in ROM\r\n"));
    i=11;
    xprintf_P(PSTR("Integer: %d\r\n"), i);
    c='Q';
    xprintf_P(PSTR("Char: %c\r\n"), c);

    f=15.563;
    xprintf_P(PSTR("FLoat: %0.3f\r\n"), f);
    strncpy(s1,"Keynet Spymovil", 20);
    xprintf_P(PSTR("String: %s\r\n"), s1);
    //
    xprintf_P(PSTR("Todo junto: [d]=%d, [c]=%c, [s]=%s, [f]=%0.3f\r\n"),i,c,s1,f);

    // DEFINED
    xprintf("Spymovil %s %s %s %s \r\n" , HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);

}
//------------------------------------------------------------------------------
static void cmdHelpFunction(void)
{

    FRTOS_CMD_makeArgv();

    if ( strcmp( strupr(argv[1]), "WRITE") == 0 ) {
    	xprintf_P( PSTR("-write:\r\n"));
		xprintf_P( PSTR("  ee {pos} {string}\r\n"));
		xprintf_P( PSTR("  led {d1,d2,d3} {on,off}\r\n"));
		xprintf_P( PSTR("  pwrsensor {on,off}\r\n"));
		xprintf_P( PSTR("  pwranalog {on,off}\r\n"));

    }  else if ( strcmp( strupr(argv[1]), "CONFIG") == 0 ) {
    	xprintf_P( PSTR("-config:\r\n"));
    	xprintf_P( PSTR("  debugI2C {on,off}\r\n"));

    }  else if ( strcmp( strupr(argv[1]), "READ") == 0 ) {
    	xprintf_P( PSTR("-read:\r\n"));
		xprintf_P( PSTR("  ee {pos} {lenght}\r\n"));
		xprintf_P( PSTR("  adc {ch}\r\n"));

    }  else {
        // HELP GENERAL
    	xprintf_P( PSTR("Available commands are:\r\n"));
    	xprintf_P( PSTR("-cls\r\n"));
    	xprintf_P( PSTR("-help\r\n"));
    	xprintf_P( PSTR("-status\r\n"));
    	xprintf_P( PSTR("-reset\r\n"));
    	xprintf_P( PSTR("-write...\r\n"));
    	xprintf_P( PSTR("-config...\r\n"));
    	xprintf_P( PSTR("-read...\r\n"));

    }

	xprintf("Exit help \r\n");

}
//------------------------------------------------------------------------------
static void cmdClsFunction(void)
{
	// ESC [ 2 J
	xprintf("\x1B[2J\0");
}
//------------------------------------------------------------------------------
static void cmdResetFunction(void)
{
    xprintf("Reset..\r\n");
    reset();
}
//------------------------------------------------------------------------------
static void cmdStatusFunction(void)
{

    // https://stackoverflow.com/questions/12844117/printing-defined-constants

    xprintf("Spymovil %s %s %s %s \r\n" , HW_MODELO, FRTOS_VERSION, FW_REV, FW_DATE);

}
//------------------------------------------------------------------------------
static void cmdWriteFunction(void)
{

    FRTOS_CMD_makeArgv();

    // PWRANALOG
    // write pwranalog {on,off}
	if (!strcmp_P( strupr(argv[1]), PSTR("PWRANALOG")) ) {
		if (!strcmp_P( strupr(argv[2]), PSTR("ON")) ) {
			SET_OANALOG() ? pv_snprintfP_OK(): pv_snprintfP_ERR();
			return;
		}
		if (!strcmp_P( strupr(argv[2]), PSTR("OFF")) ) {
			CLEAR_OANALOG() ? pv_snprintfP_OK(): pv_snprintfP_ERR();
			return;
		}
		pv_snprintfP_ERR();
		return;
	}

    // PWRSENSOR
    // write pwrsensor {on,off}
	if (!strcmp_P( strupr(argv[1]), PSTR("PWRSENSOR")) ) {
		if (!strcmp_P( strupr(argv[2]), PSTR("ON")) ) {
			SET_OPWRSENSORS() ? pv_snprintfP_OK(): pv_snprintfP_ERR();
			return;
		}
		if (!strcmp_P( strupr(argv[2]), PSTR("OFF")) ) {
			CLEAR_OPWRSENSORS() ? pv_snprintfP_OK(): pv_snprintfP_ERR();
			return;
		}
		pv_snprintfP_ERR();
		return;
	}

    // LED
    // write led {d1,d2,d3} {on,off}
	if (!strcmp_P( strupr(argv[1]), PSTR("LED")) ) {
		if (!strcmp_P( strupr(argv[2]), PSTR("D1")) ) {
			if (!strcmp_P( strupr(argv[3]), PSTR("ON")) ) {
				SET_LED_D1();
				pv_snprintfP_OK();
				return;
			}
			if (!strcmp_P( strupr(argv[3]), PSTR("OFF")) ) {
				CLEAR_LED_D1();
				pv_snprintfP_OK();
				return;
			}
			pv_snprintfP_ERR();
			return;
		}

		if (!strcmp_P( strupr(argv[2]), PSTR("D2")) ) {
			if (!strcmp_P( strupr(argv[3]), PSTR("ON")) ) {
				SET_LED_D2();
				pv_snprintfP_OK();
				return;
			}
			if (!strcmp_P( strupr(argv[3]), PSTR("OFF")) ) {
				CLEAR_LED_D2();
				pv_snprintfP_OK();
				return;
			}
			pv_snprintfP_ERR();
			return;
		}

		if (!strcmp_P( strupr(argv[2]), PSTR("D3")) ) {
			if (!strcmp_P( strupr(argv[3]), PSTR("ON")) ) {
				SET_LED_D3();
				pv_snprintfP_OK();
				return;
			}
			if (!strcmp_P( strupr(argv[3]), PSTR("OFF")) ) {
				CLEAR_LED_D3();
				pv_snprintfP_OK();
				return;
			}
			pv_snprintfP_ERR();
			return;
		}

		pv_snprintfP_ERR();
		return;
	}

	// EE
	// write ee pos string
	if ((strcmp_P( strupr(argv[1]), PSTR("EE\0")) == 0) ) {
		( EE_test_write ( argv[2], argv[3] ) > 0)?  pv_snprintfP_OK() : pv_snprintfP_ERR();
		return;
	}

    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	pv_snprintfP_ERR();
	pv_snprintfP_OK();
	return;

}
//------------------------------------------------------------------------------
static void cmdReadFunction(void)
{

    FRTOS_CMD_makeArgv();

	// ADC
	// read adc channel
	if (!strcmp_P( strupr(argv[1]), PSTR("ADC\0")) ) {
		ADC7828_test_read ( argv[2] ) ? pv_snprintfP_OK() : pv_snprintfP_ERR();
		return;
	}

	// EE
	// read ee address length
	if (!strcmp_P( strupr(argv[1]), PSTR("EE\0")) ) {
		EE_test_read ( argv[2], argv[3] );
		pv_snprintfP_OK();
		return;
	}

    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;

}
//------------------------------------------------------------------------------
static void cmdConfigFunction(void)
{

    FRTOS_CMD_makeArgv();

	// config debugi2c on,off
	if (!strcmp_P( strupr(argv[1]), PSTR("DEBUGI2C")) ) {
		if (!strcmp_P( strupr(argv[2]), PSTR("ON")) ) {
			frtos_ioctl(fdI2C, ioctl_I2C_SET_DEBUG, NULL );
			pv_snprintfP_OK();
			return;
		}
		if (!strcmp_P( strupr(argv[2]), PSTR("OFF")) ) {
			frtos_ioctl(fdI2C, ioctl_I2C_CLEAR_DEBUG, NULL );
			pv_snprintfP_OK();
			return;
		}
		pv_snprintfP_ERR();
		return;
	}

    // CMD NOT FOUND
	xprintf("ERROR\r\nCMD NOT DEFINED\r\n\0");
	return;

}
//------------------------------------------------------------------------------
static void pv_snprintfP_OK(void )
{
	xprintf("ok\r\n\0");
}
//------------------------------------------------------------------------------
static void pv_snprintfP_ERR(void)
{
	xprintf("error\r\n\0");
}
//------------------------------------------------------------------------------

