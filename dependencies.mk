#
# Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of lsp-lltl-lib
#
# lsp-lltl-lib is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# lsp-lltl-lib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with lsp-lltl-lib.  If not, see <https://www.gnu.org/licenses/>.
#

# Variables that describe dependencies
LSP_COMMON_LIB_VERSION     := 1.0.12
LSP_COMMON_LIB_NAME        := lsp-common-lib
LSP_COMMON_LIB_TYPE        := hdr
LSP_COMMON_LIB_URL         := https://github.com/sadko4u/$(LSP_COMMON_LIB_NAME).git

LSP_TEST_FW_VERSION        := 1.0.7
LSP_TEST_FW_NAME           := lsp-test-fw
LSP_TEST_FW_TYPE           := src
LSP_TEST_FW_URL            := https://github.com/sadko4u/$(LSP_TEST_FW_NAME).git

LIBPTHREAD_VERSION         := system
LIBPTHREAD_NAME            := libpthread
LIBPTHREAD_TYPE            := opt
LIBPTHREAD_LDFLAGS         := -lpthread

LIBSHLWAPI_VERSION         := system
LIBSHLWAPI_NAME            := libshlwapi
LIBSHLWAPI_TYPE            := opt
LIBSHLWAPI_LDFLAGS         := -lshlwapi
