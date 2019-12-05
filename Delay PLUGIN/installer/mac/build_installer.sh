#!/bin/bash

xcodebuild -project ../Builds/MacOSX/Delay\ PLUGIN.xcodeproj -configuration Release -scheme "Delay PLUGIN - All"
packagesbuild Delay\ Plugin.pkgproj
dmgcanvas Delay.dmgCanvas DyanDelay.dmg

