#
# spec file for package libudjatatasmart
#
# Copyright (c) <2024> Perry Werneck <perry.werneck@gmail.com>.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://github.com/PerryWerneck/libudjatatasmart/issues
#

%define module_name atasmart

Summary:		ATA S.M.A.R.T. Disk Health library for %{udjat_product_name}  
Name:			libudjat%{module_name}
Version:		2.0.0
Release:		0
License:		LGPL-3.0
Source:			%{name}-%{version}.tar.xz

URL:			https://github.com/PerryWerneck/libudjat%{module_name}

Group:			Development/Libraries/C and C++
BuildRoot:		/var/tmp/%{name}-%{version}

BuildRequires:	gcc-c++ >= 5
BuildRequires:	pkgconfig(libudjat) >= 2.0.0
BuildRequires:	pkgconfig(libatasmart)
BuildRequires:	pkgconfig(libudjatsysinfo)

BuildRequires:	meson >= 0.61.4
BuildRequires:	udjat-rpm-macros 

%description
ATA S.M.A.R.T. Disk Health Monitoring module for %{udjat_product_name}

Add physical disk state agent(s) to the %{udjat_product_name} agent tree.

%package -n %{udjat_library}
Summary:	HTTP client library for %{product_name}

%description -n %{udjat_library}
ATA S.M.A.R.T. Disk Health Monitoring module for %{udjat_product_name}

C++ Physical disk state classes for use with lib%{product_name}

%package devel
Summary:	Development files for %{name}
%udjat_devel_requires

%description devel
HTTP client library for %{product_name}

C++ Physical disk state classes for use with lib%{product_name}

%lang_package -n %{udjat_library}
%udjat_module_package -n %{module_name}

%prep
%autosetup
%meson

%build
%meson_build

%install
%meson_install
%find_lang %{name}-%{udjat_package_major}.%{udjat_package_minor} langfiles

%files -n %{udjat_library}
%defattr(-,root,root)
%{_libdir}/%{name}.so.%{udjat_package_major}.%{udjat_package_minor}

%files -n %{udjat_library}-lang -f langfiles

%files devel
%defattr(-,root,root)

%{_libdir}/*.so
%{_libdir}/*.a
%{_libdir}/pkgconfig/*.pc

%{_includedir}/udjat/module/*.h
%{_includedir}/udjat/agent/*.h
%{_includedir}/udjat/tools/*.h
%{_includedir}/udjat/tools/atasmart/*.h

%post -n %{udjat_library} -p /sbin/ldconfig

%postun -n %{udjat_library} -p /sbin/ldconfig

%changelog
