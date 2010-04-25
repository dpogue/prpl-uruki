#!/bin/sh
echo "Generating configuration files for prpl-uruki, please wait..."
echo;

echo "Running libtoolize, please ignore non-fatal messages."
echo n | libtoolize --copy --force || exit;

aclocal || exit;
autoheader || exit;
automake --add-missing --copy || exit;
autoconf || exit;
automake || exit;

intltoolize --copy --force --automake || exit;

echo;
echo "Done."
