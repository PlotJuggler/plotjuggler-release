Name:           ros-kinetic-plotjuggler
Version:        0.10.2
Release:        0%{?dist}
Summary:        ROS plotjuggler package

Group:          Development/Libraries
License:        LGPLv3
URL:            https://github.com/facontidavide/PlotJuggler
Source0:        %{name}-%{version}.tar.gz

Requires:       qt5-qtbase-devel
Requires:       ros-kinetic-ros-type-introspection
Requires:       ros-kinetic-rosbag
Requires:       ros-kinetic-rosbag-storage
Requires:       ros-kinetic-roscpp
Requires:       ros-kinetic-roscpp-serialization
Requires:       ros-kinetic-rostime
Requires:       ros-kinetic-topic-tools
BuildRequires:  qt5-qtbase-devel
BuildRequires:  ros-kinetic-catkin
BuildRequires:  ros-kinetic-ros-type-introspection
BuildRequires:  ros-kinetic-rosbag
BuildRequires:  ros-kinetic-rosbag-storage
BuildRequires:  ros-kinetic-roscpp
BuildRequires:  ros-kinetic-roscpp-serialization
BuildRequires:  ros-kinetic-rostime
BuildRequires:  ros-kinetic-topic-tools

%description
PlotJuggler: juggle with data

%prep
%setup -q

%build
# In case we're installing to a non-standard location, look for a setup.sh
# in the install tree that was dropped by catkin, and source it.  It will
# set things like CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
if [ -f "/opt/ros/kinetic/setup.sh" ]; then . "/opt/ros/kinetic/setup.sh"; fi
mkdir -p obj-%{_target_platform} && cd obj-%{_target_platform}
%cmake .. \
        -UINCLUDE_INSTALL_DIR \
        -ULIB_INSTALL_DIR \
        -USYSCONF_INSTALL_DIR \
        -USHARE_INSTALL_PREFIX \
        -ULIB_SUFFIX \
        -DCMAKE_INSTALL_LIBDIR="lib" \
        -DCMAKE_INSTALL_PREFIX="/opt/ros/kinetic" \
        -DCMAKE_PREFIX_PATH="/opt/ros/kinetic" \
        -DSETUPTOOLS_DEB_LAYOUT=OFF \
        -DCATKIN_BUILD_BINARY_PACKAGE="1" \

make %{?_smp_mflags}

%install
# In case we're installing to a non-standard location, look for a setup.sh
# in the install tree that was dropped by catkin, and source it.  It will
# set things like CMAKE_PREFIX_PATH, PKG_CONFIG_PATH, and PYTHONPATH.
if [ -f "/opt/ros/kinetic/setup.sh" ]; then . "/opt/ros/kinetic/setup.sh"; fi
cd obj-%{_target_platform}
make %{?_smp_mflags} install DESTDIR=%{buildroot}

%files
/opt/ros/kinetic

%changelog
* Tue Feb 14 2017 Davide Faconti <davide.faconti@gmail.com> - 0.10.2-0
- Autogenerated by Bloom

* Tue Feb 14 2017 Davide Faconti <davide.faconti@gmail.com> - 0.10.1-0
- Autogenerated by Bloom

* Sun Feb 12 2017 Davide Faconti <davide.faconti@gmail.com> - 0.10.0-0
- Autogenerated by Bloom

* Thu Feb 09 2017 Davide Faconti <davide.faconti@gmail.com> - 0.9.1-0
- Autogenerated by Bloom

* Tue Feb 07 2017 Davide Faconti <davide.faconti@gmail.com> - 0.9.0-2
- Autogenerated by Bloom

* Tue Feb 07 2017 Davide Faconti <davide.faconti@gmail.com> - 0.9.0-1
- Autogenerated by Bloom

* Tue Feb 07 2017 Davide Faconti <davide.faconti@gmail.com> - 0.9.0-0
- Autogenerated by Bloom

* Tue Jan 24 2017 Davide Faconti <davide.faconti@gmail.com> - 0.8.1-0
- Autogenerated by Bloom

