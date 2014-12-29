#ifndef CIRCLE_H
#define CIRCLE_H

#include "shape.h"

class Circle : public Shape {
public:
	int area(); 
	double get_length();
	double get_width();

	Circle() : length(0), width(0) {}
	Circle(double length_in, double width_in) 
		: length(length_in), width(width_in) {}

private:
	double length;
	double width; 

}