#!/bin/bash

xcodebuild -project ../../Builds/MacOSX/Dlay.xcodeproj -configuration Release -scheme "Dlay - All"
packagesbuild DLay.pkgproj
dmgcanvas dlay.dmgCanvas Dlay.dmg
