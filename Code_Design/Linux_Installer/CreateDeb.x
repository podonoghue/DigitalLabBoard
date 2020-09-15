#!/bin/bash

# Version Information
MAJOR=2
MINOR=0
MICRO=0
NANO=0
DEBIAN_VERSION=1
VERSION=${MAJOR}.${MINOR}.${MICRO}.${NANO}

ARCH=`uname -m`

SOURCE_DIRECTORY=.

DATE=`date +"%Y%m%d%H%M%S"`

PACKAGE_NAME="cpldprogrammer"
APP_NAME="cpldprogrammer"

#======================================================
# Create DEB file
#
makeDebFile() {

#   CP="cp -pv"
#   MKDIR="mkdir -pv"
#   RM="rm -f"
#   MV="mv -v"
#  GZIP="gzip -v"

   CP="cp -p"
   MKDIR="mkdir -p"
   RM="rm -f"
   MV="mv"
   GZIP="gzip"
   
   # Where to construct the install tree
   DUMMY_ROOT="../${PACKAGE_NAME}_${VERSION}-${DEBIAN_VERSION}-${ARCH}"
   
   # Shared document files
   SHARED_DOC_DIR="${DUMMY_ROOT}/usr/share/doc/${PACKAGE_NAME}"
   
   BIN_DIR="${DUMMY_ROOT}/usr/bin"
   LIB_DIR="${DUMMY_ROOT}/usr/lib/${ARCH}-linux-gnu/${PACKAGE_NAME}"

   if [[ -e ${DUMMY_ROOT} ]]; then
      ${RM} -R ${DUMMY_ROOT}
   fi
   
   ${MKDIR} ${DUMMY_ROOT}
   ${CP} -r "${SOURCE_DIRECTORY}/image/." ${DUMMY_ROOT}   
   ${CP} -r "${SOURCE_DIRECTORY}/../PackageFiles/bin/${ARCH}-linux-gnu/CPLD_ProgrammerApp" "${BIN_DIR}/${APP_NAME}"   
#   ${CP} -rp "${SOURCE_DIRECTORY}/../Shared/${ARCH}-linux-gnu/." "${LIB_DIR}"   

   ${MKDIR} ${SHARED_DOC_DIR}
   ${GZIP} -n -9 -k -c "${SOURCE_DIRECTORY}/../PackageFiles"/changelog >${SHARED_DOC_DIR}/changelog.gz
   ${CP}    "${SOURCE_DIRECTORY}/../PackageFiles"/copyright             ${SHARED_DOC_DIR}

   # Fix permissions
   find ${DUMMY_ROOT}   -type d -exec chmod 755 {} \;
   find ${DUMMY_ROOT}   -type f -name \*.map\* -exec chmod 644 {} \;
   find ${DUMMY_ROOT}   -type f ! -executable -exec chmod 644 {} \;
   find ${DUMMY_ROOT}   -type f -name \*.so\* -exec chmod 644 {} \; -exec strip {} \;
   find ${DUMMY_ROOT}   -type f -executable -exec chmod 755 {} \; -exec strip {} \;
   
   echo "ln -s ./${ARCH}-linux-gnu/${PACKAGE_NAME} ${DUMMY_ROOT}/usr/lib/${PACKAGE_NAME}"
   ln -s ${ARCH}-linux-gnu/${PACKAGE_NAME} ${DUMMY_ROOT}/usr/lib/${PACKAGE_NAME}

   echo fakeroot dpkg-deb --build ${DUMMY_ROOT}
   fakeroot dpkg-deb --build ${DUMMY_ROOT}

   echo lintian ${DUMMY_ROOT}.deb   
   lintian ${DUMMY_ROOT}.deb 

   ${RM} -R ${DUMMY_ROOT}

}

# Build for native ARCH
makeDebFile

exit


