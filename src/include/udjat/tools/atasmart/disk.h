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

 #pragma once

 #include <udjat/defs.h>
 #include <udjat/tools/temperature.h>
 #include <string>
 #include <atasmart.h>

 namespace Udjat {

	namespace Smart {

		/// @brief S.M.A.R.T. disk abstraction.
		class UDJAT_API Disk {
		private:
			SkDisk *d;

		public:
			Disk(const char *name);
			~Disk();

			Disk & read();
			const SkIdentifyParsedData * identify();
			SkSmartOverall getOverral();

			uint64_t size();
			uint64_t badsectors();

			/// @brief get the power on time.
			uint64_t poweron();

			/// @brief get the power cycle count.
			uint64_t powercicle();

			/// @brief get the number of bad sectors (i.e. pending and reallocated).
			Udjat::Temperature temperature();

			/// @brief Is the disk awake?
			bool is_awake();

			/// @brief Is identify available?
			bool identify_is_available();

			std::string formattedSize();

		};

	}

 }

