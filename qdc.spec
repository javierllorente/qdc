#
# spec file for package qdc
#
# Copyright (c) 2014 SUSE LINUX Products GmbH, Nuernberg, Germany.
# Copyright (c) 2015-2016 Javier Llorente
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#


Name:           qdc
Version:        0.4.99
Release:        0
Summary:        Castilian dictionary
License:        GPL-2.0 or GPL-3.0
Group:          Productivity/Office/Dictionary
Url:            http://www.javierllorente.com/
Source:         %{name}-%{version}.tar.bz2
BuildRequires:  hicolor-icon-theme
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5WebKitWidgets)
BuildRequires:  update-desktop-files
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Obsoletes:      qrae < %{version}
Provides:       qrae = %{version}

%description
A Qt-based Castilian dictionary

%prep
%setup -q -n %{name}

%build
qmake-qt5
make %{?_smp_mflags}

%install
make INSTALL_ROOT=%{buildroot} install
install -m 644 scripts/migrate.sh %{buildroot}%{_datadir}/%{name}/migrate.sh
%suse_update_desktop_file -i %name Dictionary

%post
sh %{_datadir}/%{name}/migrate.sh

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%doc ChangeLog README.md COPYING
%{_bindir}/%{name}
%{_datadir}/applications/%{name}.desktop
%{_datadir}/icons/hicolor/128x128/apps/%{name}.png
%{_datadir}/%{name}

%changelog
