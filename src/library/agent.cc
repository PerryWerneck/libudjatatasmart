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

 /**
  * @brief Implements SMART agent.
  *
  * <http://0pointer.de/blog/projects/being-smart.html>
  * <http://git.0pointer.net/libatasmart.git/tree/skdump.c>
  * <http://git.0pointer.net/libatasmart.git/tree/atasmart.c>
  *
  * @author perry.werneck@gmail.com
  *
  */

 #include <config.h>
 #include <udjat/defs.h>
 #include <udjat/agent/abstract.h>
 #include <udjat/agent.h>
 #include <memory>
 #include <udjat/agent/atasmart.h>
 #include <udjat/tools/string.h>
 #include <udjat/tools/intl.h>
 #include <udjat/tools/atasmart/disk.h>
 
 using namespace std;

 namespace Udjat {

	std::shared_ptr<Abstract::Agent> Smart::Agent::Factory::AgentFactory(const Abstract::Object &, const XML::Node &node) const {
		return make_shared<Smart::Agent>(node);
	}

	static const char * NameFactory(const char * devname) {

		if(devname && *devname) {
			const char * ptr = strrchr(devname,'/');
			if(ptr && *ptr && ptr[1]) {
				return String{ptr+1}.as_quark();
			}


			return String{devname}.as_quark();
		}

		throw runtime_error("Missing required attribute 'device-name'");

	}

	Smart::Agent::Agent(const char *name) : Udjat::Agent<unsigned short>{NameFactory(name)}, devname{name} {
	}

	Smart::Agent::Agent(const pugi::xml_node &node) : Udjat::Agent<unsigned short>{node}, devname{String{node,"device"}.c_str()} {
	}

	Smart::Agent::Agent(const char *name, const pugi::xml_node &node) : Udjat::Agent<unsigned short>{node}, devname{name} {
	}

	Smart::Agent::~Agent() {
	}

	void Smart::Agent::start() {
		super::start((unsigned short) Smart::Disk(devname).read().getOverral());
	}

	bool Smart::Agent::refresh() {
		return set((unsigned short) Smart::Disk(devname).read().getOverral());
	}

	std::shared_ptr<Abstract::State> Smart::Agent::computeState() {

		unsigned short value = super::get();

		// Check registered states.
		for(auto state : states) {
			if(state->compare(value))
				return state;
		}

		// Not found, check the predefined ones.
		static const struct {
			unsigned short	  value;		///< @brief Agent value for the state.
			const char 		* name;			///< @brief State name.
			Udjat::Level	  level;		///< @brief State level.
			const char		* summary;		///< @brief State summary.
			const char		* body;			///< @brief State description
		} predefined_states[] = {

			{
				SK_SMART_OVERALL_GOOD,
				"good",
				Udjat::ready,
				N_( "${name} Health is Good" ),
				""
			},
			{
				SK_SMART_OVERALL_BAD_ATTRIBUTE_IN_THE_PAST,
				"badonthepast",
				Udjat::ready,
				N_( "Pre fail in the past on ${name}" ),
				N_( "At least one pre-fail attribute exceeded its threshold in the past on ${name}" )
			},
			{
				SK_SMART_OVERALL_BAD_SECTOR,
				"badsector",
				Udjat::warning,
				N_( "Bad sector on ${name}" ),
				N_( "At least one bad sector on ${name}" )
			},
			{
				SK_SMART_OVERALL_BAD_ATTRIBUTE_NOW,
				"badattribute",
				Udjat::error,
				N_( "Pre fail exceeded on ${name}" ),
				N_( "At least one pre-fail attribute is exceeding its threshold now on ${name}" )
			},
			{
				SK_SMART_OVERALL_BAD_SECTOR_MANY,
				"manybad",
				Udjat::error,
				N_( "Too many bad sectors on ${name}" ),
				""
			},
			{
				SK_SMART_OVERALL_BAD_STATUS,
				"badstatus",
				Udjat::error,
				N_( "Smart Self Assessment negative on ${name}" ),
				""
			},

		};

		for(const auto &state : predefined_states) {

			if(state.value == value) {

				// Found internal state, use it.
				return Abstract::Agent::StateFactory(
					state.name,
					Udjat::Level::critical,
#ifdef GETTEXT_PACKAGE
					dgettext(GETTEXT_PACKAGE,state.summary),
					dgettext(GETTEXT_PACKAGE,state.body)
#else
					state.summary,
					state.body
#endif
				);

			}

		}

		Logger::String{"Unable to identify state for value '",((int) value),"'"}.warning(name());

		// Still not found, use the default one.
		return Abstract::Agent::computeState();

	}


 }

/*
 #include <config.h>
 #include <udjat/defs.h>

 #include "private.h"

 #include <udjat/agent/abstract.h>
 #include <udjat/tools/quark.h>
 #include <udjat/smart/disk.h>
 #include <udjat/tools/logger.h>
 #include <udjat/tools/intl.h>
 #include <udjat/request.h>
 #include <udjat/tools/disk/stat.h>
 #include <udjat/tools/string.h>
 #include <sys/time.h>

 using Udjat::Quark;

 namespace Udjat {

	static const char * getAgentName(const char * devname) {

		if(devname && *devname) {

			const char * ptr = strrchr(devname,'/');
			if(ptr && (ptr+1))
				return Quark(ptr+1).c_str();
			return "disk";

		}

		throw runtime_error("Missing required attribute 'device-name'");

	}

	Smart::Agent::Agent(const char *n) : Udjat::Agent<unsigned short>(getAgentName(n), -1), devicename(Quark(n).c_str()) {
		init();
	}

	Smart::Agent::Agent(const char *n, const pugi::xml_node &node) : Udjat::Agent<unsigned short>(getAgentName(n), -1), devicename(Quark(n).c_str()) {

		init();

		if(Attribute(node,"diskstats",true).as_bool(false)) {

			unit = Udjat::Disk::Unit::get(node);
			Udjat::Disk::Stat(devicename).reset(stats);

		}

	}

	Smart::Agent::Agent(const pugi::xml_node &node)
		: Agent(node.attribute("device-name").as_string(),node) {
	}

	std::shared_ptr<Abstract::State> Smart::Agent::computeState() {

	}

	void Smart::Agent::init() {

		Object::properties.icon = "drive-harddisk";

		if(!(Object::properties.label && *Object::properties.label)) {
			string label{"Hard disk "};

			const char * ptr = strrchr(this->devicename,'/');
			if(ptr) {
				label += (ptr+1);
			} else {
				label += devicename;
			}

			Object::properties.label = Quark(label).c_str();
		}

		// Get data from disk.

		try {

			Smart::Disk disk(devicename);

			auto ipd = disk.read().identify();

			string summary{ipd->model};

			try {

				string sz = disk.formattedSize();

				summary += " (" + sz + ")";

			} catch(const std::exception &e) {

				error() << e.what();

			}

			Object::properties.summary = Quark(summary).c_str();

		} catch(const std::exception &e) {

			error() << Logger::Message("Error '{}' getting device information",e) << endl;

		}

	}

	/// @brief Get device status, update internal state.
	bool Smart::Agent::refresh() {

		try {

			set(Smart::Disk(devicename).read().getOverral());

		} catch(const std::exception &e) {

			failed(Logger::Message(_("Can't get overall state of {}"),devicename).c_str(), e);

		}

		if(unit) {
			Udjat::Disk::Stat(devicename).compute(stats);

#ifdef DEBUG
			trace() << "Read=" << (stats.read / unit->value) << " Write=" << (stats.write / unit->value) << endl;
#endif // DEBUG
		}

		return true;

	}

	/// @brief Export device info.
	Udjat::Value & Smart::Agent::getProperties(Udjat::Value &value) const noexcept {

		super::getProperties(value);

		try {

			Smart::Disk disk(devicename);
			disk.read();

			value["temperature"] = disk.temperature().to_string().c_str();
			value["size"] = disk.formattedSize();

			if(disk.identify_is_available()) {
				auto ipd = disk.read().identify();
				value["serial"] = ipd->serial;
				value["firmware"] = ipd->firmware;
				value["model"] = ipd->model;
			} else {
				value["serial"] = "";
				value["firmware"] = "";
				value["model"] = "";
			}

			value["badsectors"] = disk.badsectors();
			value["poweron"] = disk.poweron();
			value["powercicle"] = disk.powercicle();

			if(unit) {
				value["read"] = stats.read / unit->value;
				value["write"] = stats.write / unit->value;
			}

		} catch(const exception &e) {

			error() << "Error '" << e.what() << "' getting device info" << endl;

		}

		return value;
	}

	Smart::Agent::~Agent() {
	}


 }
*/

