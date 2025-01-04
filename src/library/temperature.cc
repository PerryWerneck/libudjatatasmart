/* SPDX-License-Identifier: LGPL-3.0-or-later */

/*
 * Copyright (C) 2021 Perry Werneck <perry.werneck@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

 #include <udjat/tools/temperature.h>
 #include <sstream>
 #include <cmath>
 #include <iomanip>

 using namespace std;

 namespace Udjat {

	float Temperature::as_celsius() const {

		switch(unity) {
		case Kelvin:
			// https://www.rapidtables.com/convert/temperature/kelvin-to-celsius.html
			return value - 273.15;

		case Celsius:
			return value;

		case Fahrenheit:
			// https://www.rapidtables.com/convert/temperature/fahrenheit-to-celsius.html
			return (value - 32) / 1.8;
		}

		throw runtime_error("Cant convert temperature unity");
	}

	float Temperature::as_kelvin() const {

		switch(unity) {
		case Kelvin:
			return value;

		case Celsius:
			// https://www.rapidtables.com/convert/temperature/celsius-to-kelvin.html
			return value + 273.15;

		case Fahrenheit:
			// https://www.rapidtables.com/convert/temperature/fahrenheit-to-kelvin.html
			return (value + 459.67) * 5 / 9;

		}

		throw runtime_error("Cant convert temperature unity");
	}

	float Temperature::as_fahrenheit() const {

		switch(unity) {
		case Kelvin:
			// https://www.rapidtables.com/convert/temperature/kelvin-to-fahrenheit.html
			return ((value * 9)/5) - 459.67;

		case Celsius:
			// https://www.rapidtables.com/convert/temperature/celsius-to-fahrenheit.html
			return value * 1.8 + 32;

		case Fahrenheit:
			return value;

		}

		throw runtime_error("Cant convert temperature unity");
	}

	Temperature & Temperature::set(const Unity unity) {

		switch(unity) {
		case Kelvin:
			value = as_kelvin();
			break;

		case Celsius:
			value = as_celsius();
			break;

		case Fahrenheit:
			value = as_fahrenheit();
			break;

		default:
			throw runtime_error("Unexpected temperature unity");

		}

		this->unity = unity;

		return *this;

	}

	std::string Temperature::to_string() const {

		if(FP_ZERO == fpclassify(this->value)) {
			return "";
		}

		// https://www.delftstack.com/howto/cpp/how-to-convert-float-to-string-in-cpp/
		// https://www.delftstack.com/howto/cpp/cpp-round-to-2-decimals/
		std::stringstream sstream;
		sstream << setprecision(1) << this->value << " °" << ((char) this->unity);
		return sstream.str();

	}

 }
