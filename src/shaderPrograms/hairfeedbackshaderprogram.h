#ifndef HAIRFEEDBACKSHADERPROGRAM_H
#define HAIRFEEDBACKSHADERPROGRAM_H

#include "hairshaderprogram.h"

class HairFeedbackShaderProgram : public HairShaderProgram
{
public:
    virtual GLuint createShaderProgram() override;
};

#endif // HAIRFEEDBACKSHADERPROGRAM_H
