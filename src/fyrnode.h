// ===========================================================================
// Copyright (C) 2020 Manish Meganathan, Mariyam A.Ghani. All Rights Reserved.
// 
// This file is part of the FyrNode library.
// No part of the FyrNode library can not be copied and/or distributed 
// without the express permission of Manish Meganathan and Mariyam A.Ghani
// ===========================================================================

#pragma once

#ifndef FYRNODE_H_INCLUDED
#define FYRNODE_H_INCLUDED

#include "Arduino.h"

class FyrNode
{
  public:
    FyrNode();
    void begin();
    void update();
};

class FyrNodeControl
{
  public:
    FyrNodeControl();
    void begin();
    void update();
};

#endif