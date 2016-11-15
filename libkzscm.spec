%if 0%{?fedora} >= 17 || 0%{?rhel} >= 7
%global _with_systemd 1
%else
%global _with_systemd 0
%endif

Name:           libkzscm
Version:        0.6
Release:        20161115gita5a5f3a%{?dist}
Summary:        libkzscm is a C library implementing R4RS Scheme
Group:          Library
License:        GPL
URL:            https://github.com/Karl-Z/libkzscm
Source0:        %name-20161115gita5a5f3a.tar.gz

BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

%ifarch noarch
%define _source_filedigest_algorithm md5
%define _binary_filedigest_algorithm md5
%define _source_payload w0.gzdio
%define _binary_payload w0.gzdio
%endif

%description
libkzscm is a C library implementing Scheme as described in the
Revised^4 Report on the Algorithmic Language Scheme based on libscheme.

It adds following features and improvements:
   * Regexp
   * Posix popen
   * GC 6.8

%prep
%setup -q -n %{name}

%build
# %%configure
make %{?_smp_mflags}

%install
install -d %{buildroot}%{_libdir}
install -d %{buildroot}%{_includedir}/%name
install -d %{buildroot}%{_includedir}/%name
install -m644 libscheme.a %{buildroot}%{_libdir}/
install -m644 scheme.h %{buildroot}%{_includedir}/%name/
install -m644 re/scheme_regexp.h %{buildroot}%{_includedir}/%name/
install -m644 posix/scheme_posix.h %{buildroot}%{_includedir}/%name/

%files
%defattr(644,root,root,755)
%{_includedir}/%name
%{_libdir}/libscheme.a


%clean
rm -rf %{buildroot}

%changelog

* Tue Nov 15 2016 Carl Zhang <carl_zhang@carrefour.com> 0.6-1
- Initial package setup.

