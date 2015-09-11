%define build_tests 1
%define use_session_bus 0
%define daemon_user messageport
%define daemon_group messageport

Name:       message-port
Summary:    Message port daemon
Version:    1.0.3
Release:    0
Group:      System/Service
License:    LGPL-2.1+
Source0:    %{name}-%{version}.tar.gz
Source1:    %{name}.manifest

BuildRequires: pkgconfig(aul)
BuildRequires: pkgconfig(dlog)
BuildRequires: pkgconfig(gio-2.0)
BuildRequires: pkgconfig(gio-unix-2.0)
BuildRequires: pkgconfig(glib-2.0) >= 2.30
BuildRequires: pkgconfig(gobject-2.0)
BuildRequires: pkgconfig(pkgmgr-info)
BuildRequires: pkgconfig(capi-base-common)

%description
This daemon allows the webapplications to communicates using
Tizen MessagePort WebAPI.


%package -n lib%{name}
Summary:    Client library for message port
Group:      Base/Libraries
Requires(post):   /sbin/ldconfig
Requires(postun): /sbin/ldconfig
Requires: %{name} = %{version}-%{release}
BuildRequires: pkgconfig(bundle)

%description -n lib%{name}
Client library that porvies C APIs to work with message port.


%package -n lib%{name}-devel
Summary:    Development files for libmessage-port
Group:      Development/Libraries
Requires:   lib%{name} = %{version}-%{release}

%description -n lib%{name}-devel
Development files for message-port client library.


%if %{build_tests} == 1

%package -n %{name}-tests
Summary: Unit tests for messageport
Group:   Development/Testing
Requires: lib%{name} = %{version}-%{release}

%description -n %{name}-tests
Unit tests for messageport implementation.

%endif

%prep
%setup -q -n %{name}-%{version}
cp -a %{SOURCE1} .
mkdir -p m4 > /dev/null


%build
%reconfigure \
%if %{build_tests} == 1
     --enable-tests \
%endif
%if %{use_session_bus} == 1
    --enable-sessionbus \
%endif

%__make %{?_smp_mflags}


%install
%make_install

mkdir -p %{buildroot}%{_unitdir}
cp messageportd.service %{buildroot}%{_unitdir}

#DBus DAC (message-port-dbus.manifest enables DBus SMACK)
mkdir -p %{buildroot}%{_sysconfdir}/dbus-1/system.d
cp packaging/message-port.conf %{buildroot}%{_sysconfdir}/dbus-1/system.d/message-port.conf


%post
getent group %{daemon_group} >/dev/null || groupadd -r -o %{daemon_group}
getent passwd %{daemon_user} >/dev/null || useradd -r -g %{daemon_group} -s /bin/false -d /run/%{daemon_user} -c "Message Port daemon" %{daemon_user}

systemctl enable messageportd.service

%postun
systemctl disable messageportd.service

%post -n lib%{name}
ldconfig

%postun -n lib%{name}
ldconfig

# daemon: message-port
%files -n %{name}
%defattr(-,root,root,-)
%{_bindir}/messageportd
%if %{use_session_bus} == 1
%{_datadir}/dbus-1/services/org.tizen.messageport.service
%manifest %{name}.manifest
%endif
%{_unitdir}/messageportd.service
%license COPYING.LIB
%config %attr(644,root,root) %{_sysconfdir}/dbus-1/system.d/*

# libmessage-port
%files -n lib%{name}
%defattr(-,root,root,-)
%manifest %{name}.manifest
%defattr(-,root,root,-)
%doc AUTHORS COPYING.LIB
%{_libdir}/lib%{name}.so.*

#libmessage-port-devel
%files -n lib%{name}-devel
%defattr(-,root,root,-)
%manifest %{name}.manifest
%{_libdir}/pkgconfig/%{name}.pc
%{_libdir}/lib%{name}.so
%{_includedir}/*.h

%if %{build_tests} == 1
%files -n %{name}-tests
%defattr(-,root,root,-)
%manifest %{name}.manifest
%{_bindir}/msgport-test-app
%{_bindir}/msgport-test-app-cpp
%endif
