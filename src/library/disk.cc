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

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/tools/temperature.h>
 #include <udjat/tools/atasmart/disk.h>
 #include <udjat/tools/configuration.h>
 #include <atasmart.h>
 #include <system_error>
 
 using namespace std;

 namespace Udjat {

	 Smart::Disk::Disk(const char *name) : d(nullptr) {

		if(sk_disk_open(name, &d) < 0) {
			throw system_error(errno, system_category(), string{"Can't open "} + name);
		}

	 }

	 Smart::Disk::~Disk() {
		sk_disk_free(d);
	 }

	 Smart::Disk & Smart::Disk::read() {

		// TODO: Reading SMART data might cause the disk to wake up from sleep. Hence from monitoring daemons make sure to call sk_disk_check_power_mode() to check wether the disk is sleeping and skip the read if so

		if(sk_disk_smart_read_data(d) < 0) {
			throw system_error(errno, system_category(), "Can't read S.M.A.R.T. data");
		}
		return *this;
	 }

	bool Smart::Disk::identify_is_available() {

		SkBool available = 0;

		if(sk_disk_identify_is_available(d,&available) < 0) {
			throw system_error(errno, system_category(), "Can't teste if identify is available");
		}

		return available != 0;

	}

	const SkIdentifyParsedData * Smart::Disk::identify() {
		const SkIdentifyParsedData *ipd;
		if(sk_disk_identify_parse(d, &ipd) < 0) {
			throw system_error(errno, system_category(), "Can't parse S.M.A.R.T. identify");
		}
		return ipd;
	}

	SkSmartOverall Smart::Disk::getOverral() {

		SkSmartOverall overall;

		if (sk_disk_smart_get_overall(d, &overall) < 0) {
			throw system_error(errno, system_category(), "Can't get S.M.A.R.T. overall state");
		}

		return overall;

	}

	bool Smart::Disk::is_awake() {
		SkBool awake = 0;

		if(sk_disk_check_sleep_mode(d,&awake) < 0) {
			throw system_error(errno, system_category(), "Can't get disk awake state");
		}

		return awake != 0;

	}

	uint64_t Smart::Disk::size() {

		uint64_t value;

		if(sk_disk_get_size(d,&value) < 0) {
			throw system_error(errno, system_category(), "Can't get S.M.A.R.T. disk size");
		}

		return value;

	}

	uint64_t Smart::Disk::badsectors() {

		uint64_t value;

		if(sk_disk_smart_get_bad(d,&value) < 0) {
			throw system_error(errno, system_category(), "Can't get bad sectors");
		}

		return value;

	}

	uint64_t Smart::Disk::poweron() {

		uint64_t mseconds;

		if(sk_disk_smart_get_power_on(d,&mseconds) < 0) {
			if(errno == ENOENT) {
				return 0;
			}
			throw system_error(errno, system_category(), "Can't get power on");
		}

		return mseconds;

	}

	uint64_t Smart::Disk::powercicle() {

		uint64_t value;

		if(sk_disk_smart_get_power_cycle(d,&value) < 0) {
			throw system_error(errno, system_category(), "Can't get power cicle");
		}

		return value;

	}

	Temperature Smart::Disk::temperature() {

		uint64_t value;
		if(sk_disk_smart_get_temperature(d,&value) < 0) {
			if(errno == ENOENT) {
				return Temperature{};
			}
			throw system_error(errno, system_category(), "Can't get temperature");
		}

		// The smart value is in 'Kelvin'
		Temperature temperature{((float) value / 1000), Temperature::Kelvin};

		Config::Value<string> unitname("smart","temperature-unit","C");

		temperature.set((Temperature::Unity) ::toupper(unitname[0]));

		return temperature;
	}

	string Smart::Disk::formattedSize() {

		static const struct {
			uint64_t value;
			const char *name;
		} values[] = {
			{ 1000000000000LL,	"TB"	},
			{ 1000000000LL,		"GB"	},
			{ 1000000LL,		"MB"	},
			{ 1000LL,			"KB"	}
		};

		uint64_t bytes = size();

		for(size_t ix = 0; ix < (sizeof(values)/sizeof(values[0])); ix++) {
			if(bytes > values[ix].value) {
				return std::to_string(bytes / values[ix].value) + " " + values[ix].name;
			}
		}

		return std::to_string(bytes);

	 }

 }
