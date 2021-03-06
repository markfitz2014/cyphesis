// Cyphesis Online RPG Server and AI Engine
// Copyright (C) 2000,2001 Alistair Riddoch
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA


#ifndef RULESETS_PYTHON_API_H
#define RULESETS_PYTHON_API_H

#include <string>
#include <set>
#include <sigc++/signal.h>
#include <boost/asio/io_service.hpp>

class AssetsManager;

extern sigc::signal<void> python_reload_scripts;

void init_python_api(const std::string & ruleset, bool log_stdout = true);
void shutdown_python_api();

void observe_python_directories(boost::asio::io_service& io_service, AssetsManager& assetsManager);

#endif // RULESETS_PYTHON_API_H
