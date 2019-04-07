#pragma once
#pragma pack(push,1) 
struct MultibootInfo{
    unsigned flags; 
    unsigned memLower; //always 640K 
    unsigned memUpper; //amount of memory above 1MB 
    unsigned long long mbiFramebufferAddress; 
    unsigned mbiFramebufferPitch; 
    unsigned mbiFramebufferWidth; 
    unsigned mbiFramebufferHeight;
    unsigned char mbiFramebufferBpp;
    unsigned char mbiFramebufferType;
    unsigned char mbiFramebufferRedPos;
    unsigned char mbiFramebufferRedMask;
    unsigned char mbiFramebufferGreenPos;
    unsigned char mbiFramebufferGreenMask;
    unsigned char mbiFramebufferBluePos;
    unsigned char mbiFramebufferBlueMask;
};
#pragma pack(pop)