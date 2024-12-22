/*
 * nunchuk library
 */
#ifndef NUNCHUK_H_
#define NUNCHUK_H_

#include <inttypes.h>

#define IDLEN		4 // bytes

struct ncState {
	uint8_t		joy_x_axis;
	uint8_t		joy_y_axis;
	uint16_t	accel_x_axis;
	uint16_t	accel_y_axis;
	uint16_t	accel_z_axis;
	uint8_t		z_button;
	uint8_t		c_button;
};

struct ncCal {
	uint16_t	x0;
	uint16_t	y0;
	uint16_t	z0;
	uint16_t	x1;
	uint16_t	y1;
	uint16_t	z1;
	uint8_t		xmin;
	uint8_t		xmax;
	uint8_t		xcenter;
	uint8_t		ymin;
	uint8_t		ymax;
	uint8_t		ycenter;
	uint16_t	chksum;
};

class NunChuk {
	public:
		NunChuk();
		bool		begin(uint8_t address);
		bool		getState(uint8_t address);
		ncState		state;
		char		id[2*IDLEN+3]; // '0xAABBCCDD\0'
		static uint8_t	buffer[];

		uint16_t centerValue = 128;
		uint8_t deadzone = 20;

<<<<<<< Updated upstream
		bool init_nunchuck(int NUNCHUK_ADDRESS);
        bool nunchuck_show_state_TEST(int NUNCHUK_ADDRESS, int NUNCHUCK_WAIT);
	private:
		bool	_getId(uint8_t address);
		uint8_t _read(uint8_t address, uint8_t offset, uint8_t len);
=======
    ncState state;
    char id[2 * IDLEN + 3] = {}; // Format: "0xAABBCCDD\0"

    static uint8_t buffer[];

    uint16_t centerValue = 128;
    uint8_t deadzone = 20;

    
    bool init_nunchuck();
    bool nunchuck_show_state_TEST();

private:
    bool _getId(uint8_t address);
    uint8_t _read(uint8_t address, uint8_t offset, uint8_t len, uint8_t* buffer);
>>>>>>> Stashed changes
};

// Nunchuk globaly declared in Nunchuk.cpp
extern NunChuk Nunchuk;

#endif
