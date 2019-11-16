/*
  ==============================================================================

    LookAndFeelHolder.h
    Created: 15 Nov 2019 12:12:11pm
    Author:  Alexandra D'Yan

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

template<typename T>

class LookAndFeelHolder {
public:
	LookAndFeelHolder() {
		LookAndFeel::setDefaultLookAndFeel(&laf);
	}

private:
	T laf;
};