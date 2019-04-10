#pragma once

class LShape
{
	size_t up;		// coordinates defining Lshape
	size_t bend;
	size_t side;

public:

	size_t getUp() const { return up; }
	size_t getBend()const { return bend; }
	size_t getSide()const { return side; }
	void setUp(size_t a) { up = a; return; }
	void setBend(size_t a) { bend = a; return; }
	void setSide(size_t a) { side = a; return; }

};