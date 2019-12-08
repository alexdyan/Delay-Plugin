#!/bin/bash

xcodebuild -project ../../Builds/MacOSX/Dlay.xcodeproj -configuration Release -scheme "Dlay - All"
packagesbuild Delay\ Plugin.pkgproj
dmgcanvas Delay.dmgCanvas Dlay.dmg

