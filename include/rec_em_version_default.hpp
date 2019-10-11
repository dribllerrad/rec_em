/*
 *  /include/rec_em_version_default.hpp  [Default versioning header]
 *
 *	Copyright (c) 2019 Darrell Bird.  All Rights Reserved.
 *
 *  No part of this file may be reproduced or transmitted in any form by
 *  any means or for any purpose without expressed written permission.
 *
 *
 *
 *
 *  rec_em:
 *  - Creates a c++ header from an input file as an unsigned char
 *    array which can be used as an embedded resource in another binary.
 *    The unsigned char array will be wrapped in a namespace.  Also in the
 *    namespace will be the name of the input file used to generate it and
 *    the length of the array data as an unsigned int.
 *
 *  - Can recursively convert all the input files of a given folder into a
 * 	  single header.
 *
 *  Author:
 *  - Darrell Bird (bird.darrell@gmail.com)
 *
 * This is a default version of the generated header file
 * to avoid errors IDE parsing errors in the case that rec_em
 * has not been built.  During the build, a current version
 * of this file is generated in the build directory and is
 * used in compilation.
 *
 *
 */


#pragma once

namespace rec_em
{

#define PROJECT_NAME "rec_em"
#define REC_EM_VERSION "1.0.0.0"
#define MAJOR_VERSION 1
#define MINOR_VERSION 0
#define PATCH_VERSION 0
#define BUILD_NUMBER 1
#define GIT_COMMIT "0"

    struct Version
    {
        constexpr static auto project_name = PROJECT_NAME;
        constexpr static auto project_version = REC_EM_VERSION;
        constexpr static int major_version = MAJOR_VERSION;
        constexpr static int minor_version = MINOR_VERSION;
        constexpr static int patch_version = PATCH_VERSION;
        constexpr static int build_number = BUILD_NUMBER;
        constexpr static auto git_commit = GIT_COMMIT;
    };

    inline constexpr Version version;

} // namespace rec_em
