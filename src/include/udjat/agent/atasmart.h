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
 #include <udjat/agent/abstract.h>
 #include <udjat/agent.h>
 #include <memory>
 
 namespace Udjat {

	namespace Smart {

		/// @brief S.M.A.R.T. agent.
		class UDJAT_API Agent : public Udjat::Agent<unsigned short> {
		private:

			/// @brief Device name.
			const char *devname;

			/// @brief Initialize
			void init();

		public:

			class Factory : public Udjat::Abstract::Agent::Factory {
			public:
				Factory(const char *name = "storage") : Udjat::Abstract::Agent::Factory{name} {
				}

				std::shared_ptr<Abstract::Agent> AgentFactory(const Abstract::Object &parent, const XML::Node &node) const override;

			};
			
			Agent(const char *name);
			Agent(const pugi::xml_node &node);
			Agent(const char *name, const pugi::xml_node &node);
			virtual ~Agent();

			/// @brief Get device name.
			inline const char * device() const noexcept {
				return this->devname;
			}

			void start() override;
			
			/// @brief Get device status, update internal state.
			bool refresh() override;

			std::shared_ptr<Abstract::State> computeState() override;

		};

	}

 }

